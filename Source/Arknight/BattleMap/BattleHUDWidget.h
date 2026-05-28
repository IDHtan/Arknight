// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../DS.h"
#include "BattleHUDWidget.generated.h"

class UExitWidget;

class UCheckBox;
class UButton;

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
	

};
