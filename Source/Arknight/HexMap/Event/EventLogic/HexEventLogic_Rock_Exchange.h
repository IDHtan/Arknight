// 以物换物
#pragma once
#include "CoreMinimal.h"
#include "../HexEventLogicBase.h"
#include "../../Controller/HexMapSubsystem.h"
#include "HexEventLogic_Rock_Exchange.generated.h"

UCLASS()
class ARKNIGHT_API UHexEventLogic_Rock_Exchange : public UHexEventLogicBase
{
	GENERATED_BODY()
	virtual void ExecuteOption_Implementation(int32 OptionIndex) override
	{
		switch (OptionIndex)
		{
		case 0: { // 真以物换物 — up to 10 Grain 1:1 for Wood/Rock/Metal
			const int32 Grain = MapSubsystemRef->GetResource(EResourceType::Grain);
			const int32 Amount = FMath::Min(Grain, 10);
			MapSubsystemRef->AddHexMapResource(EResourceType::Grain, -Amount);
			MapSubsystemRef->AddHexMapResource(EResourceTypeMeta::GetRegionPrimaryResource(MapSubsystemRef->CurrentRegion), Amount);
			break; }
		case 1: MapSubsystemRef->AddHexMapResource(EResourceType::Grain, 8); break;
		case 2: { // 本手·妙手·俗手
			const int32 Lost = MapSubsystemRef->GetResource(EResourceType::Grain) / 2;
			MapSubsystemRef->AddHexMapResource(EResourceType::Grain, -Lost);
			switch (FMath::RandRange(0, 2))
			{
			case 0: MapSubsystemRef->AddHexMapResource(EResourceType::Grain, Lost); break;
			case 1: MapSubsystemRef->AddHexMapResource(EResourceType::Grain, Lost * 2); break;
			case 2: MapSubsystemRef->AddHexMapResource(EResourceType::Grain, 1); break;
			}
			break; }
		}
	}
};
