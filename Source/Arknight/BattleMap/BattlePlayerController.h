// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "../DS.h"
#include "BattlePlayerController.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRosterUpdatedSignature, const TArray<FOperatorRosterData>&, CurrentRoster);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCostChangedSignature, int32, NewCost);
/**
 * 
 */
UCLASS()
class ARKNIGHT_API ABattlePlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnRosterUpdatedSignature OnRosterUpdated;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnCostChangedSignature OnCostChanged;
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Economy")
	int32 CurrentDeploymentCost = 10;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Roster")
	TArray<FOperatorRosterData> LocalRoster;

public:
	virtual void BeginPlay() override;

	/*UFUNCTION(BlueprintCallable, Category = "Interaction")
	void ExecuteDeployment(FName OperatorName, EDeploymentDirection DeployDirection);*/
};
