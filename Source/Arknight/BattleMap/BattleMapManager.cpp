// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleMapManager.h"
#include "../BattleMap/BattleCell.h"
#include "../RougeliteSettings.h"

void ABattleMapManager::GenerateGrid(FName LevelID)
{
	FLevelLayoutConfig* LevelConfig = LevelConfigTable->FindRow<FLevelLayoutConfig>(LevelID, TEXT("Load Level"));
	if(!LevelConfig)
	{
		UE_LOG(LogTemp, Warning, TEXT("LevelID %s not found in LevelConfigTable"), *LevelID.ToString());
		return;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("LevelID %s is founded in LevelConfigTable"), *LevelID.ToString());
	}

	GridY = LevelConfig->GridY;
	GridX = LevelConfig->GridX;
	for (int32 i = 0; i < GridX * GridY; i++) {
		int32 X = i % GridX;
		int32 Y = i / GridX;
		FIntVector2 Coord(X, Y);
		TSubclassOf<ABattleCell> CellClass = LevelConfig->LayoutData.IsValidIndex(i) ? LevelConfig->LayoutData[i] : nullptr;
		if(CellClass)
		{
			FVector SpawnLocation(X*CellSize, Y*CellSize, 0);
			FRotator SpawnRotation = FRotator::ZeroRotator;
			ABattleCell* NewCell = GetWorld()->SpawnActor<ABattleCell>(CellClass, SpawnLocation, SpawnRotation);
			GridMap.Add(Coord, NewCell);
			NewCell->InitializeCell(Coord);
			UE_LOG(LogTemp, Warning, TEXT("A Cell is spawned at %s"), *Coord.ToString());
		}
		else
		{
			GridMap.Add(Coord, nullptr);
			UE_LOG(LogTemp, Warning, TEXT("A empty cell is spawned at %s"), *Coord.ToString());
		}
	}
}

ABattleCell* ABattleMapManager::GetCellFromCoordinate(FIntVector2 Coord)
{
	return GridMap.Contains(Coord) ? GridMap[Coord] : nullptr;
}

//this function wont be used, lets see
bool ABattleMapManager::IsCellDeployable(FIntVector2 CellCoord, EOperatorDeployType DeployType)
{
	ABattleCell* Cell = GetCellFromCoordinate(CellCoord);
	if(!Cell) 	{
		return false;
	}
	switch (DeployType) {
		case EOperatorDeployType::Melee:
			return Cell->LogicalCellType == ECellType::DeployableGround;
		case EOperatorDeployType::Ranged:
			return Cell->LogicalCellType == ECellType::DeployableHighGround;
		case EOperatorDeployType::Any:
			return Cell->LogicalCellType == ECellType::DeployableGround || Cell->LogicalCellType == ECellType::DeployableHighGround;
		default:
			return false;
	}
}

void ABattleMapManager::ReplaceCell(FIntVector2 Coord, TSubclassOf<ABattleCell> NewCellClass)
{
	ABattleCell* OldCell = GetCellFromCoordinate(Coord);
	if(OldCell)
	{
		OldCell->Destroy();
		GridMap[Coord] = nullptr;
	}
	if(NewCellClass)
	{
		FVector SpawnLocation(Coord.X*CellSize, Coord.Y*CellSize, 0);
		FRotator SpawnRotation = FRotator::ZeroRotator;
		ABattleCell* NewCell = GetWorld()->SpawnActor<ABattleCell>(NewCellClass, SpawnLocation, SpawnRotation);
		GridMap[Coord] = NewCell;
		NewCell->InitializeCell(Coord);
	}
}

ABattleMapManager::ABattleMapManager()
{
	PrimaryActorTick.bCanEverTick = false;

	const URougeliteSettings* Settings = GetDefault<URougeliteSettings>();
	LevelConfigTable = Settings->LevelLayoutTable.LoadSynchronous();

}


