// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MainMenuGameMode.generated.h"

/**
 * GameMode for Lvl_MainMenu.
 * Sets default PlayerController to AMainMenuPlayerController so the
 * MainMenuHUDWidget is automatically created on level start.
 */
UCLASS()
class ARKNIGHT_API AMainMenuGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMainMenuGameMode();
};