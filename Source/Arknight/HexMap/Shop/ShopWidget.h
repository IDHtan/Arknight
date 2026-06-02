// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../../DS.h"
#include "ShopWidget.generated.h"

class UButton;
class UIcon;
class UShopTransactionSubPanel;

/**
 * Shop main interface (primary interface).
 * Provides 6 fixed resource exchange entries.
 *
 * No random logic — pure data interaction layer.
 * Called by PlayerController via OpenShop().
 * Resource entries reuse UIcon (shared init logic with Backpack via IconImageTable).
 * AP entry uses a standalone UIcon looked up by RowName="AP".
 */
UCLASS()
class ARKNIGHT_API UShopWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	UButton* ExitButton;

	UPROPERTY(meta = (BindWidget))
	TArray<UIcon*> ResourceButtons;

	UPROPERTY(meta = (BindWidget))
	UIcon* APButton;

	UPROPERTY(meta = (BindWidget))
	UShopTransactionSubPanel* TransactionPanel;

public:
	UFUNCTION(BlueprintCallable)
	void OpenShop();

	UFUNCTION()
	void OnResourceButtonClicked(EResourceType TargetResourceType);

	UFUNCTION()
	void OnAPButtonClicked();

	UFUNCTION()
	void CloseShop();
};
