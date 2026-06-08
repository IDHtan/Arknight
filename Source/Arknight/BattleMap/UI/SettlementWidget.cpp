// Fill out your copyright notice in the Description page of Project Settings.


#include "SettlementWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/WrapBox.h"
#include "Kismet/GameplayStatics.h"
#include "../../HexMap/Shop/Icon.h"
#include "../../HexMap/Controller/HexMapSubsystem.h"

void USettlementWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ConfirmButton)
	{
		ConfirmButton->OnClicked.AddDynamic(this, &USettlementWidget::OnConfirmClicked);
	}
}

void USettlementWidget::Init(EHexNodeType BattleType,
                              const TMap<EResourceType, int32>& Resources,
                              bool bIsVictory)
{
	UGameplayStatics::SetGamePaused(this, true);
	CachedBattleType = BattleType;
	bCachedIsVictory = bIsVictory;

	// --- Victory/failure text (emergency only) ---
	if (VictoryText)
	{
		if (CachedBattleType == EHexNodeType::Combat_Emergency)
		{
			VictoryText->SetVisibility(ESlateVisibility::Visible);
			VictoryText->SetText(bIsVictory
				? FText::FromString(TEXT("作战成功！"))
				: FText::FromString(TEXT("作战失败，扣除3点生存点数")));
		}
		else
		{
			VictoryText->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	// --- Dynamically create icon + quantity per resource ---
	if (IconsContainer && IconWidgetClass)
	{
		IconsContainer->ClearChildren();

		for (const TPair<EResourceType, int32>& Pair : Resources)
		{
			if (Pair.Value <= 0) continue;

			// Vertical box: icon on top, quantity text below
			UVerticalBox* VBox = NewObject<UVerticalBox>(this);

			UIcon* Icon = CreateWidget<UIcon>(this, IconWidgetClass);
			if (Icon)
			{
				Icon->SetResourceType(Pair.Key);
				VBox->AddChildToVerticalBox(Icon);
			}

			UTextBlock* QtyText = NewObject<UTextBlock>(this);
			QtyText->SetText(FText::AsNumber(Pair.Value));
			QtyText->SetJustification(ETextJustify::Center);
			VBox->AddChildToVerticalBox(QtyText);

			IconsContainer->AddChildToWrapBox(VBox);
		}
	}
}

void USettlementWidget::OnConfirmClicked()
{
	UHexMapSubsystem* HexMap = GetGameInstance()->GetSubsystem<UHexMapSubsystem>();
	if (!HexMap) return;

	if (CachedBattleType == EHexNodeType::Combat_Emergency && !bCachedIsVictory)
	{
		// ChangeCurrentAP no longer auto-fires ConcludeGame.
		// We explicitly branch to avoid competing OpenLevel calls.
		if (HexMap->GetCurrentAP() > 3)
		{
			HexMap->ChangeCurrentAP(-3);
			UGameplayStatics::OpenLevel(this, TEXT("/Game/Maps/Lvl_HexMap"));
		}
		else
		{
			HexMap->ChangeCurrentAP(-3);
			HexMap->ConcludeGame();  // AP <= 0, game over
		}
	}
	else
	{
		UGameplayStatics::OpenLevel(this, TEXT("/Game/Maps/Lvl_HexMap"));
	}
}
