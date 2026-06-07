// 荒地拾遗 //have a little problem with adding AP directly
#pragma once
#include "CoreMinimal.h"
#include "../HexEventLogicBase.h"
#include "Kismet/GameplayStatics.h"
#include "../../Controller/HexMapSubsystem.h"
#include "HexEventLogic_Wood_Wasteland.generated.h"

UCLASS()
class ARKNIGHT_API UHexEventLogic_Wood_Wasteland : public UHexEventLogicBase
{
	GENERATED_BODY()
	virtual void ExecuteOption_Implementation(int32 OptionIndex) override
	{
		switch (OptionIndex)
		{
		case 0: MapSubsystemRef->AddHexMapResource(EResourceType::Wood, 5); break;
		case 1: {
			FName ID = MapSubsystemRef->GetRandomBattleLevelID(EHexNodeType::Combat_Normal);
			if (!ID.IsNone()) { MapSubsystemRef->PrepareForBattle(ID, EHexNodeType::Combat_Normal); UGameplayStatics::OpenLevel(this, TEXT("/Game/Maps/Lvl_BattleMap")); bTriggeredCombat = true; }
			break; }
		case 2: {
			FName ID = MapSubsystemRef->GetRandomBattleLevelID(EHexNodeType::Combat_Emergency);
			if (!ID.IsNone()) { MapSubsystemRef->PrepareForBattle(ID, EHexNodeType::Combat_Emergency); UGameplayStatics::OpenLevel(this, TEXT("/Game/Maps/Lvl_BattleMap")); bTriggeredCombat = true; }
			break; }
		}
	}
};
