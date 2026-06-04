// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "../../DS.h"
#include "BattlePlayerController.generated.h"

class ADeployableCell;
class AResourceCell;
class UInputMappingContext;
class UInputAction;
class UBattleHUDWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCostChangedSignature, int32, NewCost);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOperatorCardClicked, FName, OperatorName);
/**
 * 
 */
UCLASS()
class ARKNIGHT_API ABattlePlayerController : public APlayerController
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnCostChangedSignature OnCostChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnOperatorCardClicked OnOperatorCardClicked;
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UBattleHUDWidget> HUDWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	UBattleHUDWidget* HUDWidgetInstance;

#pragma region Input
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* BattleMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* ClickAction;
#pragma endregion	

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category="Select")
	AOperatorBase* SelectedOperator;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Select")
	FName SelectedOperatorCardName;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Select")
	AResourceCell* SelectedResourceCell;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Select")
	ADeployableCell* SelectedDeployableCell;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Economy")
	int32 CurrentDeploymentCost = 10;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Roster")
	TArray<FOperatorLocalRosterData> LocalRoster;

public:
	ABattlePlayerController();

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	virtual void Tick(float DeltaSeconds) override;

	virtual void SetupInputComponent() override;
	
public:
	UFUNCTION(BlueprintCallable)
	void OnClickStarted();

	UFUNCTION(BlueprintCallable)
	void OperatorCardChanged(FName OperatorName);

public:
#pragma region Cost
	FTimerHandle IncreaseCostNaturallyTimerHandle;

	UFUNCTION(BlueprintCallable)
	void IncreaseCost(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void Deploy(FName OperatorName, ADeployableCell* DeployCell, EDeploymentDirection DeployDirection);

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void Retreat(AOperatorBase* Operat);
};
