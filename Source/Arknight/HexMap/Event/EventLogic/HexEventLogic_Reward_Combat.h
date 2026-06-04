// 战潮复归
#pragma once
#include "CoreMinimal.h"
#include "../HexEventLogicBase.h"
#include "../Modifiers/RunModifier_GainMultiplier.h"
#include "../Modifiers/RunModifier_NoTimeLimit.h"
#include "../../Controller/HexMapSubsystem.h"
#include "HexEventLogic_Reward_Combat.generated.h"

UCLASS()
class ARKNIGHT_API UHexEventLogic_Reward_Combat : public UHexEventLogicBase
{
	GENERATED_BODY()
	virtual void ExecuteOption_Implementation(int32 OptionIndex) override
	{
		switch (OptionIndex)
		{
		case 0: { // 强袭 — emergency resource ×3 (×1.5 on ×2 base)
			URunModifier_GainMultiplier* Mod = NewObject<URunModifier_GainMultiplier>(this);
			Mod->Multiplier = 1.5f;
			Mod->bEmergenyOnly = true;
			Mod->Scope = EModifierScope::CurrentRegion;
			MapSubsystemRef->AddModifierInstance(Mod);
			break; }
		case 1: { // 巧击 — no emergency time limit
			URunModifier_NoTimeLimit* Mod = NewObject<URunModifier_NoTimeLimit>(this);
			Mod->Scope = EModifierScope::CurrentRegion;
			MapSubsystemRef->AddModifierInstance(Mod);
			break; }
		}
	}
};
