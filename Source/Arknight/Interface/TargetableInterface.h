// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Math/MathFwd.h"
#include "../DS.h"
#include "TargetableInterface.generated.h"


UINTERFACE(MinimalAPI)
class UTargetableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ARKNIGHT_API ITargetableInterface
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Targeting")
	FIntVector2 GetGridPosition() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Targeting")
	bool CanBeTargetedBy(AActor* Instigator) const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
	void TakeCombatDamage(float DamageAmount, EDamageType DamageType, AActor* DamageInstigator);
};
