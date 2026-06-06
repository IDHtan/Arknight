// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../HexMap/Shop/Icon.h"
#include "InfrastructureIcon.generated.h"

class UTextBlock;

/**
 * Extended icon widget that adds a quantity text below the icon.
 * Used by CraftingPanel, TradeStationPanel, and ExchangePanel
 * to display resource counts directly on each icon.
 */
UCLASS()
class ARKNIGHT_API UInfrastructureIcon : public UIcon
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* QuantityText;

	UFUNCTION(BlueprintCallable, Category = "InfrastructureIcon")
	void SetQuantity(int32 Quantity);

	virtual void SetNone() override;
	virtual void Update() override;
};
