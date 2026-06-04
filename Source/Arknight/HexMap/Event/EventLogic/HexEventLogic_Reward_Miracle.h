// 奇迹奇遇
#pragma once
#include "CoreMinimal.h"
#include "../HexEventLogicBase.h"
#include "../Modifiers/RunModifier_NoMoveCost.h"
#include "../../Controller/HexMapSubsystem.h"
#include "HexEventLogic_Reward_Miracle.generated.h"

UCLASS()
class ARKNIGHT_API UHexEventLogic_Reward_Miracle : public UHexEventLogicBase
{
	GENERATED_BODY()
	virtual void ExecuteOption_Implementation(int32 OptionIndex) override
	{
		switch (OptionIndex)
		{
		case 0: // 退老还童 — refund all AP consumed this region
			MapSubsystemRef->ChangeCurrentAP(MapSubsystemRef->CurrentHexMapConsumedAP);
			break;
		case 1: { // 无限火力 — no movement cost this region
			URunModifier_NoMoveCost* Mod = NewObject<URunModifier_NoMoveCost>(this);
			Mod->Scope = EModifierScope::CurrentRegion;
			MapSubsystemRef->AddModifierInstance(Mod);
			break; }
		}
	}
};
