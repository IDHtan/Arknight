// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "../DS.h"
#include "BattlePlayerController.generated.h"

class ADeployableCell;
class UInputMappingContext;
class UInputAction;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRosterUpdatedSignature, const TArray<FOperatorLocalRosterData>&, CurrentRoster);
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
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "GameFlow")
	bool bIsGamePaused;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "GameFlow")
	EGameSpeedState CurrentSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* BattleMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* ClickAction;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category="Select")
	AOperatorBase* SelectedOperator;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Select")
	ABattleCell* SelectedCell;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Economy")
	int32 CurrentDeploymentCost = 10;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Roster")
	TArray<FOperatorLocalRosterData> LocalRoster;

public:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	virtual void SetupInputComponent() override;
	
public:
	void OnClickStarted();

public:
	FTimerHandle IncreaseCostNaturallyTimerHandle;

	UFUNCTION(BlueprintCallable)
	void IncreaseCost(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void ExecuteDeployment(FName OperatorName, ADeployableCell* DeployCell, EDeploymentDirection DeployDirection);



private:
	void RefreshLocalRosterDeployability();
};
