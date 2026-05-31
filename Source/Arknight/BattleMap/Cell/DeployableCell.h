// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../DS.h"
#include "BattleCell.h"
#include "DeployableCell.generated.h"

/**
 * 
 */
UCLASS()
class ARKNIGHT_API ADeployableCell : public ABattleCell
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "DeployState")
	bool bIsOccupied;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "DeployState")
	AOperatorBase* OccupyingOperator;

public:
	UFUNCTION(BlueprintCallable)
	void SetOccupyingOperator(AOperatorBase* Operator);

	UFUNCTION(BlueprintCallable)
	bool CanDeploy(EOperatorDeployType DeployTpe);

	UFUNCTION(BlueprintCallable)
	void OnOperatorCardClicked(FName OperatorName);

	UFUNCTION(BlueprintImplementableEvent, Category = "CellVisual")
	void EnableVisualEffect();

	UFUNCTION(BlueprintImplementableEvent, Category = "CellVisual")
	void DisableVisualEffect();
};
