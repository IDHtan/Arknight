// Fill out your copyright notice in the Description page of Project Settings.

#include "TradeStationPanel.h"

#include "Components/Button.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/WrapBox.h"
#include "Engine/DataTable.h"
#include "InfrastructureIcon.h"
#include "../HexMap/Shop/Icon.h"
#include "../URougeliteRunSubsystem.h"
#include "../RougeliteSettings.h"

void UTradeStationPanel::NativeConstruct()
{
	Super::NativeConstruct();

	RunSubsystem = GetGameInstance() ? GetGameInstance()->GetSubsystem<URougeliteRunSubsystem>() : nullptr;

	if (IncreaseButton)
	{
		IncreaseButton->OnClicked.AddDynamic(this, &UTradeStationPanel::OnIncreaseClicked);
	}
	if (DecreaseButton)
	{
		DecreaseButton->OnClicked.AddDynamic(this, &UTradeStationPanel::OnDecreaseClicked);
	}
	if (ConfirmButton)
	{
		ConfirmButton->OnClicked.AddDynamic(this, &UTradeStationPanel::OnConfirmClicked);
	}
	if (ExitButton)
	{
		ExitButton->OnClicked.AddDynamic(this, &UTradeStationPanel::Close);
	}

	SetVisibility(ESlateVisibility::Collapsed);
}

void UTradeStationPanel::Open()
{
	PopulateResourceList();

	// MoneyIcon always shows Money type
	if (MoneyIcon)
	{
		MoneyIcon->SetResourceType(EResourceType::Money);
	}

	bHasValidRate = false;
	CurrentSellQty = 0;
	SetIcon();
	SetQuantityTexts();
	SetVisibility(ESlateVisibility::Visible);
}

void UTradeStationPanel::Close()
{
	SetVisibility(ESlateVisibility::Collapsed);

	if (IconsContainer)
	{
		IconsContainer->ClearChildren();
	}
	ResourceIcons.Empty();
}

void UTradeStationPanel::PopulateResourceList()
{
	if (!IconsContainer || !IconWidgetClass || !RunSubsystem)
	{
		return;
	}

	IconsContainer->ClearChildren();
	IconsContainer->SetInnerSlotPadding(SlotPadding);
	ResourceIcons.Empty();

	for (const TPair<EResourceType, int32>& Pair : RunSubsystem->GlobalResource)
	{
		if (!EResourceTypeMeta::IsStorable(Pair.Key))
		{
			continue;
		}
		if (Pair.Value <= 0)
		{
			continue;
		}

		UInfrastructureIcon* IconWidget = CreateWidget<UInfrastructureIcon>(this, IconWidgetClass);
		if (!IconWidget)
		{
			continue;
		}

		IconWidget->SetResourceType(Pair.Key);
		IconWidget->SetQuantity(Pair.Value);
		IconWidget->OnIconClicked.AddDynamic(this, &UTradeStationPanel::OnResourceSelected);

		USizeBox* SizeBox = NewObject<USizeBox>(this);
		SizeBox->SetWidthOverride(IconSize.X);
		SizeBox->SetHeightOverride(IconSize.Y);
		SizeBox->AddChild(IconWidget);

		IconsContainer->AddChildToWrapBox(SizeBox);
		ResourceIcons.Add(IconWidget);
	}
}

void UTradeStationPanel::SetIcon()
{
	if (!bHasValidRate)
	{
		CostIcon->SetNone();
		return;
	}

	CostIcon->SetResourceType(CurrentRate.ResourceType);
}

void UTradeStationPanel::SetQuantityTexts()
{
	if (!bHasValidRate)
	{
		CostQuantityText->SetText(FText::AsNumber(0));
		MoneyQuantityText->SetText(FText::AsNumber(0));
		DecreaseButton->SetIsEnabled(false);
		IncreaseButton->SetIsEnabled(false);
		ConfirmButton->SetIsEnabled(false);
		return;
	}

	const int32 MoneyGained = CurrentSellQty * CurrentRate.MoneyPerUnit;
	CostQuantityText->SetText(FText::AsNumber(CurrentSellQty));
	MoneyQuantityText->SetText(FText::AsNumber(MoneyGained));

	if (CurrentSellQty <= 0)
	{
		DecreaseButton->SetIsEnabled(false);
		ConfirmButton->SetIsEnabled(false);
	}
	else
	{
		DecreaseButton->SetIsEnabled(true);
		ConfirmButton->SetIsEnabled(true);
	}

	const int32 Owned = RunSubsystem ? RunSubsystem->GetGlobalResourceAmount(CurrentRate.ResourceType) : 0;
	if (CurrentSellQty >= Owned)
	{
		IncreaseButton->SetIsEnabled(false);
	}
	else
	{
		IncreaseButton->SetIsEnabled(true);
	}
}

void UTradeStationPanel::OnResourceSelected(EResourceType Type)
{
	SelectedResource = Type;

	bHasValidRate = false;
	const URougeliteSettings* Settings = GetDefault<URougeliteSettings>();
	UDataTable* Table = Settings ? Settings->TradeRateTable.LoadSynchronous() : nullptr;
	if (Table)
	{
		TArray<FInfrastructureTradeRate*> AllRows;
		Table->GetAllRows<FInfrastructureTradeRate>(TEXT("TradeStationPanel"), AllRows);
		for (const FInfrastructureTradeRate* Row : AllRows)
		{
			if (Row && Row->ResourceType == Type)
			{
				CurrentRate = *Row;
				bHasValidRate = true;
				break;
			}
		}
	}

	CurrentSellQty = 0;
	SetIcon();
	SetQuantityTexts();
}

void UTradeStationPanel::OnIncreaseClicked()
{
	if (!bHasValidRate)
	{
		return;
	}

	CurrentSellQty++;
	SetQuantityTexts();
}

void UTradeStationPanel::OnDecreaseClicked()
{
	if (!bHasValidRate)
	{
		return;
	}

	CurrentSellQty--;
	SetQuantityTexts();
}

void UTradeStationPanel::OnConfirmClicked()
{
	if (!bHasValidRate || CurrentSellQty <= 0 || !RunSubsystem)
	{
		return;
	}

	const int32 MoneyGained = CurrentSellQty * CurrentRate.MoneyPerUnit;

	if (!RunSubsystem->TryConsumeResource(CurrentRate.ResourceType, CurrentSellQty))
	{
		UE_LOG(LogTemp, Warning, TEXT("TradeStationPanel::OnConfirmClicked: insufficient resources"));
		return;
	}

	RunSubsystem->AddResource(EResourceType::Money, MoneyGained);

	PopulateResourceList();

	bHasValidRate = false;
	CurrentSellQty = 0;

	SetIcon();
	SetQuantityTexts();
}