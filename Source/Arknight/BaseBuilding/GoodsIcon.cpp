// Fill out your copyright notice in the Description page of Project Settings.

#include "GoodsIcon.h"
#include "Components/TextBlock.h"

void UGoodsIcon::SetExchangeItem(const FInfrastructureExchangeItem* InRow, int32 InStock)
{
	ExchangeItemRow = InRow;
	CurrentStock = InStock;
	Update();
}

void UGoodsIcon::SetNone()
{
	Super::SetNone();

	ExchangeItemRow = nullptr;
	CurrentStock = 0;

	if (PriceText)
	{
		PriceText->SetText(FText::GetEmpty());
	}
	if (DiscountText)
	{
		DiscountText->SetText(FText::GetEmpty());
		DiscountText->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UGoodsIcon::Update()
{
	if (!ExchangeItemRow)
	{
		SetNone();
		return;
	}

	Super::Update();

	// --- PriceText ---
	if (PriceText)
	{
		PriceText->SetText(FText::AsNumber(ExchangeItemRow->MoneyPerUnit));
	}

	// --- QuantityText (inherited) ---
	if (QuantityText)
	{
		if (ExchangeItemRow->bIsLimited)
		{
			SetQuantity(CurrentStock);
		}
		else
		{
			QuantityText->SetText(FText::GetEmpty());
			QuantityText->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	// --- DiscountText ---
	if (DiscountText)
	{
		if (ExchangeItemRow->DiscountRate > 0.f)
		{
			const int32 Percent = FMath::RoundToInt(ExchangeItemRow->DiscountRate * 100.f);
			DiscountText->SetText(FText::Format(
				FText::FromString(TEXT("-{0}%")), FText::AsNumber(Percent)));
			DiscountText->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			DiscountText->SetText(FText::GetEmpty());
			DiscountText->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}
