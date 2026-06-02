// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "../../DS.h"
#include "HexMapPlayerController.generated.h"

class UHexMapHUDWidget;
class UHexMapSubsystem;

/**
 * 
 */
UCLASS()
class ARKNIGHT_API AHexMapPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UHexMapHUDWidget> MapWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	UHexMapHUDWidget* MapWidgetInstance = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "System")
	UHexMapSubsystem* HexMapSubsystemP = nullptr;

	UFUNCTION(BlueprintCallable, Category = "HexMap|Input")
	void HandleNodeClicked(FIntVector2 Coordinate);
};
