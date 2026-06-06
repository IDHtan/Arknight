// Fill out your copyright notice in the Description page of Project Settings.

#include "MainMenuPlayerController.h"
#include "MainMenuHUDWidget.h"
#include "Blueprint/UserWidget.h"

void AMainMenuPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (MainMenuHUDClass)
	{
		MainMenuHUD = CreateWidget<UMainMenuHUDWidget>(this, MainMenuHUDClass);
		if (MainMenuHUD)
		{
			MainMenuHUD->AddToViewport();

			bShowMouseCursor = true;
			FInputModeUIOnly InputMode;
			InputMode.SetWidgetToFocus(MainMenuHUD->TakeWidget());
			SetInputMode(InputMode);
		}
	}
}
