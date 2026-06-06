// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../DS.h"
#include "TradeStationPanel.generated.h"

class UButton;
class UIcon;
class UTextBlock;
class UWrapBox;
class UInfrastructureIcon;
class URougeliteRunSubsystem;

/**
 * Trade station panel — sell resources for Money.
 *
 * Left: dynamically populated resource list (InfrastructureIcon with quantity).
 * Right: CostIcon (resource being sold) + MoneyIcon (fixed Money type).
 * Confirm deducts resource, adds Money.
 *
 * Rate lookup: DT_TradeRate → match ResourceType → MoneyPerUnit.
 */
UCLASS()
class ARKNIGHT_API UTradeStationPanel : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	// === Left: dynamic resource list ===
	UPROPERTY(meta = (BindWidget))
	UWrapBox* IconsContainer;

	UPROPERTY(EditDefaultsOnly, Category = "Trade|Layout")
	TSubclassOf<UInfrastructureIcon> IconWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Trade|Layout")
	FVector2D IconSize = FVector2D(100.0f, 100.0f);

	UPROPERTY(EditDefaultsOnly, Category = "Trade|Layout")
	FVector2D SlotPadding = FVector2D(8.0f, 8.0f);

	UPROPERTY()
	TArray<UInfrastructureIcon*> ResourceIcons;

	// === Right: cost (resource sold) / money gained ===
	UPROPERTY(meta = (BindWidget))
	UIcon* CostIcon;

	UPROPERTY(meta = (BindWidget))
	UIcon* MoneyIcon;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CostQuantityText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MoneyQuantityText;

	UPROPERTY(meta = (BindWidget))
	UButton* IncreaseButton;

	UPROPERTY(meta = (BindWidget))
	UButton* DecreaseButton;

	UPROPERTY(meta = (BindWidget))
	UButton* ConfirmButton;

	UPROPERTY(meta = (BindWidget))
	UButton* ExitButton;

	// === Runtime state ===
	UPROPERTY()
	EResourceType SelectedResource = EResourceType::Wood;

	FInfrastructureTradeRate CurrentRate;
	bool bHasValidRate = false;
	int32 CurrentSellQty = 0;

	UPROPERTY()
	URougeliteRunSubsystem* RunSubsystem = nullptr;

	// === Methods ===
	void Open();
	UFUNCTION()
	void Close();
	
protected:
	void PopulateResourceList();
	void SetIcon();
	void SetQuantityTexts();

public:
	UFUNCTION()
	void OnResourceSelected(EResourceType Type);

	UFUNCTION()
	void OnIncreaseClicked();

	UFUNCTION()
	void OnDecreaseClicked();

	UFUNCTION()
	void OnConfirmClicked();
};