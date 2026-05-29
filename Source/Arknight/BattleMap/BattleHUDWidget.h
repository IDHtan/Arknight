// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../DS.h"
#include "BattleHUDWidget.generated.h"

class UExitWidget;
class UTextBlock;
class UCheckBox;
class UButton;
class UOperatorDetailWidget;
class UCellDetailWidget;

/**
 * 
 */
UCLASS()
class ARKNIGHT_API UBattleHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidget))
	UCheckBox* PauseCheckBox;

	UPROPERTY(meta=(BindWidget))
	UCheckBox* SpeedCheckBox;

	UPROPERTY(meta=(BindWidget))
	UButton* ExitButton;

	UPROPERTY(meta = (BindWidget))
	UExitWidget* ExitConfirmPanel;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextCost;
	
	UPROPERTY(meta = (BindWidget))
	UOperatorDetailWidget* OperatorDetailPanel;

	UPROPERTY(meta = (BindWidget))
	UCellDetailWidget* CellDetailPanel;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "GameFlow")
	bool bIsGamePaused;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "GameFlow")
	EGameSpeedState CurrentSpeed;

public:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void ChangePauseState(bool bIsChecked);

	UFUNCTION()
	void ChangeSpeedState(bool bIsChecked);

	UFUNCTION()
	void OnExitButtonClicked();

	UFUNCTION()
	void OnCostUpdated(int32 NewCost);
	
	UFUNCTION(BlueprintCallable)
	void ShowOperatorSelected(AOperatorBase* SelectedOp);

	UFUNCTION(BlueprintCallable)
	void ShowOperatorDetailByClass(TSubclassOf<AOperatorBase> OpClass);

	UFUNCTION(BlueprintCallable)
	void ShowCellSelected(AResourceCell* SelectedCell);

	UFUNCTION(BlueprintCallable)
	void HideAllDetails();
};
