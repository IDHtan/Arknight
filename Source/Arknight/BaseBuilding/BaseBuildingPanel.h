// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BaseBuildingPanel.generated.h"

class UButton;
class UImage;
class UTextBlock;
class UCraftingPanel;
class UTradeStationPanel;
class UExchangePanel;
class URougeliteRunSubsystem;

/**
 * Base building main panel — container for the three station sub-panels.
 *
 * Facilities can be locked (unlock with BuildMaterials):
 *   - CraftingPanel: always available
 *   - TradeStationPanel: requires bHasUnlockedTradeStation
 *   - ExchangePanel: requires bHasUnlockedExchange
 *
 * When locked, a LockedImage + fully-transparent UnlockButton overlay is shown.
 */
UCLASS()
class ARKNIGHT_API UBaseBuildingPanel : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// --- Unlocked facility buttons ---
	UPROPERTY(meta = (BindWidget))
	UButton* CraftingButton;

	UPROPERTY(meta = (BindWidget))
	UButton* TradeStationButton;

	UPROPERTY(meta = (BindWidget))
	UButton* ExchangeButton;

	// --- Locked state: images + transparent unlock buttons ---
	UPROPERTY(meta = (BindWidget))
	UButton* UnlockTradeStationButton;

	UPROPERTY(meta = (BindWidget))
	UButton* UnlockExchangeButton;

	UPROPERTY(meta = (BindWidget))
	UButton* ExitButton;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MoneyText;

	// --- Sub-panels ---
	UPROPERTY(meta = (BindWidget))
	UCraftingPanel* CraftingPanel;

	UPROPERTY(meta = (BindWidget))
	UTradeStationPanel* TradeStationPanel;

	UPROPERTY(meta = (BindWidget))
	UExchangePanel* ExchangePanel;

	// --- Unlock costs (BuildMaterials) ---
	UPROPERTY(EditDefaultsOnly, Category = "Unlock")
	int32 TradeStationUnlockCost = 10;

	UPROPERTY(EditDefaultsOnly, Category = "Unlock")
	int32 ExchangeUnlockCost = 10;

	// --- Runtime ---
	UPROPERTY()
	URougeliteRunSubsystem* RunSubsystem = nullptr;

	// --- Methods ---
	void Open();
	void Close();
	void RefreshUnlockState();
	void RefreshMoneyDisplay();
	void CollapseAllSubPanels();

	UFUNCTION()
	void OnMoneyChangedHandler(int32 NewAmount);

	UFUNCTION()
	void OnCraftingButtonClicked();

	UFUNCTION()
	void OnTradeStationButtonClicked();

	UFUNCTION()
	void OnExchangeButtonClicked();

	UFUNCTION()
	void OnExitButtonClicked();

	UFUNCTION()
	void OnUnlockTradeStationClicked();

	UFUNCTION()
	void OnUnlockExchangeClicked();
};
