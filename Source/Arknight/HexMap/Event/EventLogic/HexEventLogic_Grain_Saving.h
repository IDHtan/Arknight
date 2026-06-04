// 储财
#pragma once
#include "CoreMinimal.h"
#include "../HexEventLogicBase.h"
#include "../Modifiers/RunModifier_BanResource.h"
#include "../Modifiers/RunModifier_GainMultiplier.h"
#include "../../Controller/HexMapSubsystem.h"
#include "HexEventLogic_Grain_Saving.generated.h"

UCLASS()
class ARKNIGHT_API UHexEventLogic_Grain_Saving : public UHexEventLogicBase
{
	GENERATED_BODY()
	virtual void ExecuteOption_Implementation(int32 OptionIndex) override
	{
		switch (OptionIndex)
		{
		case 0: { // 我要存钱
			// Lose all Rock
			const int32 RockOwned = MapSubsystemRef->GetResource(EResourceType::Rock);
			MapSubsystemRef->AddHexMapResource(EResourceType::Rock, -RockOwned);

			// Ban Rock acquisition this region
			URunModifier_BanResource* Ban = NewObject<URunModifier_BanResource>(this);
			Ban->BannedType = EResourceType::Rock;
			Ban->Scope = EModifierScope::CurrentRegion;
			MapSubsystemRef->AddModifierInstance(Ban);

			// Double resource gain next region
			URunModifier_GainMultiplier* Double = NewObject<URunModifier_GainMultiplier>(this);
			Double->Multiplier = 2.0f;
			Double->bEmergenyOnly = false;
			Double->Scope = EModifierScope::NextRegion;
			MapSubsystemRef->AddModifierInstance(Double);
			break; }
		case 1: // 我要提钱
			MapSubsystemRef->AddHexMapResource(EResourceType::Rock, 10);
			break;
		}
	}
};
