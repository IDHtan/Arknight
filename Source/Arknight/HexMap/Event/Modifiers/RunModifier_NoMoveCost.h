// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../RunModifierBase.h"
#include "RunModifier_NoMoveCost.generated.h"

// [无限火力] — movement costs 0 AP in current region
UCLASS()
class ARKNIGHT_API URunModifier_NoMoveCost : public URunModifierBase
{
	GENERATED_BODY()

	virtual bool QueryNoMovementCost() const override { return true; }
};
