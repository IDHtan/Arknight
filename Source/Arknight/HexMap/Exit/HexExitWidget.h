// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HexExitWidget.generated.h"

class UButton;

/**
 * Exit confirmation dialog on the HexMap HUD.
 * Asks the player to confirm leaving the current run.
 * Confirm → ConcludeGame, Cancel → dismiss panel.
 */
UCLASS()
class ARKNIGHT_API UHexExitWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UButton* ConfirmButton;

	UPROPERTY(meta = (BindWidget))
	UButton* CancelButton;

public:
	UFUNCTION(BlueprintCallable)
	void Open();

	UFUNCTION()
	void OnConfirmClicked();

	UFUNCTION()
	void OnCancelClicked();
};
