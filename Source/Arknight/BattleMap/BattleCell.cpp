// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleCell.h"

void ABattleCell::InitializeCell(FIntVector2 Coord)
{
	GridCoordinate = Coord;
}


ABattleCell::ABattleCell()
{
	PrimaryActorTick.bCanEverTick = false;

	CellMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CellMesh"));
	RootComponent = CellMeshComponent;
}

