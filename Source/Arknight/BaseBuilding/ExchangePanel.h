// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../DS.h"
#include "ExchangePanel.generated.h"

class UButton;
class UIcon;
class UGoodsIcon;
class UTextBlock;
class UWrapBox;
class URougeliteRunSubsystem;

/**
 * Exchange panel — buy resources with Money.
 *
 * Left: dynamically generated UGoodsIcon list — one per DT_ExchangeItem row.
 *       Each icon self-manages price / stock quantity / discount display.
 * Right: TargetIcon, cost/gain texts, buy qty controls, confirm button.
 *
 * Data source: DT_ExchangeItem (all rows loaded at open time).
 */
UCLASS()
class ARKNIGHT_API UExchangePanel : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// === Left: dynamic icon container ===
	UPROPERTY(meta = (BindWidget))
	UWrapBox* IconsContainer;

	UPROPERTY(EditDefaultsOnly, Category = "Exchange|Layout")
	TSubclassOf<UGoodsIcon> GoodsIconWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Exchange|Layout")
	FVector2D IconSize = FVector2D(100.0f, 100.0f);

	UPROPERTY(EditDefaultsOnly, Category = "Exchange|Layout")
	FVector2D SlotPadding = FVector2D(8.0f, 8.0f);

	UPROPERTY()
	TArray<UGoodsIcon*> ExchangeIcons;

	// === Right ===
	UPROPERTY(meta = (BindWidget))
	UIcon* TargetIcon;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CostMoneyText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* GainResourceText;

	UPROPERTY(meta = (BindWidget))
	UButton* IncreaseButton;

	UPROPERTY(meta = (BindWidget))
	UButton* DecreaseButton;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MoneyHoldText;

	UPROPERTY(meta = (BindWidget))
	UButton* ConfirmButton;

	UPROPERTY(meta = (BindWidget))
	UButton* ExitButton;

	// === Runtime data ===
	UPROPERTY()
	TArray<FInfrastructureExchangeItem> ExchangeItems;

	int32 SelectedIndex = -1;
	int32 CurrentBuyQty = 0;

	UPROPERTY()
	URougeliteRunSubsystem* RunSubsystem = nullptr;

	// === Methods ===
	void Open();
	UFUNCTION()
	void Close();

	void LoadExchangeItems();
	void PopulateExchangeIcons();

	void RefreshAllStockDisplay();
	void RefreshMoneyHoldDisplay();

	UFUNCTION()
	void OnMoneyChangedHandler(int32 NewAmount);

protected:
	void SetIcon();
	void SetQuantityTexts();

public:
	void OnResourceSelected(int32 Index);

	UFUNCTION()
	void OnIncreaseClicked();

	UFUNCTION()
	void OnDecreaseClicked();

	UFUNCTION()
	void OnConfirmClicked();
};
