// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "BattlePlayerController.h"
#include "BattleCameraPawn.h"
#include "../BattleMapManager.h"
#include "../../HexMap/Controller/HexMapSubsystem.h"

ABattleGameMode::ABattleGameMode()
{

}

void ABattleGameMode::BeginPlay()
{
	Super::BeginPlay();

	ABattleMapManager* MapManager = GetWorld()->SpawnActor<ABattleMapManager>();
	UHexMapSubsystem* HexMapSubsystem = GetGameInstance()->GetSubsystem<UHexMapSubsystem>();
	const FName PendingLevelID = HexMapSubsystem ? HexMapSubsystem->ConsumePendingBattleID() : NAME_None;
	if (MapManager && !PendingLevelID.IsNone())
	{
		MapManager->GenerateGrid(PendingLevelID);
	}

	ABattlePlayerController* PlayerController = Cast<ABattlePlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	ABattleCameraPawn* CameraPawn = Cast<ABattleCameraPawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (PlayerController && CameraPawn)
	{
		PlayerController->SetViewTarget(CameraPawn);
	}
	if (MapManager && CameraPawn)
	{
		CameraPawn->FrameGridToScreen(MapManager->GridX, MapManager->GridY, MapManager->CellSize);
	}
}
