// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InfrastructureIcon.h"
#include "../DS.h"
#include "GoodsIcon.generated.h"

class UTextBlock;

/**
 * Exchange goods icon — extends InfrastructureIcon with price and discount display.
 *
 * Controls its own visibility per element:
 *   - QuantityText  → collapsed when bIsLimited == false
 *   - DiscountText  → collapsed when DiscountRate == 0.f
 *   - PriceText     → always visible when ExchangeItemRow is valid
 *
 * SetExchangeItem(Row, Stock) is the unified entry point; it stores
 * the row pointer + current stock, then calls Update() to refresh all display.
 */
UCLASS()
class ARKNIGHT_API UGoodsIcon : public UInfrastructureIcon
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* PriceText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* DiscountText;

	// --- Runtime data ---
	const FInfrastructureExchangeItem* ExchangeItemRow = nullptr;
	int32 CurrentStock = 0;

	// --- Unified setter ---
	void SetExchangeItem(const FInfrastructureExchangeItem* InRow, int32 InStock);

	// --- Overrides ---
	virtual void SetNone() override;
	virtual void Update() override;
};
