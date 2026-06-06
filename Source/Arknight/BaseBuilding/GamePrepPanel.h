// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../DS.h"
#include "GamePrepPanel.generated.h"

class UButton;
class UIcon;
class UTextBlock;
class URougeliteRunSubsystem;
class UHexMapSubsystem;

/**
 * Battle preparation panel — exchange Food / HighQualityFood for AP,
 * then initialise a new HexMap run and enter the starting region.
 *
 * Layout (pre-placed in UMG):
 *   [Food Icon]          [HQFood Icon]        [AP Icon]
 *   Owned: X              Owned: Y             Gain: Z
 *   Consume: A  [-][+]    Consume: B  [-][+]
 *
 *   [Confirm]  [Exit]
 */
UCLASS()
class ARKNIGHT_API UGamePrepPanel : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	// --- Three pre-set icons (bHasLinkedResourceType=true in UMG) ---
	UPROPERTY(meta = (BindWidget))
	UIcon* FoodIcon;

	UPROPERTY(meta = (BindWidget))
	UIcon* HighQualityFoodIcon;

	UPROPERTY(meta = (BindWidget))
	UIcon* APIcon;

	// --- Owned quantity texts ---
	UPROPERTY(meta = (BindWidget))
	UTextBlock* FoodOwnedText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* HQFoodOwnedText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* APGainText;

	// --- Consume amount texts ---
	UPROPERTY(meta = (BindWidget))
	UTextBlock* FoodConsumeText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* HQFoodConsumeText;

	// --- +/- buttons ---
	UPROPERTY(meta = (BindWidget))
	UButton* FoodIncreaseButton;

	UPROPERTY(meta = (BindWidget))
	UButton* FoodDecreaseButton;

	UPROPERTY(meta = (BindWidget))
	UButton* HQFoodIncreaseButton;

	UPROPERTY(meta = (BindWidget))
	UButton* HQFoodDecreaseButton;

	// --- Action buttons ---
	UPROPERTY(meta = (BindWidget))
	UButton* ConfirmButton;

	UPROPERTY(meta = (BindWidget))
	UButton* ExitButton;

	// --- Exchange rates (AP per unit consumed) ---
	UPROPERTY(EditDefaultsOnly, Category = "ExchangeRate")
	int32 APPerFood = 2;

	UPROPERTY(EditDefaultsOnly, Category = "ExchangeRate")
	int32 APPerHighQualityFood = 8;

	// --- Starting region for the run ---
	UPROPERTY(EditDefaultsOnly, Category = "Run")
	EHexRegionType StartingRegion = EHexRegionType::Wood;

	// --- Runtime state ---
	UPROPERTY()
	int32 FoodToConsume = 0;

	UPROPERTY()
	int32 HQFoodToConsume = 0;

	UPROPERTY()
	int32 CalculatedAP = 0;

	UPROPERTY()
	int32 OwnedFood = 0;

	UPROPERTY()
	int32 OwnedHQFood = 0;

	UPROPERTY()
	URougeliteRunSubsystem* RunSubsystem = nullptr;

	UPROPERTY()
	UHexMapSubsystem* HexMapSubsystem = nullptr;

	// --- Methods ---
	void Open();
	UFUNCTION()
	void Close();

	void RefreshOwnedDisplay();
	void RefreshConsumeDisplay();
	void CalculateAP();

	UFUNCTION()
	void OnFoodIncreaseClicked();

	UFUNCTION()
	void OnFoodDecreaseClicked();

	UFUNCTION()
	void OnHQFoodIncreaseClicked();

	UFUNCTION()
	void OnHQFoodDecreaseClicked();

	UFUNCTION()
	void OnConfirmClicked();
};
