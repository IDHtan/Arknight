// Fill out your copyright notice in the Description page of Project Settings.


#include "ShopTransactionSubPanel.h"

#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Icon.h"
#include "../Controller/HexMapPlayerController.h"
#include "../Controller/HexMapSubsystem.h"
#include "../Hex/HexMapHUDWidget.h"
#include "../Backpack/InventoryWidget.h"

void UShopTransactionSubPanel::NativeConstruct()
{
	Super::NativeConstruct();

	if (ConfirmButton)
	{
		ConfirmButton->OnClicked.AddDynamic(this, &UShopTransactionSubPanel::OnConfirmClicked);
		ConfirmButton->SetIsEnabled(false);
	}
	if (ExitButton)
	{
		ExitButton->OnClicked.AddDynamic(this, &UShopTransactionSubPanel::OnExitButtonClicked);
	}

	if (InputQuantityBox)
	{
		InputQuantityBox->OnTextChanged.AddDynamic(this, &UShopTransactionSubPanel::OnInputQuantityChanged);
	}

	HexMapSubsystemP = GetGameInstance() ? GetGameInstance()->GetSubsystem<UHexMapSubsystem>() : nullptr;
	if (!HexMapSubsystemP)
	{
		UE_LOG(LogTemp, Warning, TEXT("ShopTransactionSubPanel::NativeConstruct: failed to get HexMapSubsystem"));
	}

	SetVisibility(ESlateVisibility::Collapsed);
}

void UShopTransactionSubPanel::ShowTransaction(EResourceType TargetType)
{
	CurrentTargetType = TargetType;
	bTargetIsAP = false;

	if (TargetType == EResourceType::HighQualityFood)
	{
		CurrentCostType = EResourceType::Grain;
		CurrentExchangeRate = 15;
	}
	else
	{
		CurrentCostType = EResourceTypeMeta::GetRegionPrimaryResource(HexMapSubsystemP->CurrentRegion);
		CurrentExchangeRate = 1;
	}
	bHasCurrentCostType = true;

	if (CostIcon)
	{
		CostIcon->SetResourceType(CurrentCostType);
	}
	if (TargetIcon)
	{
		TargetIcon->SetResourceType(CurrentTargetType);
	}

	if (ConfirmButton)
	{
		ConfirmButton->SetIsEnabled(true);
	}
	if (InputQuantityBox)
	{
		InputQuantityBox->SetText(FText::AsNumber(0));
	}

	OnInputQuantityChanged(FText::AsNumber(0));
}

void UShopTransactionSubPanel::ShowAPTransaction()
{
	bHasCurrentCostType = false;
	bTargetIsAP = true;

	if (CostIcon)
	{
		CostIcon->OnIconButtonClicked.AddDynamic(this, &UShopTransactionSubPanel::RequestBackpackSelection);
	}

	if (TargetIcon)
	{
		TargetIcon->SetResourceType(EResourceType::AP);
	}

	if (ConfirmButton)
	{
		ConfirmButton->SetIsEnabled(false);
	}
	if (InputQuantityBox)
	{
		InputQuantityBox->SetText(FText::AsNumber(0));
	}

	OnInputQuantityChanged(FText::AsNumber(0));
}

void UShopTransactionSubPanel::OnExitButtonClicked()
{
	if (CostIcon)
	{
		CostIcon->OnIconButtonClicked.RemoveDynamic(this, &UShopTransactionSubPanel::RequestBackpackSelection);
	}
	SetVisibility(ESlateVisibility::Collapsed);
}

void UShopTransactionSubPanel::RequestBackpackSelection()
{
	AHexMapPlayerController* PC = Cast<AHexMapPlayerController>(GetOwningPlayer());
	if (!PC || !PC->MapWidgetInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("ShopTransactionSubPanel::RequestBackpackSelection: failed to find HUD"));
		return;
	}

	UInventoryWidget* Inventory = PC->MapWidgetInstance->InventoryPanel;
	if (!Inventory)
	{
		UE_LOG(LogTemp, Warning, TEXT("ShopTransactionSubPanel::RequestBackpackSelection: InventoryPanel is null"));
		return;
	}

	Inventory->OpenInventory(true);
	Inventory->OnResourceConfirmed.RemoveDynamic(this, &UShopTransactionSubPanel::OnBackpackResourceSelected);
	Inventory->OnResourceConfirmed.AddDynamic(this, &UShopTransactionSubPanel::OnBackpackResourceSelected);
}

void UShopTransactionSubPanel::OnBackpackResourceSelected(EResourceType Type)
{
	AHexMapPlayerController* PC = Cast<AHexMapPlayerController>(GetOwningPlayer());
	if (PC && PC->MapWidgetInstance && PC->MapWidgetInstance->InventoryPanel)
	{
		PC->MapWidgetInstance->InventoryPanel->OnResourceConfirmed.RemoveDynamic(
			this, &UShopTransactionSubPanel::OnBackpackResourceSelected);
	}

	CurrentCostType = Type;
	bHasCurrentCostType = true;

	// Determine exchange rate for AP based on cost resource type
	//   10 non-Grain main region resource → 1 AP
	//   15 other non-Grain resource → 1 AP
	//   15 Grain → 1 AP as well (Grain→HighQualityFood handled in ShowTransaction)
	const EResourceType RegionResource = EResourceTypeMeta::GetRegionPrimaryResource(HexMapSubsystemP->CurrentRegion);
	if (Type == RegionResource && Type != EResourceType::Grain)
	{
		CurrentExchangeRate = 10;
	}
	else
	{
		CurrentExchangeRate = 15;
	}

	if (CostIcon)
	{
		CostIcon->SetResourceType(CurrentCostType);
	}

	if (ConfirmButton)
	{
		ConfirmButton->SetIsEnabled(true);
	}
}

void UShopTransactionSubPanel::OnInputQuantityChanged(const FText& Text)
{
	if (!OutputQuantityText)
	{
		return;
	}

	InputQuantity = FCString::Atoi(*Text.ToString());
	if (InputQuantity <= 0 || CurrentExchangeRate <= 0)
	{
		OutputQuantity = 0;
		OutputQuantityText->SetText(FText::AsNumber(0));
		return;
	}

	OutputQuantity = InputQuantity / CurrentExchangeRate;
	OutputQuantityText->SetText(FText::AsNumber(OutputQuantity));
}

void UShopTransactionSubPanel::OnConfirmClicked()
{
	if (!HexMapSubsystemP)
	{
		return;
	}

	if (!bHasCurrentCostType)
	{
		UE_LOG(LogTemp, Warning, TEXT("ShopTransactionSubPanel::OnConfirmClicked: no cost type selected"));
		return;
	}

	if (InputQuantity <= 0 || OutputQuantity <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("ShopTransactionSubPanel::OnConfirmClicked: invalid quantity"));
		return;
	}

	const int32 Balance = HexMapSubsystemP->GetResource(CurrentCostType);
	if (Balance < InputQuantity)
	{
		UE_LOG(LogTemp, Warning, TEXT("ShopTransactionSubPanel::OnConfirmClicked: Balance %d insufficient for cost %d"),
			Balance, InputQuantity);
		return;
	}

	HexMapSubsystemP->AddHexMapResource(CurrentCostType, -InputQuantity);
	if (bTargetIsAP)
	{
		HexMapSubsystemP->ChangeCurrentAP(OutputQuantity);
	}
	else
	{
		HexMapSubsystemP->AddHexMapResource(CurrentTargetType, OutputQuantity);
	}

	UE_LOG(LogTemp, Log, TEXT("ShopTransactionSubPanel: traded %d %s → %d %s"),
		InputQuantity, *UEnum::GetValueAsString(CurrentCostType),
		OutputQuantity, bTargetIsAP ? TEXT("AP") : *UEnum::GetValueAsString(CurrentTargetType));

	if (CostIcon)
	{
		CostIcon->OnIconButtonClicked.RemoveDynamic(this, &UShopTransactionSubPanel::RequestBackpackSelection);
	}
	SetVisibility(ESlateVisibility::Collapsed);
}

