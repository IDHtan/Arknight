// Fill out your copyright notice in the Description page of Project Settings.


#include "RunModifier_DelayedReward.h"
#include "../../Controller/HexMapSubsystem.h"
#include "../../../URougeliteRunSubsystem.h"

void URunModifier_DelayedReward::ExecuteDelayedResourceReward(URougeliteRunSubsystem* RunSubsystem)
{
	if (!RunSubsystem) return;

	UGameInstance* GI = RunSubsystem->GetGameInstance();
	UHexMapSubsystem* HexMap = GI ? GI->GetSubsystem<UHexMapSubsystem>() : nullptr;
	if (!HexMap) return;

	const int32 RewardAmount = HexMap->CurrentHexMapConsumedAP;
	if (RewardAmount > 0)
	{
		HexMap->AddHexMapResource(RewardType, RewardAmount);
		UE_LOG(LogTemp, Log, TEXT("RunModifier_DelayedReward: granted %d %s for consumed AP"),
			RewardAmount, *UEnum::GetValueAsString(RewardType));
	}

	// Return dispatched operators to the battle roster
	for (const FName& OpName : DispatchedOperators)
	{
		FOperatorRosterData* Found = RunSubsystem->GlobalRoster.FindByPredicate(
			[&OpName](const FOperatorRosterData& D) { return D.OperatorName == OpName; });
		if (Found)
		{
			Found->bCanJoinBattle = true;
			UE_LOG(LogTemp, Log, TEXT("RunModifier_DelayedReward: operator '%s' returned to roster"), *OpName.ToString());
		}
	}
}
