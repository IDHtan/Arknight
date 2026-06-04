// 开拓支援
#pragma once
#include "CoreMinimal.h"
#include "../HexEventLogicBase.h"
#include "../Modifiers/RunModifier_DelayedReward.h"
#include "../../Controller/HexMapSubsystem.h"
#include "../../../URougeliteRunSubsystem.h"
#include "HexEventLogic_Metal_Support.generated.h"

UCLASS()
class ARKNIGHT_API UHexEventLogic_Metal_Support : public UHexEventLogicBase
{
	GENERATED_BODY()
	virtual void ExecuteOption_Implementation(int32 OptionIndex) override
	{
		switch (OptionIndex)
		{
		case 0: { // 驰援险地 — dispatch 2 random operators
			TArray<FName> Candidates;
			for (const FOperatorRosterData& Op : RunSubsystemRef->GlobalRoster)
			{
				if (Op.bCanJoinBattle)
					Candidates.Add(Op.OperatorName);
			}

			if (Candidates.Num() >= 2)
			{
				// Shuffle and pick first 2
				for (int32 i = Candidates.Num() - 1; i > 0; --i)
				{
					int32 j = FMath::RandRange(0, i);
					Candidates.Swap(i, j);
				}

				FOperatorRosterData* OpA = RunSubsystemRef->GlobalRoster.FindByPredicate(
					[&](const FOperatorRosterData& D) { return D.OperatorName == Candidates[0]; });
				FOperatorRosterData* OpB = RunSubsystemRef->GlobalRoster.FindByPredicate(
					[&](const FOperatorRosterData& D) { return D.OperatorName == Candidates[1]; });

				if (OpA) OpA->bCanJoinBattle = false;
				if (OpB) OpB->bCanJoinBattle = false;

				// Delayed reward: Metal = consumed AP, operators return
				URunModifier_DelayedReward* Mod = NewObject<URunModifier_DelayedReward>(this);
				Mod->RewardType = EResourceType::Metal;
				Mod->DispatchedOperators = { Candidates[0], Candidates[1] };
				Mod->Scope = EModifierScope::CurrentRegion;
				MapSubsystemRef->AddModifierInstance(Mod);

				UE_LOG(LogTemp, Log, TEXT("Metal_Support: dispatched %s and %s"), *Candidates[0].ToString(), *Candidates[1].ToString());
			}
			else
			{
				// Fallback: not enough operators, just give Metal
				MapSubsystemRef->AddHexMapResource(EResourceType::Metal, 10);
			}
			break; }
		case 1: // 安逸探索
			MapSubsystemRef->AddHexMapResource(EResourceType::Metal, 10);
			break;
		}
	}
};
