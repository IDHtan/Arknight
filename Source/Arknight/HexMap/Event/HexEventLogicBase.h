// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "HexEventLogicBase.generated.h"

class UHexMapSubsystem;
class URougeliteRunSubsystem;

/**
 * Handles consequence logic for a single random event or reward.
 *
 * Blueprint extension point — subclass this in Blueprint to implement
 * specific event outcomes. In ExecuteOption(), dynamically instantiate
 * URunModifierBase subclasses and register them via MapSubsystemRef->AddModifierInstance().
 */
UCLASS(Blueprintable)
class ARKNIGHT_API UHexEventLogicBase : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UHexMapSubsystem* MapSubsystemRef;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	URougeliteRunSubsystem* RunSubsystemRef;

	// Set to true in ExecuteOption_Implementation when the option triggers OpenLevel
	// (combat). Prevents HandleOptionSelected from closing the panel and calling
	// RefreshAllNodeStates, which would race ConcludeGame against OpenLevel when AP==0.
	bool bTriggeredCombat = false;

public:
	virtual void InitializeLogic(UGameInstance* GameInstance);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ExecuteOption(int32 OptionIndex);

	// Called by EventWidget after ExecuteOption to determine which stage to show next.
	// Return -1 → close event. Default: linear advance (CurrentStage + 1).
	// Override this for non-linear event flows (branches, cycles, early termination).
	virtual int32 DetermineNextStage(int32 CurrentStage, int32 SelectedOption)
	{
		return CurrentStage + 1;
	}
};
