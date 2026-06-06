// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseBuildingPanel.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "CraftingPanel.h"
#include "TradeStationPanel.h"
#include "ExchangePanel.h"
#include "../URougeliteRunSubsystem.h"

void UBaseBuildingPanel::NativeConstruct()
{
	Super::NativeConstruct();

	RunSubsystem = GetGameInstance() ? GetGameInstance()->GetSubsystem<URougeliteRunSubsystem>() : nullptr;

	// Bind Money delegate
	if (RunSubsystem)
	{
		RunSubsystem->OnMoneyChanged.AddDynamic(this, &UBaseBuildingPanel::OnMoneyChangedHandler);
	}

	// Bind buttons
	if (CraftingButton)
	{
		CraftingButton->OnClicked.AddDynamic(this, &UBaseBuildingPanel::OnCraftingButtonClicked);
	}
	if (TradeStationButton)
	{
		TradeStationButton->OnClicked.AddDynamic(this, &UBaseBuildingPanel::OnTradeStationButtonClicked);
	}
	if (ExchangeButton)
	{
		ExchangeButton->OnClicked.AddDynamic(this, &UBaseBuildingPanel::OnExchangeButtonClicked);
	}
	if (UnlockTradeStationButton)
	{
		UnlockTradeStationButton->OnClicked.AddDynamic(this, &UBaseBuildingPanel::OnUnlockTradeStationClicked);
	}
	if (UnlockExchangeButton)
	{
		UnlockExchangeButton->OnClicked.AddDynamic(this, &UBaseBuildingPanel::OnUnlockExchangeClicked);
	}
	if (ExitButton)
	{
		ExitButton->OnClicked.AddDynamic(this, &UBaseBuildingPanel::OnExitButtonClicked);
	}

	SetVisibility(ESlateVisibility::Collapsed);
}

void UBaseBuildingPanel::NativeDestruct()
{
	if (RunSubsystem)
	{
		RunSubsystem->OnMoneyChanged.RemoveDynamic(this, &UBaseBuildingPanel::OnMoneyChangedHandler);
	}

	Super::NativeDestruct();
}

void UBaseBuildingPanel::Open()
{
	RefreshUnlockState();
	RefreshMoneyDisplay();
	CollapseAllSubPanels();

	SetVisibility(ESlateVisibility::Visible);
}

void UBaseBuildingPanel::Close()
{
	SetVisibility(ESlateVisibility::Collapsed);
	CollapseAllSubPanels();
}

void UBaseBuildingPanel::RefreshUnlockState()
{
	if (!RunSubsystem || !RunSubsystem->CurrentSaveGame)
	{
		return;
	}

	const bool bTradeUnlocked = RunSubsystem->CurrentSaveGame->bHasUnlockedTradeStation;
	const bool bExchangeUnlocked = RunSubsystem->CurrentSaveGame->bHasUnlockedExchange;

	// Crafting is always available
	if (CraftingButton)
	{
		CraftingButton->SetVisibility(ESlateVisibility::Visible);
	}

	// TradeStation
	if (TradeStationButton)
	{
		TradeStationButton->SetVisibility(bTradeUnlocked ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
	if (UnlockTradeStationButton)
	{
		UnlockTradeStationButton->SetVisibility(bTradeUnlocked ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
	}

	// Exchange
	if (ExchangeButton)
	{
		ExchangeButton->SetVisibility(bExchangeUnlocked ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
	if (UnlockExchangeButton)
	{
		UnlockExchangeButton->SetVisibility(bExchangeUnlocked ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
	}
}

void UBaseBuildingPanel::RefreshMoneyDisplay()
{
	const int32 MoneyAmount = RunSubsystem ? RunSubsystem->GetGlobalResourceAmount(EResourceType::Money) : 0;
	if (MoneyText)
	{
		MoneyText->SetText(FText::AsNumber(MoneyAmount));
	}
}

void UBaseBuildingPanel::OnMoneyChangedHandler(int32 NewAmount)
{
	if (MoneyText)
	{
		MoneyText->SetText(FText::AsNumber(NewAmount));
	}
}

void UBaseBuildingPanel::CollapseAllSubPanels()
{
	if (CraftingPanel)
	{
		CraftingPanel->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (TradeStationPanel)
	{
		TradeStationPanel->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (ExchangePanel)
	{
		ExchangePanel->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UBaseBuildingPanel::OnCraftingButtonClicked()
{
	CollapseAllSubPanels();
	CraftingPanel->Open();
}

void UBaseBuildingPanel::OnTradeStationButtonClicked()
{
	CollapseAllSubPanels();
	TradeStationPanel->Open();
}

void UBaseBuildingPanel::OnExchangeButtonClicked()
{
	CollapseAllSubPanels();
	ExchangePanel->Open();
}

void UBaseBuildingPanel::OnExitButtonClicked()
{
	Close();
}

void UBaseBuildingPanel::OnUnlockTradeStationClicked()
{
	if (!RunSubsystem)
	{
		return;
	}

	const int32 OwnedBuildMaterials = RunSubsystem->GetGlobalResourceAmount(EResourceType::BuildMaterials);
	if (OwnedBuildMaterials >= TradeStationUnlockCost)
	{
		RunSubsystem->TryConsumeResource(EResourceType::BuildMaterials, TradeStationUnlockCost);
		RunSubsystem->UnlockTradeStation();
		RefreshUnlockState();
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("BaseBuildingPanel: Not enough BuildMaterials to unlock TradeStation (%d/%d)"),
			OwnedBuildMaterials, TradeStationUnlockCost);
	}
}

void UBaseBuildingPanel::OnUnlockExchangeClicked()
{
	if (!RunSubsystem)
	{
		return;
	}

	const int32 OwnedBuildMaterials = RunSubsystem->GetGlobalResourceAmount(EResourceType::BuildMaterials);
	if (OwnedBuildMaterials >= ExchangeUnlockCost)
	{
		RunSubsystem->TryConsumeResource(EResourceType::BuildMaterials, ExchangeUnlockCost);
		RunSubsystem->UnlockExchange();
		RefreshUnlockState();
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("BaseBuildingPanel: Not enough BuildMaterials to unlock Exchange (%d/%d)"),
			OwnedBuildMaterials, ExchangeUnlockCost);
	}
}
