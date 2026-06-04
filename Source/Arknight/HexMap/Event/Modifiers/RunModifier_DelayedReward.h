// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../RunModifierBase.h"
#include "RunModifier_DelayedReward.generated.h"

// [驰援险地] — at region end, grant reward proportional to AP consumed this region
// and return dispatched operators to the battle roster.
UCLASS()
class ARKNIGHT_API URunModifier_DelayedReward : public URunModifierBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modifier")
	EResourceType RewardType = EResourceType::Metal;

	// Operators sent on this mission — excluded from battle, returned on region end
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modifier")
	TArray<FName> DispatchedOperators;

	virtual void ExecuteDelayedResourceReward(URougeliteRunSubsystem* RunSubsystem) override;
};
