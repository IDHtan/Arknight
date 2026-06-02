// Fill out your copyright notice in the Description page of Project Settings.


#include "EventWidget.h"

#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "EventOptionWidget.h"
#include "HexEventLogicBase.h"

void UEventWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetVisibility(ESlateVisibility::Collapsed);
}

void UEventWidget::OpenEvent(const FHexEventConfig& Config)
{
	// --- Set text and image ---
	if (EventTitleText)
	{
		EventTitleText->SetText(Config.EventTitle);
	}
	if (EventDescText)
	{
		EventDescText->SetText(Config.EventDescription);
	}
	if (EventImage)
	{
		EventImage->SetBrushFromSoftTexture(Config.EventImage);
	}

	// --- Clear and repopulate option widgets ---
	if (OptionsContainer)
	{
		OptionsContainer->ClearChildren();
	}

	if (!OptionWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("EventWidget::OpenEvent: OptionWidgetClass is not set"));
		return;
	}

	const TArray<FHexEventOption>& Options = Config.Options;
	for (int32 i = 0; i < Options.Num(); ++i)
	{
		UEventOptionWidget* OptionWidget = CreateWidget<UEventOptionWidget>(this, OptionWidgetClass);
		if (!OptionWidget)
		{
			continue;
		}

		OptionWidget->InitOption(Options[i], i);
		OptionWidget->OnOptionSelected.AddDynamic(this, &UEventWidget::HandleOptionSelected);

		if (OptionsContainer)
		{
			OptionsContainer->AddChildToHorizontalBox(OptionWidget);
		}
	}

	// --- Instantiate and initialize event logic ---
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

	SetVisibility(ESlateVisibility::Visible);
}

void UEventWidget::HandleOptionSelected(int32 OptionIndex)
{
	if (!ActiveLogicInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("EventWidget::HandleOptionSelected: ActiveLogicInstance is null"));
		return;
	}

	ActiveLogicInstance->ExecuteOption(OptionIndex);

	// Close panel after selection (single-choice events)
	SetVisibility(ESlateVisibility::Collapsed);
}
