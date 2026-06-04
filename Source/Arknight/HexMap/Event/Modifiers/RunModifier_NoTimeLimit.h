// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../RunModifierBase.h"
#include "RunModifier_NoTimeLimit.generated.h"

// [巧击] — disables the emergency combat timer
UCLASS()
class ARKNIGHT_API URunModifier_NoTimeLimit : public URunModifierBase
{
	GENERATED_BODY()

	virtual bool QueryDisableCombatTimeLimit() const override { return true; }
};
