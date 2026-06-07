// 炊烟袅袅
#pragma once
#include "CoreMinimal.h"
#include "../HexEventLogicBase.h"
#include "Kismet/GameplayStatics.h"
#include "../../Controller/HexMapSubsystem.h"
#include "HexEventLogic_Rock_Cooking.generated.h"

UCLASS()
class ARKNIGHT_API UHexEventLogic_Rock_Cooking : public UHexEventLogicBase
{
	GENERATED_BODY()
	virtual void ExecuteOption_Implementation(int32 OptionIndex) override
	{
		switch (OptionIndex)
		{
		case 0: MapSubsystemRef->AddHexMapResource(EResourceType::Grain, 10); break;
		case 1: {
			MapSubsystemRef->ChangeCurrentAP(2);
			const int32 Grain = MapSubsystemRef->GetResource(EResourceType::Grain);
			MapSubsystemRef->AddHexMapResource(EResourceType::Grain, -(Grain * 60 / 100));
			break; }
		case 2: {
			FName ID = MapSubsystemRef->GetRandomBattleLevelID(EHexNodeType::Combat_Normal);
			if (!ID.IsNone()) { MapSubsystemRef->PrepareForBattle(ID, EHexNodeType::Combat_Normal); UGameplayStatics::OpenLevel(this, TEXT("/Game/Maps/Lvl_BattleMap")); bTriggeredCombat = true; }
			break; }
		}
	}
};
