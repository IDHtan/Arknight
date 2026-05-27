// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "DS.h"
#include "RougeliteSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class ARKNIGHT_API URougeliteSaveGame : public USaveGame
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TMap<EResourceType, int32> GlobalResources;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TMap<FName, int32> OperatorLevels;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int32 PlayerLevel = 1;
};
