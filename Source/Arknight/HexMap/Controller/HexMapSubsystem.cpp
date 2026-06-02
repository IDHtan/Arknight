// Fill out your copyright notice in the Description page of Project Settings.


#include "HexMapSubsystem.h"
#include "RougeliteRunSubsystem.h"
#include "../Event/RunModifierBase.h"
#include "../../RougeliteSettings.h"
#include "Engine/DataTable.h"
#define HexRegionSize 4

namespace
{
	bool IsInsideRegionBounds(const FIntVector2& Coord)
	{
		return Coord.X >= 0 && Coord.X < HexRegionSize && Coord.Y >= 0 && Coord.Y < HexRegionSize;
	}
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
	CurrentAP = 0;
	CurrentHexMapConsumedAP = 0;
	CurrentGameConsumedAP = 0;
	CurrentBattleResources.Empty();
	CurrentHexMapResources.Empty();
	CurrentGameResources.Empty();

	GenerateRegionNodes(EHexRegionType::Wood);
	GenerateRegionNodes(EHexRegionType::Rock);
	GenerateRegionNodes(EHexRegionType::Metal);
	GenerateRegionNodes(EHexRegionType::Grain);
	EnterRegion(CurrentRegion);
}

void UHexMapSubsystem::EnterRegion(EHexRegionType Region)
{
	ProcessRegionEndModifiers();
	
	CurrentRegion = Region;
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
		UE_LOG(LogTemp, Error, TEXT("HexMapSubsystem::EnterRegion: Region %s not found in AllRegionsData"), *Region.ToString());
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

	static const TArray<FIntVector2> HexDirections={
		FIntVector2(1, 0),
		FIntVector2(0, 1),
		FIntVector2(-1, 1),
		FIntVector2(1, -1)
	};

	bool bNoMovementCost = false;
	for (URunModifierBase* Modifier : ActiveModifiers)
	{
		if (IsValid(Modifier) && Modifier->QueryNoMovementCost())
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
			else if(Direction==FIntVector2(1,0)||Direction==FIntVector2(0,1))
			{
				if(CurrentAP>=1)
					ReachableCoords.Add(Candidate);
			}
			else
			{
				if(CurrentAP>=3)
					ReachableCoords.Add(Candidate);
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
		if (IsValid(Modifier) && Modifier->QueryNoMovementCost())
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
	RefreshAllNodeStates();

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

	if (FHexNodeData* EntranceNode = RegionData->Nodes.Find(FIntVector2(0, 0)))
	{
		EntranceNode->NodeState = EHexNodeState::Cleared;
	}

	if (FHexNodeData* CurrentNode = RegionData->Nodes.Find(CurrentCoordinate))
	{
		CurrentNode->NodeState = EHexNodeState::Cleared;
	}

	if (FHexNodeData* TeleportNode = RegionData->Nodes.Find(FIntVector2(3, 3)))
	{
		TeleportNode->NodeState = EHexNodeState::Unmasked;
	}

	for (TPair<FIntVector2, FHexNodeData>& Pair : RegionData->Nodes)
	{
		if (ReachableNeighbors.Contains(Pair.Key))
		{
			Pair.Value.NodeState = EHexNodeState::Reachable;
		}
		else
		{
			Pair.Value.NodeState = Pair.Value.NodeState == EHexNodeState::Reachable
				? EHexNodeState::Masked
				: Pair.Value.NodeState;
		}
	}

	OnHexNodeStatesRefreshed.Broadcast();
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

		if (Row->TargetRegion != CurrentRegion)
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

void UHexMapSubsystem::PrepareForBattle(FName LevelID)
{
	if (LevelID.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("PrepareForBattle failed: LevelID is None"));
		return;
	}
	CurrentBattleResources.Empty();
	PendingBattleLevelID = LevelID;
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
	//unfinished
	UE_LOG(LogTemp, Log, TEXT("HexMapSubsystem::ConcludeBattle is not implemented yet."));
}

void UHexMapSubsystem::ConcludeGame()
{
	//unfinished
	UE_LOG(LogTemp, Log, TEXT("HexMapSubsystem::ConcludeGame is not implemented yet."));
}

void UHexMapSubsystem::ChangeCurrentAP(int32 APDelta)
{
	CurrentAP += APDelta;
	if (APDelta < 0)
	{
		const int32 ConsumedAP = FMath::Abs(APDelta);
		CurrentHexMapConsumedAP += ConsumedAP;
		CurrentGameConsumedAP += ConsumedAP;
	}

	OnAPChanged.Broadcast(CurrentAP);

	if(CurrentAP<=0)
	{
		CurrentAP=0;
		ConcludeGame();
	}
}

void UHexMapSubsystem::AddBattleResource(EResourceType Type, int32 Amount)
{
	CurrentBattleResources.FindOrAdd(Type) += Amount;
	if(CurrentBattleResources[Type]<=0)
	{
		CurrentBattleResources.Remove(Type);
	}
}

void UHexMapSubsystem::AddHexMapResource(EResourceType Type, int32 Amount)
{
	CurrentHexMapResources.FindOrAdd(Type) += Amount;
	if(CurrentHexMapResources[Type]<=0)
	{
		CurrentHexMapResources.Remove(Type);
	}
	AddGameResource(Type, Amount);
}

void UHexMapSubsystem::AddGameResource(EResourceType Type, int32 Amount)
{
	CurrentGameResources.FindOrAdd(Type) += Amount;
	if(CurrentGameResources[Type]<=0)
	{
		CurrentGameResources.Remove(Type);
	}

	if(URougeliteRunSubsystem* RunSubsystem=GetGameInstance()->GetSubsystem<URougeliteRunSubsystem>())
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
	// Modifier execution rules are deferred until the modifier system is implemented.
	UE_LOG(LogTemp, Log, TEXT("HexMapSubsystem::ProcessRegionEndModifiers is currently a placeholder."));
}
