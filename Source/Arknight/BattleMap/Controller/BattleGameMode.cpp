// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "BattlePlayerController.h"
#include "BattleCameraPawn.h"
#include "../BattleMapManager.h"
#include "../Cell/ResourceCell.h"
#include "../UI/SettlementWidget.h"
#include "../../HexMap/Controller/HexMapSubsystem.h"
#include "../../HexMap/Event/RunModifierBase.h"

ABattleGameMode::ABattleGameMode()
{
}

void ABattleGameMode::BeginPlay()
{
	Super::BeginPlay();

	// --- Read combat type from HexMapSubsystem ---
	UHexMapSubsystem* HexMap = GetGameInstance()->GetSubsystem<UHexMapSubsystem>();
	CurrentBattleType = HexMap ? HexMap->PendingBattleType : EHexNodeType::Combat_Normal;
	CurrentBattleResources.Empty();

	// --- Generate grid ---
	ABattleMapManager* MapManager = GetWorld()->SpawnActor<ABattleMapManager>();
	const FName PendingLevelID = HexMap ? HexMap->ConsumePendingBattleID() : NAME_None;
	if (MapManager && !PendingLevelID.IsNone())
	{
		MapManager->GenerateGrid(PendingLevelID);
	}

	// --- Camera setup ---
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

	// --- Emergency timer ---
	if (CurrentBattleType == EHexNodeType::Combat_Emergency)
	{
		GetWorldTimerManager().SetTimer(
			EmergencyTimerHandle,
			this, &ABattleGameMode::OnEmergencyTimeUp,
			EmergencyTimeLimit, false);
	}
}

void ABattleGameMode::OnEmergencyTimeUp()
{
	ConcludeBattle();
}

void ABattleGameMode::AddBattleResource(EResourceType Type, int32 Amount)
{
	CurrentBattleResources.FindOrAdd(Type) += Amount;
	if (CurrentBattleResources[Type] <= 0)
	{
		CurrentBattleResources.Remove(Type);
	}
}

void ABattleGameMode::ConcludeBattle()
{
	UHexMapSubsystem* HexMap = GetGameInstance()->GetSubsystem<UHexMapSubsystem>();
	if (!HexMap) return;

	GetWorldTimerManager().ClearTimer(EmergencyTimerHandle);

	// --- 1. Resource settlement: ban → emergency ×2 → modifier → save back ---
	for (TPair<EResourceType, int32>& Pair : CurrentBattleResources)
	{
		int32 FinalAmount = Pair.Value;

		// 1a. Check if this resource type is banned by any active modifier
		bool bBanned = false;
		for (URunModifierBase* Mod : HexMap->ActiveModifiers)
		{
			if (IsValid(Mod) && Mod->QueryBanResourceAcquisition(Pair.Key))
			{
				bBanned = true;
				break;
			}
		}
		if (bBanned)
		{
			FinalAmount = 0;
		}
		else
		{
			// 1b. Emergency combat default ×2
			if (CurrentBattleType == EHexNodeType::Combat_Emergency)
			{
				FinalAmount *= 2;
			}

			// 1c. Modifier multiplier chain 
			for (URunModifierBase* Mod : HexMap->ActiveModifiers)
			{
				if (IsValid(Mod))
				{
					FinalAmount = Mod->QueryResourceGainMultiplier(FinalAmount);
				}
			}
		}

		// 1d. Save multiplied amount back for SettlementWidget
		Pair.Value = FinalAmount;

		if (FinalAmount > 0)
		{
			HexMap->AddHexMapResource(Pair.Key, FinalAmount);
		}
	}

	// --- 2. Victory/failure check (emergency only) — AP deduction DEFERRED to SettlementWidget ---
	bool bIsVictory = true;
	if (CurrentBattleType == EHexNodeType::Combat_Emergency)
	{
		TArray<AActor*> Cells;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AResourceCell::StaticClass(), Cells);
		for (AActor* Actor : Cells)
		{
			if (AResourceCell* Cell = Cast<AResourceCell>(Actor))
			{
				if (Cell->DropAmount > 0)
				{
					bIsVictory = false;
					break;
				}
			}
		}
	}

	// --- 3. Show settlement widget (reads multiplied CurrentBattleResources) ---
	if (SettlementWidgetClass)
	{
		USettlementWidget* Widget = CreateWidget<USettlementWidget>(GetWorld(), SettlementWidgetClass);
		if (Widget)
		{
			Widget->Init(CurrentBattleType, CurrentBattleResources, bIsVictory);
			Widget->AddToViewport();
		}
	}

	CurrentBattleResources.Empty();
}
