// 矿洞探索
#pragma once
#include "CoreMinimal.h"
#include "../HexEventLogicBase.h"
#include "Kismet/GameplayStatics.h"
#include "../../Controller/HexMapSubsystem.h"
#include "HexEventLogic_Grain_Mine.generated.h"

UCLASS()
class ARKNIGHT_API UHexEventLogic_Grain_Mine : public UHexEventLogicBase
{
	GENERATED_BODY()
	virtual void ExecuteOption_Implementation(int32 OptionIndex) override
	{
		switch (OptionIndex)
		{
		case 0: MapSubsystemRef->AddHexMapResource(EResourceType::Rock, 5); break;
		case 1:
			if (FMath::FRand() < 0.6f) MapSubsystemRef->AddHexMapResource(EResourceType::Rock, 15);
			break;
		case 2: {
			FName ID = MapSubsystemRef->GetRandomBattleLevelID(EHexNodeType::Combat_Emergency);
			if (!ID.IsNone()) { MapSubsystemRef->PrepareForBattle(ID, EHexNodeType::Combat_Emergency); UGameplayStatics::OpenLevel(this, TEXT("Lvl_BattleMap")); bTriggeredCombat = true; }
			break; }
		}
	}
};
