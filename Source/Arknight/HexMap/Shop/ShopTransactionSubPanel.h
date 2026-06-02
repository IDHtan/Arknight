// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../../DS.h"
#include "ShopTransactionSubPanel.generated.h"

class UButton;
class UImage;
class UEditableTextBox;

/**
 * Trade confirmation panel (secondary interface within Shop).
 * Shows the exchange formula, allows the player to input quantity,
 * and completes the transaction via backpack deduction and resource delivery.
 *
 * Fixed exchange formulas:
 *   1. Basic resource trade: current-region main resource 1:1 for any other basic resource.
 *   2. Rare resource trade: 10x main resource (non-Grain) = 1 AP;
 *      15x other resource (non-Grain) = 1 AP;
 *      15x Grain = 1 HighQualityFood.
 */
UCLASS()
class ARKNIGHT_API UShopTransactionSubPanel : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	UButton* ConfirmButton;

	UPROPERTY(meta = (BindWidget))
	UButton* ExitButton;

	UPROPERTY(meta = (BindWidget))
	UButton* CostResourceHitBox;

	UPROPERTY(meta = (BindWidget))
	UImage* CostResourceImage;

	UPROPERTY(meta = (BindWidget))
	UImage* TargetResourceImage;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* InputQuantityBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bHasCurrentCostType = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EResourceType CurrentCostType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bHasCurrentTargetType = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EResourceType CurrentTargetType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bTargetIsAP = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FName CurrentCostIconRowName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FName CurrentTargetIconRowName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float CurrentExchangeRate = 1.0f;

public:
	UFUNCTION(BlueprintCallable)
	void ShowTransaction(EResourceType TargetType);

	UFUNCTION(BlueprintCallable)
	void ShowAPTransaction();

	UFUNCTION(BlueprintCallable)
	void OnExitButtonClicked();

	UFUNCTION(BlueprintCallable)
	void OnHitBoxClicked();

	UFUNCTION(BlueprintCallable)
	void RequestBackpackSelection();

	UFUNCTION()
	void OnConfirmClicked();
};
