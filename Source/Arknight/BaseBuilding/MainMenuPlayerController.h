// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainMenuPlayerController.generated.h"

class UMainMenuHUDWidget;

/**
 * Player controller for Lvl_MainMenu.
 * Creates and pushes the MainMenuHUDWidget to viewport on BeginPlay.
 */
UCLASS()
class ARKNIGHT_API AMainMenuPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "MainMenu")
	TSubclassOf<UMainMenuHUDWidget> MainMenuHUDClass;

	UPROPERTY()
	UMainMenuHUDWidget* MainMenuHUD = nullptr;
};
