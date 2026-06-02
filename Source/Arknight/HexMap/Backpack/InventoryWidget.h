// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../../DS.h"
#include "InventoryWidget.generated.h"

class UButton;
class UIcon;
class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnResourceConfirmedSignature, EResourceType, SelectedResource);

/**
 * HexMap backpack panel.
 * Displays resources collected during the current run.
 *
 * Two modes:
 *   Normal mode  — opened via main HUD button, browse-only.
 *   Selection mode — invoked by Shop transaction panel; exposes ConfirmButton
 *                    and returns the player's chosen resource type.
 */
UCLASS()
class ARKNIGHT_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnResourceConfirmedSignature OnResourceConfirmed;

	UPROPERTY(meta = (BindWidget))
	UButton* ExitButton;

	UPROPERTY(meta = (BindWidget))
	UButton* ConfirmButton;

	UPROPERTY(meta = (BindWidget))
	TArray<UIcon*> ResourceButtons;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ResourceNameText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ResourceNumberText;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsSelectionMode = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bHasSelectedResource = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EResourceType CurrentSelectedResource;

public:
	UFUNCTION(BlueprintCallable)
	void OpenInventory(bool bInSelectionMode);

	UFUNCTION()
	void OnResourceButtonClicked(EResourceType Type);

	UFUNCTION()
	void OnConfirmClicked();

	UFUNCTION()
	void CloseInventory();
};
