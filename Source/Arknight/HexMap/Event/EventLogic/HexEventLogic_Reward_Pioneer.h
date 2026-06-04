// 绝境开拓
#pragma once
#include "CoreMinimal.h"
#include "../HexEventLogicBase.h"
#include "../../Controller/HexMapSubsystem.h"
#include "HexEventLogic_Reward_Pioneer.generated.h"

UCLASS()
class ARKNIGHT_API UHexEventLogic_Reward_Pioneer : public UHexEventLogicBase
{
	GENERATED_BODY()
	virtual void ExecuteOption_Implementation(int32 OptionIndex) override
	{
		switch (OptionIndex)
		{
		case 0: { // 打好基础 — 60 of a random basic resource
			static const EResourceType Basics[] = { EResourceType::Wood, EResourceType::Rock, EResourceType::Metal, EResourceType::Grain };
			MapSubsystemRef->AddHexMapResource(Basics[FMath::RandRange(0, 3)], 60);
			break; }
		case 1: // 深入开拓 — 30 HighQualityFood
			MapSubsystemRef->AddHexMapResource(EResourceType::HighQualityFood, 30);
			break;
		}
	}
};
