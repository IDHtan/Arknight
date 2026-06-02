// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/WrapBox.h"
#include "../../DS.h"
#include "InventoryWidget.generated.h"

class UButton;
class UIcon;
class UImage;
class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnResourceConfirmedSignature, EResourceType, SelectedResource);

/**
 * HexMap backpack panel.
 * Dynamically populates resource icons from CurrentGameResources on open.
 *
 * Two modes:
 *   Normal mode  — opened via main HUD button, browse-only. ConfirmButton hidden.
 *   Selection mode — invoked by Shop transaction panel; ConfirmButton visible.
 *                    Player selects a resource → OnResourceConfirmed.Broadcast.
 */
UCLASS()
class ARKNIGHT_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintAssignable)
	FOnResourceConfirmedSignature OnResourceConfirmed;

	UPROPERTY(meta = (BindWidget))
	UButton* ExitButton;

	UPROPERTY(meta = (BindWidget))
	UButton* ConfirmButton;

	// Container for dynamically spawned resource icons — auto-wraps children
	UPROPERTY(meta = (BindWidget))
	UWrapBox* IconsContainer;

	// Template class for spawning resource icons
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<UIcon> IconWidgetClass;

	// Fixed size each spawned icon scales to
	UPROPERTY(EditDefaultsOnly, Category = "Inventory|Layout")
	FVector2D IconSize = FVector2D(100.0f, 100.0f);

	// Inner spacing between icons in the WrapBox (X = horizontal, Y = vertical)
	UPROPERTY(EditDefaultsOnly, Category = "Inventory|Layout")
	FVector2D SlotPadding = FVector2D(8.0f, 8.0f);

	// Runtime list of spawned icon widgets (cleared on close)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TArray<UIcon*> ResourceButtons;

	// Details panel: shows selected resource icon
	UPROPERTY(meta = (BindWidget))
	UImage* ResourceIcon;

	// Details panel: shows selected resource name
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ResourceNameText;

	// Details panel: shows selected resource quantity
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ResourceNumberText;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	EResourceType CurrentSelectedResource;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	bool bIsSelectionMode = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	bool bHasSelectedResource = false;

public:
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void OpenInventory(bool bInSelectionMode);

	UFUNCTION()
	void OnResourceButtonClicked(EResourceType Type);

	UFUNCTION()
	void OnConfirmClicked();

	UFUNCTION()
	void CloseInventory();
};
