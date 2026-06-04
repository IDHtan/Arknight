// 单车变？ (multi-stage gambling)
#pragma once
#include "CoreMinimal.h"
#include "../HexEventLogicBase.h"
#include "../../Controller/HexMapSubsystem.h"
#include "HexEventLogic_Wood_Bicycle.generated.h"

UCLASS()
class ARKNIGHT_API UHexEventLogic_Wood_Bicycle : public UHexEventLogicBase
{
	GENERATED_BODY()

	// Stored across stages for the multi-stage gamble
	int32 OriginalOwned = 0;

	virtual void ExecuteOption_Implementation(int32 OptionIndex) override
	{
		switch (OptionIndex)
		{
		case 0: // 变单车 — simple reward
			MapSubsystemRef->AddHexMapResource(EResourceType::Wood, 3);
			break;

		case 1: // 变摩托 Stage 0 — lose 50%, 60% chance to get 2× back
			{
				OriginalOwned = MapSubsystemRef->GetResource(EResourceType::Wood);
				const int32 Lost = OriginalOwned / 2;
				MapSubsystemRef->AddHexMapResource(EResourceType::Wood, -Lost);

				if (FMath::FRand() < 0.6f)
				{
					MapSubsystemRef->AddHexMapResource(EResourceType::Wood, OriginalOwned);
				}
			}
			break;

		case 2: // Stage 1 — continue: lose 100% remaining, 34% double
			{
				const int32 Remaining = MapSubsystemRef->GetResource(EResourceType::Wood);
				MapSubsystemRef->AddHexMapResource(EResourceType::Wood, -Remaining);

				if (FMath::FRand() < 0.34f)
				{
					MapSubsystemRef->AddHexMapResource(EResourceType::Wood, OriginalOwned * 2);
				}
			}
			break;

		case 3: // Stage 1 — quit: do nothing
			break;
		}
	}

	// Non-full-tree: option 0 ends immediately, option 1 advances to stage 1
	virtual int32 DetermineNextStage(int32 CurrentStage, int32 SelectedOption) override
	{
		if (CurrentStage == 0 && SelectedOption == 0)
			return -1; // 变单车 → close event
		return CurrentStage + 1; // default linear advance
	}
};
