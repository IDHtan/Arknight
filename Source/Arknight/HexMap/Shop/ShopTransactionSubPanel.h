// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../../DS.h"
#include "ShopTransactionSubPanel.generated.h"

class UButton;
class UIcon;
class UEditableTextBox;
class UTextBlock;
class UHexMapSubsystem;

/**
 * Trade confirmation panel (secondary panel).
 *
 * CostIcon and TargetIcon are pre-created UIcon widgets in UMG
 * with bHasLinkedResourceType=false. Their types are dynamically
 * assigned via SetResourceType() when ShowTransaction / ShowAPTransaction is called.
 *
 * Fixed exchange formulas:
 *   1. Basic: region main resource 1:1 for any other basic resource.
 *   2. Rare: 10x non-Grain main resource = 1 AP.
 *      15x other non-Grain = 1 AP.
 *      15x Grain = 1 HighQualityFood.
 */
UCLASS()
class ARKNIGHT_API UShopTransactionSubPanel : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UButton* ConfirmButton;

	UPROPERTY(meta = (BindWidget))
	UButton* ExitButton;

	UPROPERTY(meta = (BindWidget))
	UIcon* CostIcon;

	UPROPERTY(meta = (BindWidget))
	UIcon* TargetIcon;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* InputQuantityBox;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* OutputQuantityText;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bHasCurrentCostType = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EResourceType CurrentCostType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EResourceType CurrentTargetType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 CurrentExchangeRate = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bTargetIsAP = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 InputQuantity=0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 OutputQuantity=0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UHexMapSubsystem* HexMapSubsystemP;

public:
	UFUNCTION(BlueprintCallable)
	void ShowTransaction(EResourceType TargetType);

	UFUNCTION(BlueprintCallable)
	void ShowAPTransaction();

	UFUNCTION(BlueprintCallable)
	void OnExitButtonClicked();

	UFUNCTION(BlueprintCallable)
	void RequestBackpackSelection();

	UFUNCTION()
	void OnBackpackResourceSelected(EResourceType Type);

	UFUNCTION()
	void OnInputQuantityChanged(const FText& Text);

	UFUNCTION()
	void OnConfirmClicked();

private:
	// Map region type to its primary resource for cost calculation
	static EResourceType GetRegionResource(EHexRegionType Region);
};
