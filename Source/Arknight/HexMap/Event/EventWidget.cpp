// Fill out your copyright notice in the Description page of Project Settings.


#include "EventWidget.h"

#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "EventOptionWidget.h"
#include "HexEventLogicBase.h"
#include "../Controller/HexMapSubsystem.h"

void UEventWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetVisibility(ESlateVisibility::Collapsed);
}

void UEventWidget::OpenEvent(const FHexEventConfig& Config)
{
	if (EventImage)
	{
		EventImage->SetBrushFromSoftTexture(Config.EventImage);
	}

	CachedConfig = Config;

	const TSubclassOf<UHexEventLogicBase> LogicClass = Config.LogicClass;
	if (LogicClass)
	{
		ActiveLogicInstance = NewObject<UHexEventLogicBase>(this, LogicClass);
		if (ActiveLogicInstance)
		{
			ActiveLogicInstance->InitializeLogic(GetGameInstance());
		}
	}
	else
	{
		ActiveLogicInstance = nullptr;
		UE_LOG(LogTemp, Warning, TEXT("EventWidget::OpenEvent: LogicClass is null for event '%s'"), *Config.EventID.ToString());
	}

	// --- Show first stage (handles title/description/options) ---
	ShowStage(0);
	SetVisibility(ESlateVisibility::Visible);
}

void UEventWidget::ShowStage(int32 StageIndex)
{
	if (!OptionsContainer)
	{
		UE_LOG(LogTemp, Warning, TEXT("EventWidget::ShowStage: OptionsContainer is not set"));
		return;
	}
	if (!OptionWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("EventWidget::ShowStage: OptionWidgetClass is not set"));
		return;
	}

	OptionsContainer->ClearChildren();

	// Clamp stage index to valid range (defensive — ShowStage is BlueprintCallable)
	const int32 TotalStages = GetTotalStages();
	CurrentStage = FMath::Clamp(StageIndex, 0, TotalStages - 1);

	const TArray<FHexEventOption>& Options = CachedConfig.Options;
	const TArray<int32>& PerStage = CachedConfig.OptionPerStage;

	if (Options.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("EventWidget::ShowStage: no options in config"));
		return;
	}

	// Calculate option range for this stage
	int32 StartIndex = 0;
	int32 Count = Options.Num(); // default: all options for single-stage
	if (TotalStages > 1 && PerStage.IsValidIndex(CurrentStage))
	{
		// Sum previous stages' option counts to get StartIndex
		for (int32 s = 0; s < CurrentStage; ++s)
		{
			StartIndex += PerStage[s];
		}
		Count = PerStage[CurrentStage];
	}

	// --- Per-stage title and description ---
	if (EventTitleText)
	{
		if (CachedConfig.EventTitle.IsValidIndex(CurrentStage))
		{
			EventTitleText->SetText(CachedConfig.EventTitle[CurrentStage]);
		}
		else if (CachedConfig.EventTitle.Num() > 0)
		{
			EventTitleText->SetText(CachedConfig.EventTitle[0]); // fallback to first
		}
	}
	if (EventDescText)
	{
		if (CachedConfig.EventDescription.IsValidIndex(CurrentStage))
		{
			EventDescText->SetText(CachedConfig.EventDescription[CurrentStage]);
		}
		else if (CachedConfig.EventDescription.Num() > 0)
		{
			EventDescText->SetText( CachedConfig.EventDescription[0]);
		}
	}

	// Spawn option widgets for this stage
	for (int32 i = 0; i < Count; ++i)
	{
		const int32 OptionIndex = StartIndex + i;
		if (!Options.IsValidIndex(OptionIndex))
		{
			UE_LOG(LogTemp, Warning, TEXT("EventWidget::ShowStage: invalid option index %d for stage %d"), OptionIndex, CurrentStage);
			break;
		}

		UEventOptionWidget* OptionWidget = CreateWidget<UEventOptionWidget>(this, OptionWidgetClass);
		if (!OptionWidget)
		{
			UE_LOG(LogTemp, Warning, TEXT("EventWidget::ShowStage: failed to create OptionWidget for option index %d"), OptionIndex);
			continue;
		}

		OptionWidget->InitOption(Options[OptionIndex], OptionIndex);
		OptionWidget->OnOptionSelected.AddDynamic(this, &UEventWidget::HandleOptionSelected);

		OptionsContainer->AddChildToHorizontalBox(OptionWidget);
	}
}

void UEventWidget::HandleOptionSelected(int32 OptionIndex)
{
	if (!ActiveLogicInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("EventWidget::HandleOptionSelected: ActiveLogicInstance is null — closing panel"));
		SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	ActiveLogicInstance->bTriggeredCombat = false;
	ActiveLogicInstance->ExecuteOption(OptionIndex);

	// If ExecuteOption triggered an OpenLevel combat transition, skip closing/refreshing.
	// Otherwise RefreshAllNodeStates (called below) would race ConcludeGame against OpenLevel.
	if (ActiveLogicInstance->bTriggeredCombat) return;

	// Let event logic determine the next stage (supports branches, cycles, termination)
	const int32 TotalStages = GetTotalStages();
	const int32 NextStage = ActiveLogicInstance
		? ActiveLogicInstance->DetermineNextStage(CurrentStage, OptionIndex)
		: CurrentStage + 1;

	if (NextStage < 0 || NextStage >= TotalStages)
	{
		SetVisibility(ESlateVisibility::Collapsed);

		// Trigger node state refresh — may ConcludeGame if AP hit 0 during event
		if (UHexMapSubsystem* HexMap = GetGameInstance()->GetSubsystem<UHexMapSubsystem>())
		{
			HexMap->RefreshAllNodeStates();
		}
	}
	else
	{
		ShowStage(NextStage);
	}
}

int32 UEventWidget::GetTotalStages() const
{
	if (CachedConfig.MultiStageNumber > 0)
	{
		return CachedConfig.MultiStageNumber;
	}
	return 1;
}
