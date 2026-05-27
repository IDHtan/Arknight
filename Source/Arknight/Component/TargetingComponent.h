// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../DS.h"
#include "../Interface/TargetableInterface.h"
#include "TargetingComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ARKNIGHT_API UTargetingComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FIntVector2> BaseAttackRange;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	float TargetingInterval = 0.1f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 MaxTargets = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float AttackPower;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float AttackSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	EDamageType DamageType;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<FIntVector2> CurrentAbsoluteRange;

public:
	UFUNCTION(BlueprintCallable, Category = "Targeting|Logic")
	void InitializeAbsoluteRange(FIntVector2 Origin, EDeploymentDirection FacingDirection);
	
	UFUNCTION(BlueprintCallable, Category = "Targeting|Logic")
	TArray<TScriptInterface<ITargetableInterface>> PerformGridScan();

	UFUNCTION(BlueprintNativeEvent, Category = "Targeting|Logic")
	float CalculateTargetScore(const TScriptInterface<ITargetableInterface>& Target);

public:	
	UTargetingComponent();
		
};
