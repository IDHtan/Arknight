// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../../DS.h"
#include "ShopWidget.generated.h"

class UButton;
class UIcon;
class UShopTransactionSubPanel;
class UHexMapSubsystem;

/**
 * Shop main interface (primary panel).
 *
 * 6 resource icons are pre-created in UMG with bHasLinkedResourceType=true
 * and their specific EResourceType set. Collected via WidgetTree scan in
 * NativeConstruct — no dynamic creation. AP button is a standalone BindWidget.
 *
 * On entry, HUD calls OpenShop(). The panel handles all its own business.
 */
UCLASS()
class ARKNIGHT_API UShopWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UButton* ExitButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shop")
	TArray<UIcon*> ResourceButtons;

	UPROPERTY(meta = (BindWidget))
	UIcon* APButton;

	UPROPERTY(meta = (BindWidget))
	UShopTransactionSubPanel* TransactionPanel;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shop")
	UHexMapSubsystem* HexMapSubsystemP;

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
