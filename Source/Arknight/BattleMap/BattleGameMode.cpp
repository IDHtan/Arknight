// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "BattlePlayerController.h"
#include "BattleCameraPawn.h"
#include "BattleMapManager.h"
#include "../URougeliteRunSubsystem.h"

ABattleGameMode::ABattleGameMode()
{

}

void ABattleGameMode::BeginPlay()
{
	Super::BeginPlay();

	ABattleMapManager* MapManager = GetWorld()->SpawnActor<ABattleMapManager>();
	URougeliteRunSubsystem* RunSubsystem = GetGameInstance()->GetSubsystem<URougeliteRunSubsystem>();
	FName PendingLevelID = RunSubsystem->ConsumePendingBattleID();
	MapManager->GenerateGrid(PendingLevelID);

	ABattlePlayerController* PlayerController = Cast<ABattlePlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	ABattleCameraPawn* CameraPawn = Cast<ABattleCameraPawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (PlayerController && CameraPawn)
	{
		PlayerController->SetViewTarget(CameraPawn);
	}
	CameraPawn->FrameGridToScreen(MapManager->GridX, MapManager->GridY, MapManager->CellSize);
}
