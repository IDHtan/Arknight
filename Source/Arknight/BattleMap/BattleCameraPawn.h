// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "BattleCameraPawn.generated.h"

UCLASS()
class ARKNIGHT_API ABattleCameraPawn : public APawn
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera") 
	USceneComponent* RootComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera") 
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera") 
	UCameraComponent* Camera;

public:
	ABattleCameraPawn();

	UFUNCTION(BlueprintCallable, Category = "CameraLogic") 
	void FrameGridToScreen(int32 GridX, int32 GridY, float CellSize);
};
