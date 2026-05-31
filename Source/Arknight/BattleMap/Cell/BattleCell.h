// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../../Interface/TargetableInterface.h"
#include "BattleCell.generated.h"

UCLASS()
class ARKNIGHT_API ABattleCell : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* CellMeshComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CellData")
	ECellType LogicalCellType;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "CellData")
	FIntVector2 GridCoordinate;

public:
	UFUNCTION(BlueprintCallable, Category = "CellLogic")
	virtual void InitializeCell(FIntVector2 Coord);

	ABattleCell();

};
