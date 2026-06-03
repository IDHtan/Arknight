// LvlDebugGameMode.cpp

#include "LvlDebugGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "../HexMap/Controller/HexMapSubsystem.h"
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
		if (UHexMapSubsystem* Subsystem = GI->GetSubsystem<UHexMapSubsystem>())
		{
			Subsystem->PrepareForBattle(LevelID, EHexNodeType::Combat_Normal);
			UGameplayStatics::OpenLevel(this, TEXT("Lvl_BattleMap"));
		}
	}

}
