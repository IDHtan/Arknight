// Fill out your copyright notice in the Description page of Project Settings.


#include "HexMapPlayerController.h"

#include "HexMapSubsystem.h"
#include "../Hex/HexMapHUDWidget.h"
#include "Kismet/GameplayStatics.h"

void AHexMapPlayerController::BeginPlay()
{
	Super::BeginPlay();

	bShowMouseCursor = true;

	HexMapSubsystemP = GetGameInstance() ? GetGameInstance()->GetSubsystem<UHexMapSubsystem>() : nullptr;
	if (!HexMapSubsystemP)
	{
		UE_LOG(LogTemp, Warning, TEXT("HexMapPlayerController failed to cache UHexMapSubsystem."));
	}

	if (MapWidgetClass)
	{
		MapWidgetInstance = CreateWidget<UHexMapHUDWidget>(this, MapWidgetClass);
		if (MapWidgetInstance)
		{
			MapWidgetInstance->AddToViewport();
			FInputModeUIOnly InputMode;
			InputMode.SetWidgetToFocus(MapWidgetInstance->TakeWidget());
			SetInputMode(InputMode);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("HexMapPlayerController has no MapWidgetClass assigned."));
	}	
}

void AHexMapPlayerController::HandleNodeClicked(FIntVector2 Coordinate)
{
	if (!HexMapSubsystemP)
	{
		UE_LOG(LogTemp, Warning, TEXT("HandleNodeClicked failed: HexMapSubsystem is null."));
		return;
	}

	if (!MapWidgetInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("HandleNodeClicked failed: MapWidgetInstance is null."));
		return;
	}

	if (!HexMapSubsystemP->TryMoveToNode(Coordinate))
	{
		UE_LOG(LogTemp, Log, TEXT("HandleNodeClicked ignored: node (%d, %d) is not reachable."), Coordinate.X, Coordinate.Y);
		return;
	}

	const FHexNodeTriggerResult TriggerResult = HexMapSubsystemP->TriggerNodeContent(Coordinate);

	switch (TriggerResult.NodeType)
	{
	case EHexNodeType::Combat_Normal:
	case EHexNodeType::Combat_Emergency:
		HexMapSubsystemP->PrepareForBattle(TriggerResult.ContentID, TriggerResult.NodeType);
		UGameplayStatics::OpenLevel(this, TEXT("Lvl_BattleMap"));
		break;
	case EHexNodeType::Event:
	case EHexNodeType::Reward:
		MapWidgetInstance->OpenEventPanel(HexMapSubsystemP->PrepareForEvent(TriggerResult.ContentID));
		break;
	case EHexNodeType::Shop:
		MapWidgetInstance->OpenShopPanel();
		break;
	case EHexNodeType::Teleport:
		MapWidgetInstance->OpenTeleportPanel();
		break;
	default:
		break;
	}
}
