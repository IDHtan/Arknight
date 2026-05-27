// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "RougeliteSettings.generated.h"

/**
 * 
 */
UCLASS(Config = Game, defaultconfig)
class ARKNIGHT_API URougeliteSettings : public UDeveloperSettings
{
    GENERATED_BODY()

public:
    UPROPERTY(Config, EditAnywhere, Category = "DataTables")
    TSoftObjectPtr<UDataTable> OperatorDictTable;

    UPROPERTY(Config, EditAnywhere, Category = "DataTables")
    TSoftObjectPtr<UDataTable> LevelLayoutTable;
};