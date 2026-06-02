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

public:
	virtual void InitializeLogic(UGameInstance* GameInstance);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ExecuteOption(int32 OptionIndex);
};
