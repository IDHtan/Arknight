// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../RunModifierBase.h"
#include "RunModifier_RevealOnEnter.generated.h"

// [迁徙] — reveal all nodes in the destination region after teleport
UCLASS()
class ARKNIGHT_API URunModifier_RevealOnEnter : public URunModifierBase
{
	GENERATED_BODY()

	virtual void ExecuteDelayedResourceReward(URougeliteRunSubsystem* RunSubsystem) override;
};
