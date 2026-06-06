// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Materials/MaterialInstanceDynamic.h"
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

	UFUNCTION(BlueprintNativeEvent, Category = "CellVisual")
	void EnableVisualEffect();
	void EnableVisualEffect_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "CellVisual")
	void DisableVisualEffect();
	void DisableVisualEffect_Implementation();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	UMaterialInstanceDynamic* DynamicMaterialInstance;

	UPROPERTY(EditDefaultsOnly, Category = "CellVisual")
	FLinearColor EnabledTintColor = FLinearColor(0.25f, 1.0f, 0.25f, 1.0f);

	UPROPERTY(EditDefaultsOnly, Category = "CellVisual")
	FName TintParameterName = TEXT("TintColor");
};
