// Fill out your copyright notice in the Description page of Project Settings.

#include "ExchangePanel.h"

#include "Components/Button.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/WrapBox.h"
#include "Engine/DataTable.h"
#include "GoodsIcon.h"
#include "../HexMap/Shop/Icon.h"
#include "../URougeliteRunSubsystem.h"
#include "../RougeliteSettings.h"

void UExchangePanel::NativeConstruct()
{
	Super::NativeConstruct();

	RunSubsystem = GetGameInstance() ? GetGameInstance()->GetSubsystem<URougeliteRunSubsystem>() : nullptr;

	if (RunSubsystem)
	{
		RunSubsystem->OnMoneyChanged.AddDynamic(this, &UExchangePanel::OnMoneyChangedHandler);
	}

	if (IncreaseButton)
	{
		IncreaseButton->OnClicked.AddDynamic(this, &UExchangePanel::OnIncreaseClicked);
	}
	if (DecreaseButton)
	{
		DecreaseButton->OnClicked.AddDynamic(this, &UExchangePanel::OnDecreaseClicked);
	}
	if (ConfirmButton)
	{
		ConfirmButton->OnClicked.AddDynamic(this, &UExchangePanel::OnConfirmClicked);
	}
	if (ExitButton)
	{
		ExitButton->OnClicked.AddDynamic(this, &UExchangePanel::Close);
	}

	SetVisibility(ESlateVisibility::Collapsed);
}

void UExchangePanel::NativeDestruct()
{
	if (RunSubsystem)
	{
		RunSubsystem->OnMoneyChanged.RemoveDynamic(this, &UExchangePanel::OnMoneyChangedHandler);
	}

	Super::NativeDestruct();
}

void UExchangePanel::Open()
{
	LoadExchangeItems();
	PopulateExchangeIcons();
	RefreshMoneyHoldDisplay();

	SelectedIndex = -1;
	CurrentBuyQty = 0;
	SetIcon();
	SetQuantityTexts();

	SetVisibility(ESlateVisibility::Visible);
}

void UExchangePanel::Close()
{
	SetVisibility(ESlateVisibility::Collapsed);

	if (IconsContainer)
	{
		IconsContainer->ClearChildren();
	}
	ExchangeIcons.Empty();
	ExchangeItems.Empty();
	SelectedIndex = -1;
}

void UExchangePanel::LoadExchangeItems()
{
	ExchangeItems.Empty();

	const URougeliteSettings* Settings = GetDefault<URougeliteSettings>();
	UDataTable* Table = Settings ? Settings->ExchangeItemTable.LoadSynchronous() : nullptr;
	if (!Table)
	{
		UE_LOG(LogTemp, Warning, TEXT("ExchangePanel::LoadExchangeItems: ExchangeItemTable not found"));
		return;
	}

	TArray<FInfrastructureExchangeItem*> AllRows;
	Table->GetAllRows<FInfrastructureExchangeItem>(TEXT("ExchangePanel"), AllRows);
	for (const FInfrastructureExchangeItem* Row : AllRows)
	{
		if (Row)
		{
			ExchangeItems.Add(*Row);
		}
	}
}

void UExchangePanel::PopulateExchangeIcons()
{
	if (!IconsContainer || !GoodsIconWidgetClass)
	{
		return;
	}

	IconsContainer->ClearChildren();
	IconsContainer->SetInnerSlotPadding(SlotPadding);
	ExchangeIcons.Empty();

	for (int32 i = 0; i < ExchangeItems.Num(); ++i)
	{
		const FInfrastructureExchangeItem& Item = ExchangeItems[i];
		const int32 Stock = RunSubsystem ? RunSubsystem->GetExchangeStock(Item.ResourceType) : 0;

		UGoodsIcon* GoodsIcon = CreateWidget<UGoodsIcon>(this, GoodsIconWidgetClass);
		if (!GoodsIcon)
		{
			continue;
		}

		GoodsIcon->SetResourceType(Item.ResourceType);
		GoodsIcon->SetExchangeItem(&Item, Stock);

		const int32 Index = i;
		GoodsIcon->OnIconClicked.AddLambda([this, Index](EResourceType Type)
		{
			OnResourceSelected(Index);
		});

		USizeBox* SizeBox = NewObject<USizeBox>(this);
		SizeBox->SetWidthOverride(IconSize.X);
		SizeBox->SetHeightOverride(IconSize.Y);
		SizeBox->AddChild(GoodsIcon);

		IconsContainer->AddChildToWrapBox(SizeBox);
		ExchangeIcons.Add(GoodsIcon);
	}
}

void UExchangePanel::SetIcon()
{
	if (SelectedIndex < 0 || SelectedIndex >= ExchangeItems.Num())
	{
		TargetIcon->SetNone();
		return;
	}

	TargetIcon->SetResourceType(ExchangeItems[SelectedIndex].ResourceType);
}

void UExchangePanel::SetQuantityTexts()
{
	if (SelectedIndex < 0 || SelectedIndex >= ExchangeItems.Num())
	{
		CostMoneyText->SetText(FText::AsNumber(0));
		GainResourceText->SetText(FText::AsNumber(0));
		DecreaseButton->SetIsEnabled(false);
		IncreaseButton->SetIsEnabled(false);
		ConfirmButton->SetIsEnabled(false);
		return;
	}

	const FInfrastructureExchangeItem& Item = ExchangeItems[SelectedIndex];

	// Apply discount to effective price
	const int32 BasePrice = Item.MoneyPerUnit;
	const int32 EffectivePrice = Item.DiscountRate > 0.f
		? FMath::RoundToInt(BasePrice * (1.f - Item.DiscountRate))
		: BasePrice;

	// Determine max buyable qty (bounded by Money + stock)
	int32 MaxQty = RunSubsystem ? RunSubsystem->GetGlobalResourceAmount(EResourceType::Money) / EffectivePrice : 0;
	if (Item.bIsLimited)
	{
		const int32 Stock = RunSubsystem ? RunSubsystem->GetExchangeStock(Item.ResourceType) : 0;
		MaxQty = FMath::Min(MaxQty, Stock);
	}

	CurrentBuyQty = FMath::Clamp(CurrentBuyQty, 0, MaxQty);

	const int32 TotalCost = CurrentBuyQty * EffectivePrice;
	CostMoneyText->SetText(FText::AsNumber(TotalCost));
	GainResourceText->SetText(FText::AsNumber(CurrentBuyQty));

	if (CurrentBuyQty <= 0)
	{
		DecreaseButton->SetIsEnabled(false);
		ConfirmButton->SetIsEnabled(false);
	}
	else
	{
		DecreaseButton->SetIsEnabled(true);
		ConfirmButton->SetIsEnabled(MaxQty > 0);
	}

	if (CurrentBuyQty >= MaxQty)
	{
		IncreaseButton->SetIsEnabled(false);
	}
	else
	{
		IncreaseButton->SetIsEnabled(true);
	}
}

void UExchangePanel::RefreshAllStockDisplay()
{
	for (int32 i = 0; i < ExchangeItems.Num(); ++i)
	{
		if (i < ExchangeIcons.Num() && ExchangeIcons[i])
		{
			const int32 Stock = RunSubsystem ? RunSubsystem->GetExchangeStock(ExchangeItems[i].ResourceType) : 0;
			ExchangeIcons[i]->SetExchangeItem(&ExchangeItems[i], Stock);
		}
	}
}

void UExchangePanel::RefreshMoneyHoldDisplay()
{
	const int32 MoneyAmount = RunSubsystem ? RunSubsystem->GetGlobalResourceAmount(EResourceType::Money) : 0;
	if (MoneyHoldText)
	{
		MoneyHoldText->SetText(FText::AsNumber(MoneyAmount));
	}
}

void UExchangePanel::OnMoneyChangedHandler(int32 NewAmount)
{
	RefreshMoneyHoldDisplay();

	if (SelectedIndex >= 0 && SelectedIndex < ExchangeItems.Num())
	{
		SetQuantityTexts();
	}
}

void UExchangePanel::OnResourceSelected(int32 Index)
{
	if (Index < 0 || Index >= ExchangeItems.Num())
	{
		return;
	}

	SelectedIndex = Index;
	CurrentBuyQty = 0;
	SetIcon();
	SetQuantityTexts();
}

void UExchangePanel::OnIncreaseClicked()
{
	if (SelectedIndex < 0)
	{
		return;
	}

	CurrentBuyQty++;
	SetQuantityTexts();
}

void UExchangePanel::OnDecreaseClicked()
{
	if (SelectedIndex < 0)
	{
		return;
	}

	CurrentBuyQty--;
	SetQuantityTexts();
}

void UExchangePanel::OnConfirmClicked()
{
	if (SelectedIndex < 0 || CurrentBuyQty <= 0 || !RunSubsystem)
	{
		return;
	}

	const FInfrastructureExchangeItem& Item = ExchangeItems[SelectedIndex];

	const int32 EffectivePrice = Item.DiscountRate > 0.f
		? FMath::RoundToInt(Item.MoneyPerUnit * (1.f - Item.DiscountRate))
		: Item.MoneyPerUnit;
	const int32 TotalCost = CurrentBuyQty * EffectivePrice;

	if (!RunSubsystem->TryConsumeResource(EResourceType::Money, TotalCost))
	{
		UE_LOG(LogTemp, Warning, TEXT("ExchangePanel::OnConfirmClicked: insufficient Money"));
		return;
	}

	RunSubsystem->AddResource(Item.ResourceType, CurrentBuyQty);

	if (Item.bIsLimited)
	{
		RunSubsystem->ReduceExchangeStock(Item.ResourceType, CurrentBuyQty);
	}

	RefreshAllStockDisplay();
	RefreshMoneyHoldDisplay();

	SelectedIndex = -1;
	CurrentBuyQty = 0;
	SetIcon();
	SetQuantityTexts();
}
