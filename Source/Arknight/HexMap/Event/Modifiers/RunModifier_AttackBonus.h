// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../RunModifierBase.h"
#include "RunModifier_AttackBonus.generated.h"

// [人潮人海] — flat attack bonus for all operators in battle
UCLASS()
class ARKNIGHT_API URunModifier_AttackBonus : public URunModifierBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modifier")
	float BonusAttack = 90.0f;

	virtual float QueryGlobalAttackFlatBonus(float BaseAttack) const override
	{
		return BaseAttack + BonusAttack;
	}
};
