// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../../DS.h"
#include "HexMapHUDWidget.generated.h"

class UButton;
class UCanvasPanel;
class UHexMapSubsystem;
class UHexNodeWidget;
class UTextBlock;
class UInventoryWidget;
class UEventWidget;
class UShopWidget;
class UTeleportWidget;
class UHexExitWidget;

UCLASS()
class ARKNIGHT_API UHexMapHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* MapCanvas = nullptr;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CurrentAP = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HexMap|HUD")
	TArray<UHexNodeWidget*> NodeWidgets;

	UPROPERTY(meta = (BindWidget))
	UButton* OpenBackpackButton = nullptr;

	UPROPERTY(meta = (BindWidget))
	UButton* ExitButton = nullptr;

	UPROPERTY(meta = (BindWidget))
	UInventoryWidget* InventoryPanel = nullptr;

	UPROPERTY(meta = (BindWidget))
	UEventWidget* EventPanel = nullptr;

	UPROPERTY(meta = (BindWidget))
	UShopWidget* ShopPanel = nullptr;

	UPROPERTY(meta = (BindWidget))
	UTeleportWidget* TeleportPanel = nullptr;

	UPROPERTY(meta = (BindWidget))
	UHexExitWidget* ExitPanel = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HexMap|HUD")
	UHexMapSubsystem* HexMapSubsystemP = nullptr;

	UFUNCTION()
	void OnHexNodeStatesRefreshed();

	UFUNCTION(BlueprintCallable, Category = "HexMap|HUD")
	void UpdateCurrentAP(int32 NewAP);

	UFUNCTION(BlueprintCallable, Category = "HexMap|HUD")
	void RefreshAllNodesAppearance();

	UFUNCTION(BlueprintCallable, Category = "HexMap|HUD")
	void OpenEventPanel(const FHexEventConfig& Config);

	UFUNCTION(BlueprintCallable, Category = "HexMap|HUD")
	void OpenShopPanel();

	UFUNCTION(BlueprintCallable, Category = "HexMap|HUD")
	void OpenTeleportPanel();

	UFUNCTION()
	void OnBackpackButtonClicked();

	UFUNCTION()
	void OnExitButtonClicked();
};
