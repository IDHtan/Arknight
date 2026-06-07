// 搜打撤
#pragma once
#include "CoreMinimal.h"
#include "../HexEventLogicBase.h"
#include "Kismet/GameplayStatics.h"
#include "../../Controller/HexMapSubsystem.h"
#include "HexEventLogic_Metal_Search.generated.h"

UCLASS()
class ARKNIGHT_API UHexEventLogic_Metal_Search : public UHexEventLogicBase
{
	GENERATED_BODY()
	virtual void ExecuteOption_Implementation(int32 OptionIndex) override
	{
		switch (OptionIndex)
		{
		case 0: MapSubsystemRef->RevealAllRegionNodes(); break;
		case 1: {
			FName ID = MapSubsystemRef->GetRandomBattleLevelID(EHexNodeType::Combat_Emergency);
			if (!ID.IsNone()) { MapSubsystemRef->PrepareForBattle(ID, EHexNodeType::Combat_Emergency); UGameplayStatics::OpenLevel(this, TEXT("/Game/Maps/Lvl_BattleMap")); bTriggeredCombat = true; }
			break; }
		case 2: MapSubsystemRef->AddHexMapResource(EResourceType::Metal, 8); break;
		}
	}
};
