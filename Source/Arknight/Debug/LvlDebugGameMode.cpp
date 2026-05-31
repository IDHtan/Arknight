// LvlDebugGameMode.cpp

#include "LvlDebugGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "../URougeliteRunSubsystem.h"
#include "Engine/GameInstance.h"

void ALvlDebugGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (LevelID.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("LvlDebugGameMode: LevelID is None."));
		return;
	}

	if (UGameInstance* GI = UGameplayStatics::GetGameInstance(this))
	{
		if (URougeliteRunSubsystem* Subsystem = GI->GetSubsystem<URougeliteRunSubsystem>())
		{
			Subsystem->PendingBattleLevelID = LevelID;
			Subsystem->PrepareForBattle(TEXT("Lvl_BattleMap"));
		}
	}

}
