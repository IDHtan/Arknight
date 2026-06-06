// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../DS.h"
#include "CraftingPanel.generated.h"

class UButton;
class UIcon;
class UTextBlock;
class UWrapBox;
class UInfrastructureIcon;
class URougeliteRunSubsystem;

/**
 * Crafting station panel.
 *
 * Left: dynamically populated resource list (InfrastructureIcon with quantity).
 * Right: CostIcon + ProductIcon, quantity controls, confirm button.
 *
 * Recipe lookup: DT_CraftRecipe → match InputType → OutputType + InputPerOutput.
 */
UCLASS()
class ARKNIGHT_API UCraftingPanel : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	// === Left: dynamic resource list ===
	UPROPERTY(meta = (BindWidget))
	UWrapBox* IconsContainer;

	UPROPERTY(EditDefaultsOnly, Category = "Crafting|Layout")
	TSubclassOf<UInfrastructureIcon> IconWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Crafting|Layout")
	FVector2D IconSize = FVector2D(100.0f, 100.0f);

	UPROPERTY(EditDefaultsOnly, Category = "Crafting|Layout")
	FVector2D SlotPadding = FVector2D(8.0f, 8.0f);

	UPROPERTY()
	TArray<UInfrastructureIcon*> ResourceIcons;

	// === Right: cost / product ===
	UPROPERTY(meta = (BindWidget))
	UIcon* CostIcon;

	UPROPERTY(meta = (BindWidget))
	UIcon* ProductIcon;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CostQuantityText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ProductQuantityText;

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
	EResourceType SelectedInputType = EResourceType::Wood;

	FInfrastructureCraftRecipe CurrentRecipe;
	bool bHasValidRecipe = false;
	int32 CurrentProductQty = 0;

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
