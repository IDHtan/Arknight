// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../DS.h"
#include "BattleMapManager.generated.h"

class ABattleCell;

UCLASS()
class ARKNIGHT_API ABattleMapManager : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	float CellSize = 100.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	UDataTable* LevelConfigTable;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	int32 GridX = 0;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	int32 GridY = 0;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	TMap<FIntVector2, ABattleCell*> GridMap;

public:
	UFUNCTION(BlueprintCallable, Category = "GridLogic")
	void GenerateGrid(FName LevelID);

	UFUNCTION(BlueprintCallable, Category = "GridLogic")
	ABattleCell* GetCellFromCoordinate(FIntVector2 Coord);

	UFUNCTION(BlueprintCallable, Category = "GridLogic")
	bool IsCellDeployable(FIntVector2 CellCoord, EOperatorDeployType DeployType);

	UFUNCTION(BlueprintCallable, Category = "GridLogic")
	void ReplaceCell(FIntVector2 Coord, TSubclassOf<ABattleCell> NewCellClass);
	
	ABattleMapManager();

};
