// 人海战术
#pragma once
#include "CoreMinimal.h"
#include "../HexEventLogicBase.h"
#include "../Modifiers/RunModifier_AttackBonus.h"
#include "../../Controller/HexMapSubsystem.h"
#include "../../../URougeliteRunSubsystem.h"
#include "HexEventLogic_Reward_Tactics.generated.h"

UCLASS()
class ARKNIGHT_API UHexEventLogic_Reward_Tactics : public UHexEventLogicBase
{
	GENERATED_BODY()
	virtual void ExecuteOption_Implementation(int32 OptionIndex) override
	{
		switch (OptionIndex)
		{
		case 0: { // 人潮人海 — +90 attack permanently
			URunModifier_AttackBonus* Mod = NewObject<URunModifier_AttackBonus>(this);
			Mod->BonusAttack = 90.0f;
			Mod->Scope = EModifierScope::Permanent;
			MapSubsystemRef->AddModifierInstance(Mod);
			break; }
		case 1: // 人才引进 — unlock Amiya
			RunSubsystemRef->AddOperator(FName(TEXT("Amiya")));
			break;
		}
	}
};
