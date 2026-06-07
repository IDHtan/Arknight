// Fill out your copyright notice in the Description page of Project Settings.


#include "HexMapSubsystem.h"
#include "../../URougeliteRunSubsystem.h"
#include "../Event/RunModifierBase.h"
#include "../../RougeliteSettings.h"
#include "Engine/DataTable.h"
#include "Kismet/GameplayStatics.h"
#define HexRegionSize 4

static const TArray<FIntVector2> HexDirections = {
	FIntVector2(1, 0),
	FIntVector2(0, 1),
	FIntVector2(-1, 1),
	FIntVector2(1, -1)
};

void UHexMapSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	//we should not call InitializeNewRun here because the umg being called in later function might be not spawned yet, and we need to bind some delegate in umg to the subsystem, which will cause crash if the subsystem is not initialized.
}


void UHexMapSubsystem::InitializeNewRun(int32 Seed)
{
	MasterSeed = Seed;
	ActiveModifiers.Empty();
	AllRegionsData.Empty();
	CurrentRegion = EHexRegionType::Wood;
	CurrentCoordinate = FIntVector2(0,0);
	PendingBattleLevelID = NAME_None;
	LastEventID = NAME_None;
	LastBattleID = NAME_None;
	CurrentHexMapConsumedAP = 0;
	CurrentGameConsumedAP = 0;
	CurrentHexMapResources.Empty();
	CurrentGameResources.Empty();

	// Copy AP from global meta-progression into the new run
	if (URougeliteRunSubsystem* Run = GetGameInstance()->GetSubsystem<URougeliteRunSubsystem>())
	{
		CurrentGameResources.Add(EResourceType::AP,
			Run->GlobalResource.FindRef(EResourceType::AP));
	}

	GenerateRegionNodes(EHexRegionType::Wood);
	GenerateRegionNodes(EHexRegionType::Rock);
	GenerateRegionNodes(EHexRegionType::Metal);
	GenerateRegionNodes(EHexRegionType::Grain);
}

void UHexMapSubsystem::EnterRegion(EHexRegionType Region)
{
	CurrentRegion = Region;
	ProcessRegionEndModifiers();
	CurrentCoordinate = FIntVector2(0,0);
	LastEventID = NAME_None;
	LastBattleID = NAME_None;
	CurrentHexMapConsumedAP = 0;
	CurrentHexMapResources.Empty();

	GenerateRegionNodes(Region);

	if (FHexRegionData* RegionData = AllRegionsData.Find(Region))
	{
		for(TPair<FIntVector2, FHexNodeData>& Pair : RegionData->Nodes)
		{
			if(Pair.Value.NodeState == EHexNodeState::Cleared)
			{
				Pair.Value.NodeState = EHexNodeState::Unmasked;
			}
		}
		RefreshAllNodeStates();
	}
	else{
		UE_LOG(LogTemp, Error, TEXT("HexMapSubsystem::EnterRegion: Region %s not found in AllRegionsData"), *UEnum::GetValueAsString(Region));
		return;
	}
}

void UHexMapSubsystem::GenerateRegionNodes(EHexRegionType Region)
{
	FHexRegionData& RegionData = AllRegionsData.FindOrAdd(Region);
	if (RegionData.bIsGenerated)
	{
		return;
	}

	RegionData.RegionType = Region;
	RegionData.Nodes.Empty();

	const TArray<EHexNodeType> DefaultPattern = {
		EHexNodeType::Entrance,
		EHexNodeType::Combat_Normal,
		EHexNodeType::Event,
		EHexNodeType::Reward,
		EHexNodeType::Combat_Normal,
		EHexNodeType::Combat_Emergency,
		EHexNodeType::Event,
		EHexNodeType::Reward,
		EHexNodeType::Combat_Normal,
		EHexNodeType::Event,
		EHexNodeType::Shop,
		EHexNodeType::Combat_Emergency,
		EHexNodeType::Reward,
		EHexNodeType::Combat_Normal,
		EHexNodeType::Event,
		EHexNodeType::Teleport
	};

	TArray<FIntVector2> MiddleCoordinates;
	MiddleCoordinates.Reserve(DefaultPattern.Num() - 2);
	for (int32 Y = 0; Y < HexRegionSize; ++Y)
	{
		for (int32 X = 0; X < HexRegionSize; ++X)
		{
			FIntVector2 Candidate(X, Y);
			if (Candidate == FIntVector2(0, 0) || Candidate == FIntVector2(3, 3))
			{
				continue;
			}
			MiddleCoordinates.Add(Candidate);
		}
	}

	FRandomStream Stream(MasterSeed + static_cast<int32>(Region));
	for (int32 Index = MiddleCoordinates.Num() - 1; Index > 0; --Index)
	{
		int32 SwapIndex = Stream.RandRange(0, Index);
		MiddleCoordinates.Swap(Index, SwapIndex);
	}

	TArray<FIntVector2> ShuffledCoordinates;
	ShuffledCoordinates.Reserve(DefaultPattern.Num());
	ShuffledCoordinates.Add(FIntVector2(0, 0));
	ShuffledCoordinates.Append(MiddleCoordinates);
	ShuffledCoordinates.Add(FIntVector2(3, 3));

	const FIntVector2 ForbiddenEmergencyCoordA(0, 1);
	const FIntVector2 ForbiddenEmergencyCoordB(1, 0);
	for (int32 PatternIndex = 1; PatternIndex < DefaultPattern.Num() - 1; ++PatternIndex)
	{
		if (DefaultPattern[PatternIndex] != EHexNodeType::Combat_Emergency)
		{
			continue;
		}

		const FIntVector2 CurrentCoord = ShuffledCoordinates[PatternIndex];
		if (CurrentCoord != ForbiddenEmergencyCoordA && CurrentCoord != ForbiddenEmergencyCoordB)
		{
			continue;
		}

		for (int32 SwapCandidateIndex = PatternIndex + 1; SwapCandidateIndex < DefaultPattern.Num() - 1; ++SwapCandidateIndex)
		{
			if (DefaultPattern[SwapCandidateIndex] == EHexNodeType::Combat_Emergency)
			{
				continue;
			}

			const FIntVector2 SwapCoord = ShuffledCoordinates[SwapCandidateIndex];
			if (SwapCoord == ForbiddenEmergencyCoordA || SwapCoord == ForbiddenEmergencyCoordB)
			{
				continue;
			}

			ShuffledCoordinates.Swap(PatternIndex, SwapCandidateIndex);
			break;
		}
	}

	for (int32 PatternIndex = 0; PatternIndex < DefaultPattern.Num(); ++PatternIndex)
	{
		FHexNodeData NodeData;
		NodeData.GridCoordinate = ShuffledCoordinates[PatternIndex];
		NodeData.NodeType = DefaultPattern[PatternIndex];
		NodeData.NodeState = EHexNodeState::Masked;
		NodeData.ContentSeed = MasterSeed + PatternIndex;

		RegionData.Nodes.Add(NodeData.GridCoordinate, NodeData);
	}

	// Teleport (exit) starts as visited — always reachable destination
	if (FHexNodeData* TeleportNode = RegionData.Nodes.Find(FIntVector2(3, 3)))
	{
		TeleportNode->bHasBeenVisited = true;
	}

	RegionData.bIsGenerated = true;
}

FHexNodeData UHexMapSubsystem::GetNodeInfo(FIntVector2 Coord) const
{
	const FHexRegionData* RegionData = AllRegionsData.Find(CurrentRegion);
	if (!RegionData)
	{
		UE_LOG(LogTemp, Error, TEXT("HexMapSubsystem::GetNodeInfo: CurrentRegion %s not found in AllRegionsData"), *UEnum::GetValueAsString(CurrentRegion));
		return FHexNodeData();
	}

	if (const FHexNodeData* NodeData = RegionData->Nodes.Find(Coord))
	{
		return *NodeData;
	}
	else{
		UE_LOG(LogTemp, Error, TEXT("HexMapSubsystem::GetNodeInfo: Node %s not found in Region %s"), *Coord.ToString(), *UEnum::GetValueAsString(CurrentRegion));
	}
	return FHexNodeData();
}

TArray<FIntVector2> UHexMapSubsystem::GetReachableNeighbors(FIntVector2 CenterCoord)
{
	TArray<FIntVector2> ReachableCoords;
	const FHexRegionData* RegionData = AllRegionsData.Find(CurrentRegion);
	if (!RegionData)
	{
		UE_LOG(LogTemp, Error, TEXT("HexMapSubsystem::GetReachableNeighbors: CurrentRegion %s not found in AllRegionsData"), *UEnum::GetValueAsString(CurrentRegion));
		return ReachableCoords;
	}

	bool bNoMovementCost = false;
	for (URunModifierBase* Modifier : ActiveModifiers)
	{
		if (IsValid(Modifier) && Modifier->IsActive() && Modifier->QueryNoMovementCost())
		{
			bNoMovementCost = true;
			break;
		}
	}

	for (const FIntVector2& Direction : HexDirections)
	{
		const FIntVector2 Candidate = CenterCoord + Direction;
		if (Candidate.X >= 0 && Candidate.X < HexRegionSize &&
			Candidate.Y >= 0 && Candidate.Y < HexRegionSize &&
			RegionData->Nodes.Contains(Candidate) &&
			RegionData->Nodes[Candidate].NodeState != EHexNodeState::Cleared
			)
		{
			if (bNoMovementCost)
			{
				ReachableCoords.Add(Candidate);
			}
			else
			{
				const int32 CurrentAP = CurrentGameResources.FindRef(EResourceType::AP);
				if (Direction == FIntVector2(1, 0) || Direction == FIntVector2(0, 1))
				{
					if (CurrentAP >= 1)
						ReachableCoords.Add(Candidate);
				}
				else
				{
					if (CurrentAP >= 3)
						ReachableCoords.Add(Candidate);
				}
			}
		}		
	}
	return ReachableCoords;
}

bool UHexMapSubsystem::TryMoveToNode(FIntVector2 TargetCoord)
{
	FHexRegionData* RegionData = AllRegionsData.Find(CurrentRegion);
	if (!RegionData)
	{
		UE_LOG(LogTemp, Error, TEXT("HexMapSubsystem::TryMoveToNode: CurrentRegion %s not found in AllRegionsData"), *UEnum::GetValueAsString(CurrentRegion));
		return false;
	}

	FHexNodeData* TargetNode = RegionData->Nodes.Find(TargetCoord);
	if (!TargetNode || TargetNode->NodeState != EHexNodeState::Reachable)
	{
		UE_LOG(LogTemp, Error, TEXT("HexMapSubsystem::TryMoveToNode: Node %s is not reachable"), *TargetCoord.ToString());
		return false;
	}

	bool bNoMovementCost = false;
	for (URunModifierBase* Modifier : ActiveModifiers)
	{
		if (IsValid(Modifier) && Modifier->IsActive() && Modifier->QueryNoMovementCost())
		{
			bNoMovementCost = true;
			break;
		}
	}

	if (!bNoMovementCost)
	{
		if(TargetCoord-CurrentCoordinate==FIntVector2(1,0)||TargetCoord-CurrentCoordinate==FIntVector2(0,1))
			ChangeCurrentAP(-1);
		else
			ChangeCurrentAP(-3);
	}

	CurrentCoordinate = TargetCoord;

	return true;
}

void UHexMapSubsystem::RefreshAllNodeStates()
{
	FHexRegionData* RegionData = AllRegionsData.Find(CurrentRegion);
	if (!RegionData)
	{
		UE_LOG(LogTemp, Error, TEXT("HexMapSubsystem::RefreshAllNodeStates: CurrentRegion %s not found in AllRegionsData"), *UEnum::GetValueAsString(CurrentRegion));
		return;
	}

	const TArray<FIntVector2> ReachableNeighbors = GetReachableNeighbors(CurrentCoordinate);

	// If AP hit 0, no nodes are reachable — end the run.
	if (CurrentGameResources.FindRef(EResourceType::AP) <= 0)
	{
		ConcludeGame();
		return;
	}

	if (FHexNodeData* EntranceNode = RegionData->Nodes.Find(FIntVector2(0, 0)))
	{
		EntranceNode->NodeState = EHexNodeState::Cleared;
		EntranceNode->bHasBeenVisited = true;
	}

	if (FHexNodeData* CurrentNode = RegionData->Nodes.Find(CurrentCoordinate))
	{
		CurrentNode->NodeState = EHexNodeState::Cleared;
		CurrentNode->bHasBeenVisited = true;
	}

	if (FHexNodeData* TeleportNode = RegionData->Nodes.Find(FIntVector2(3, 3)))
	{
		TeleportNode->NodeState = EHexNodeState::Unmasked;
	}

	// Reveal all adjacent hexes (even if unaffordable) so the player sees them
	for (const FIntVector2& Dir : HexDirections)
	{
		const FIntVector2 Neighbor = CurrentCoordinate + Dir;
		FHexNodeData* NeighborNode = RegionData->Nodes.Find(Neighbor);
		if (NeighborNode && NeighborNode->NodeState == EHexNodeState::Masked)
		{
			NeighborNode->NodeState = EHexNodeState::Unmasked;
		}
	}

	for (TPair<FIntVector2, FHexNodeData>& Pair : RegionData->Nodes)
	{
		if (ReachableNeighbors.Contains(Pair.Key))
		{
			Pair.Value.NodeState = EHexNodeState::Reachable;
		}
		else
		{
			// Previously reachable but not chosen → stay visible (Unmasked)
			Pair.Value.NodeState = Pair.Value.NodeState == EHexNodeState::Reachable
				? EHexNodeState::Unmasked
				: Pair.Value.NodeState;
		}
	}

	// --- Game-over exit: all nodes visited in all four regions ---
	bool bAllVisited = true;
	for (const TPair<EHexRegionType, FHexRegionData>& RegionPair : AllRegionsData)
	{
		for (const TPair<FIntVector2, FHexNodeData>& NodePair : RegionPair.Value.Nodes)
		{
			if (!NodePair.Value.bHasBeenVisited)
			{
				bAllVisited = false;
				break;
			}
		}
		if (!bAllVisited) break;
	}
	if (bAllVisited)
	{
		UE_LOG(LogTemp, Log, TEXT("RefreshAllNodeStates: all nodes visited in all regions — game over"));
		ConcludeGame();
		return;
	}

	OnHexNodeStatesRefreshed.Broadcast();
}

void UHexMapSubsystem::RevealAllRegionNodes()
{
	FHexRegionData* RegionData = AllRegionsData.Find(CurrentRegion);
	if (!RegionData)
	{
		UE_LOG(LogTemp, Warning, TEXT("HexMapSubsystem::RevealAllRegionNodes: CurrentRegion %s not found"), *UEnum::GetValueAsString(CurrentRegion));
		return;
	}

	for (TPair<FIntVector2, FHexNodeData>& Pair : RegionData->Nodes)
	{
		if (Pair.Value.NodeState == EHexNodeState::Masked)
		{
			Pair.Value.NodeState = EHexNodeState::Unmasked;
		}
	}

	RefreshAllNodeStates();
}

FHexNodeTriggerResult UHexMapSubsystem::TriggerNodeContent(FIntVector2 TargetCoord)
{
	FHexNodeTriggerResult Result;
	const FHexNodeData NodeData = GetNodeInfo(TargetCoord);

	switch (NodeData.NodeType)
	{
	case EHexNodeType::Combat_Normal:
	case EHexNodeType::Combat_Emergency:
		Result.NodeType = NodeData.NodeType;
		Result.ContentID = PickContentIDFromCandidates(
			BuildBattleCandidateIDs(NodeData.NodeType),
			NodeData.ContentSeed + CurrentHexMapConsumedAP + CurrentGameConsumedAP,
			LastBattleID);
		LastBattleID = Result.ContentID;
		break;
	case EHexNodeType::Event:
	case EHexNodeType::Reward:
		Result.NodeType = NodeData.NodeType;
		Result.ContentID = PickContentIDFromCandidates(
			BuildEventCandidateIDs(NodeData.NodeType),
			NodeData.ContentSeed + CurrentHexMapConsumedAP + CurrentGameConsumedAP,
			LastEventID);
		LastEventID = Result.ContentID;
		break;
	case EHexNodeType::Shop:
		Result.NodeType = EHexNodeType::Shop;
		break;
	case EHexNodeType::Teleport:
		Result.NodeType = EHexNodeType::Teleport;
		break;
	default:
		break;
	}

	return Result;
}

TArray<FName> UHexMapSubsystem::BuildBattleCandidateIDs(EHexNodeType NodeType) const
{
	TArray<FName> CandidateIDs;
	const URougeliteSettings* Settings = GetDefault<URougeliteSettings>();
	if (!Settings)
	{
		UE_LOG(LogTemp, Warning, TEXT("HexMapSubsystem::BuildBattleCandidateIDs: Settings is null."));
		return CandidateIDs;
	}

	UDataTable* LevelLayoutConfigTable = Settings->LevelLayoutTable.LoadSynchronous();
	if (!LevelLayoutConfigTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("HexMapSubsystem::BuildBattleCandidateIDs: LevelLayoutConfigTable is null."));
		return CandidateIDs;
	}

	const TArray<FName> RowNames = LevelLayoutConfigTable->GetRowNames();
	for (const FName& RowName : RowNames)
	{
		const FLevelLayoutConfig* Row = LevelLayoutConfigTable->FindRow<FLevelLayoutConfig>(RowName, TEXT("BuildBattleCandidateIDs"));
		if (!Row)
		{
			continue;
		}

		if (Row->TargetRegion != CurrentRegion)
		{
			continue;
		}

		CandidateIDs.Add(Row->LevelID.IsNone() ? RowName : Row->LevelID);
	}

	return CandidateIDs;
}

TArray<FName> UHexMapSubsystem::BuildEventCandidateIDs(EHexNodeType NodeType) const
{
	TArray<FName> CandidateIDs;
	const URougeliteSettings* Settings = GetDefault<URougeliteSettings>();
	if (!Settings)
	{
		UE_LOG(LogTemp, Warning, TEXT("HexMapSubsystem::BuildEventCandidateIDs: Settings is null."));
		return CandidateIDs;
	}

	UDataTable* EventConfigTable = Settings->EventConfigTable.LoadSynchronous();
	if (!EventConfigTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("HexMapSubsystem::BuildEventCandidateIDs: EventConfigTable is null."));
		return CandidateIDs;
	}

	const TArray<FName> RowNames = EventConfigTable->GetRowNames();
	for (const FName& RowName : RowNames)
	{
		const FHexEventConfig* Row = EventConfigTable->FindRow<FHexEventConfig>(RowName, TEXT("BuildEventCandidateIDs"));
		if (!Row)
		{
			continue;
		}

		// Events are region-specific; Rewards are shared across all regions
		if (NodeType == EHexNodeType::Event && Row->TargetRegion != CurrentRegion)
		{
			continue;
		}

		// Also filter by NodeType so Event/Reward rows don't mix
		if (Row->NodeType != NodeType)
		{
			continue;
		}

		CandidateIDs.Add(Row->EventID.IsNone() ? RowName : Row->EventID);
	}

	return CandidateIDs;
}

FName UHexMapSubsystem::PickContentIDFromCandidates(const TArray<FName>& CandidateIDs, int32 Seed, FName LastPickedID) const
{
	if (CandidateIDs.Num() == 0)
	{
		return NAME_None;
	}

	TArray<FName> FilteredCandidateIDs = CandidateIDs;
	if (LastPickedID != NAME_None && FilteredCandidateIDs.Num() > 1)
	{
		FilteredCandidateIDs.Remove(LastPickedID);
	}

	if (FilteredCandidateIDs.Num() == 0)
	{
		FilteredCandidateIDs = CandidateIDs;
	}

	FRandomStream Stream(Seed + FMath::Rand());
	const int32 PickIndex = Stream.RandRange(0, FilteredCandidateIDs.Num() - 1);
	return FilteredCandidateIDs[PickIndex];
}

FName UHexMapSubsystem::GetRandomBattleLevelID(EHexNodeType NodeType) const
{
	// --- Responsibility ---
	// Called by UHexEventLogicBase subclasses to pick a random combat level
	// from the current region's candidate pool. Used by events that trigger
	// combat without going through a normal Combat node on the hex map
	//
	// Flow:
	//   BuildBattleCandidateIDs(NodeType) → filtered candidates for CurrentRegion
	//   PickContentIDFromCandidates(candidates, seed, LastBattleID)
	//     → excludes LastBattleID to avoid immediate repeats
	//     → uses ContentSeed + FMath::Rand() for deterministic randomness
	//   Caller then: Map->PrepareForBattle(ID) + UGameplayStatics::OpenLevel
	//
	// Returns NAME_None if the candidate pool is empty.

	const TArray<FName> Candidates = BuildBattleCandidateIDs(NodeType);
	if (Candidates.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("HexMapSubsystem::GetRandomBattleLevelID: no candidates for NodeType %s in region %s"),
			*UEnum::GetValueAsString(NodeType), *UEnum::GetValueAsString(CurrentRegion));
		return NAME_None;
	}

	const int32 Seed = MasterSeed + static_cast<int32>(NodeType) + CurrentHexMapConsumedAP;
	return PickContentIDFromCandidates(Candidates, Seed, LastBattleID);
}

void UHexMapSubsystem::PrepareForBattle(FName LevelID, EHexNodeType NodeType)
{
	if (LevelID.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("PrepareForBattle failed: LevelID is None"));
		return;
	}
	PendingBattleLevelID = LevelID;
	PendingBattleType = NodeType;
}

FName UHexMapSubsystem::ConsumePendingBattleID()
{
	const FName Result = PendingBattleLevelID;
	PendingBattleLevelID = NAME_None;
	return Result;
}

FHexEventConfig UHexMapSubsystem::PrepareForEvent(FName EventID) const
{
	const URougeliteSettings* Settings = GetDefault<URougeliteSettings>();
	if (!Settings)
	{
		UE_LOG(LogTemp, Warning, TEXT("HexMapSubsystem::PrepareForEvent: Settings is null."));
		return FHexEventConfig();
	}
	
	UDataTable* EventConfigTable = Settings->EventConfigTable.LoadSynchronous();
	if (!EventConfigTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("HexMapSubsystem::PrepareForEvent: EventConfigTable is null."));
		return FHexEventConfig();
	}
	
	const FHexEventConfig* Result = EventConfigTable->FindRow<FHexEventConfig>(EventID, TEXT("PrepareForEvent"));
	if (!Result)
	{
		UE_LOG(LogTemp, Warning, TEXT("HexMapSubsystem::PrepareForEvent: EventConfigTable does not contain EventID %s"), *EventID.ToString());
		return FHexEventConfig();
	}
	
	return *Result;
}

void UHexMapSubsystem::ConcludeBattle()
{
	// Battle settlement logic moved to ABattleGameMode::ConcludeBattle().
	// See Docs/BattleMapFlow.md for the full data-flow design.
	UE_LOG(LogTemp, Log, TEXT("HexMapSubsystem::ConcludeBattle stub — real logic is in ABattleGameMode"));
}

//this function has to be checked later
void UHexMapSubsystem::ConcludeGame()
{
	// --- Flow: HexMap run ends → MainMenu ---
	// Triggered when AP reaches 0 or player forfeits the run.
	//
	// Settlement order:
	//   1. Save all persistent data via RunSubsystem
	//   2. Clean up transient run state (ActiveModifiers, AllRegionsData, etc.)
	//   3. OpenLevel("Lvl_MainMenu")
	//
	// Note: CurrentGameResources are already synced to GlobalResources
	// via AddGameResource during the run. No late flush needed.

	URougeliteRunSubsystem* RunSubsystem = GetGameInstance() ? GetGameInstance()->GetSubsystem<URougeliteRunSubsystem>() : nullptr;
	if (RunSubsystem)
	{
		RunSubsystem->SaveGame();
	}

	// Clean up transient run state
	ActiveModifiers.Empty();
	AllRegionsData.Empty();
	CurrentHexMapResources.Empty();
	CurrentGameResources.Empty();
	CurrentHexMapConsumedAP = 0;
	CurrentGameConsumedAP = 0;
	PendingBattleLevelID = NAME_None;
	LastEventID = NAME_None;
	LastBattleID = NAME_None;

	if (UWorld* World = GetWorld())
	{
		UGameplayStatics::OpenLevel(World, TEXT("/Game/Maps/Lvl_MainMenu"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("HexMapSubsystem::ConcludeGame: GetWorld() returned null, cannot open MainMenu"));
	}
}

int32 UHexMapSubsystem::GetCurrentAP() const
{
	return GetResource(EResourceType::AP);
}

int32 UHexMapSubsystem::GetResource(EResourceType Type) const
{
	return CurrentGameResources.FindRef(Type);
}

TMap<EResourceType, int32> UHexMapSubsystem::GetHexMapLost() const
{
	TMap<EResourceType, int32> Lost;
	for (const TPair<EResourceType, int32>& Pair : CurrentHexMapResources)
	{
		const int32 Balance = CurrentGameResources.FindRef(Pair.Key);
		const int32 Loss = Pair.Value - Balance;
		if (Loss > 0) Lost.Add(Pair.Key, Loss);
	}
	return Lost;
}

void UHexMapSubsystem::ChangeCurrentAP(int32 APDelta)
{
	int32& APRef = CurrentGameResources.FindOrAdd(EResourceType::AP);
	APRef += APDelta;
	if (APDelta < 0)
	{
		const int32 ConsumedAP = FMath::Abs(APDelta);
		CurrentHexMapConsumedAP += ConsumedAP;
		CurrentGameConsumedAP += ConsumedAP;
	}

	OnAPChanged.Broadcast(APRef);

	if(APRef<=0)
	{
		APRef=0;
		// ConcludeGame() is now triggered by GetReachableNeighbors()
		// when the next RefreshAllNodeStates detects AP == 0.
		// This avoids ConcludeGame firing mid-flow before node content
		// (Event/Shop/Combat OpenLevel) has been triggered.
	}
}

void UHexMapSubsystem::AddHexMapResource(EResourceType Type, int32 Amount)
{
	// Tier 2: region gain tracking (gains only)
	if (Amount > 0)
	{
		CurrentHexMapResources.FindOrAdd(Type) += Amount;
	}

	// Tier 3: run-level balance — always applied (positive or negative)
	CurrentGameResources.FindOrAdd(Type) += Amount;

	// Tier 4: auto-sync to global meta-progression
	if (URougeliteRunSubsystem* RunSubsystem = GetGameInstance()->GetSubsystem<URougeliteRunSubsystem>())
	{
		RunSubsystem->AddGameResource(Type, Amount);
	}
}

void UHexMapSubsystem::AddModifierInstance(URunModifierBase* NewModifier)
{
	if (IsValid(NewModifier))
	{
		ActiveModifiers.Add(NewModifier);
	}
}

void UHexMapSubsystem::ProcessRegionEndModifiers()
{
	// 1. Execute delayed rewards (CurrentRegion-scoped only)
	for (URunModifierBase* Mod : ActiveModifiers)
	{
		if (IsValid(Mod) && Mod->Scope == EModifierScope::CurrentRegion)
		{
			if (URougeliteRunSubsystem* RunSub = GetGameInstance()->GetSubsystem<URougeliteRunSubsystem>())
			{
				Mod->ExecuteDelayedResourceReward(RunSub);
			}
		}
	}

	// 2. Remove CurrentRegion-scoped modifiers (they expire)
	ActiveModifiers.RemoveAll([](const URunModifierBase* Mod)
	{
		return !IsValid(Mod) || Mod->Scope == EModifierScope::CurrentRegion;
	});

	// 3. Promote NextRegion → CurrentRegion
	for (URunModifierBase* Mod : ActiveModifiers)
	{
		if (IsValid(Mod) && Mod->Scope == EModifierScope::NextRegion)
		{
			Mod->Scope = EModifierScope::CurrentRegion;
		}
	}
}
