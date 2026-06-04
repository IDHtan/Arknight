// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "../../DS.h"
#include "HexMapSubsystem.generated.h"

class URunModifierBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAPChangedSignature, int32, NewAP);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHexNodeStatesRefreshedSignature);

UCLASS()
class ARKNIGHT_API UHexMapSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UPROPERTY(BlueprintAssignable, Category = "HexMap|Events")
	FOnAPChangedSignature OnAPChanged;

	UPROPERTY(BlueprintAssignable, Category = "HexMap|Events")
	FOnHexNodeStatesRefreshedSignature OnHexNodeStatesRefreshed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HexMap|Generation")
	int32 MasterSeed = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HexMap|Modifier")
	TArray<URunModifierBase*> ActiveModifiers;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HexMap|Region")
	TMap<EHexRegionType, FHexRegionData> AllRegionsData;

#pragma region RegionState
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HexMap|Region")
	EHexRegionType CurrentRegion = EHexRegionType::Wood;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HexMap|Region")
	FIntVector2 CurrentCoordinate = FIntVector2::ZeroValue;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HexMap|Flow")
	FName LastEventID = NAME_None;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HexMap|Flow")
	FName LastBattleID = NAME_None;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RunData|Flow")
	FName PendingBattleLevelID = NAME_None;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RunData|Flow")
	EHexNodeType PendingBattleType = EHexNodeType::Combat_Normal;
#pragma endregion

#pragma region DataRecording
	// AP consumed in current region (for events like 退老还童)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RunData|Economy")
	int32 CurrentHexMapConsumedAP = 0;

	// AP consumed across the entire run
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RunData|Economy")
	int32 CurrentGameConsumedAP = 0;
#pragma endregion

public:
#pragma region NodeState
	UFUNCTION(BlueprintCallable, Category = "HexMap|Run")
	void InitializeNewRun(int32 Seed);

	UFUNCTION(BlueprintCallable, Category = "HexMap|Region")
	void GenerateRegionNodes(EHexRegionType Region);

	UFUNCTION(BlueprintCallable, Category = "HexMap|Region")
	void EnterRegion(EHexRegionType Region);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "HexMap|Query")
	FHexNodeData GetNodeInfo(FIntVector2 Coord) const;

	UFUNCTION(BlueprintCallable, Category = "HexMap|Query")
	TArray<FIntVector2> GetReachableNeighbors(FIntVector2 CenterCoord);

	UFUNCTION(BlueprintCallable, Category = "HexMap|Flow")
	bool TryMoveToNode(FIntVector2 TargetCoord);

	UFUNCTION(BlueprintCallable, Category = "HexMap|Region")
	void RefreshAllNodeStates();

	UFUNCTION(BlueprintCallable, Category = "HexMap|Region")
	void RevealAllRegionNodes();
#pragma endregion

#pragma region NodeContentFlow
	UFUNCTION(BlueprintCallable, Category = "HexMap|Flow")
	FName GetRandomBattleLevelID(EHexNodeType NodeType) const;

	UFUNCTION(BlueprintCallable, Category = "HexMap|Flow")
	FHexNodeTriggerResult TriggerNodeContent(FIntVector2 TargetCoord);

	UFUNCTION(BlueprintCallable, Category = "RunLogic")
	void PrepareForBattle(FName LevelID, EHexNodeType NodeType);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "RunLogic")
	FHexEventConfig PrepareForEvent(FName EventID) const;
#pragma endregion

#pragma region InterlevelFlow
	UFUNCTION(BlueprintCallable, Category = "RunLogic")
	FName ConsumePendingBattleID();

	UFUNCTION(BlueprintCallable, Category = "RunLogic")
	void ConcludeBattle();

	UFUNCTION(BlueprintCallable, Category = "RunLogic")
	void ConcludeGame();
#pragma endregion

#pragma region Economy
	// --- Resource getters (read from CurrentGameResources) ---
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "RunLogic")
	int32 GetResource(EResourceType Type) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "RunLogic")
	int32 GetCurrentAP() const;

	// --- Resource setters (operate on CurrentGameResources + auto-sync up) ---
	// Adds to CurrentGameResources. Also propagates to GlobalResources.
	UFUNCTION(BlueprintCallable, Category = "RunLogic")
	void AddGameResource(EResourceType Type, int32 Amount);

	// Adds to CurrentHexMapResources (gains only) AND calls AddGameResource.
	UFUNCTION(BlueprintCallable, Category = "RunLogic")
	void AddHexMapResource(EResourceType Type, int32 Amount);

	// Modifies AP; tracks consumed AP in CurrentHexMapConsumedAP / CurrentGameConsumedAP
	UFUNCTION(BlueprintCallable, Category = "RunLogic")
	void ChangeCurrentAP(int32 APDelta);

	// --- Read-only view for iteration (e.g. InventoryWidget populating icons) ---
	const TMap<EResourceType, int32>& GetAllResources() const { return CurrentGameResources; }

	// --- Special: how much was lost (spent) in this region, for events like 退老还童 ---
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "RunLogic")
	TMap<EResourceType, int32> GetHexMapLost() const;
#pragma endregion

#pragma region Modifier
	UFUNCTION(BlueprintCallable, Category = "HexMap|Modifier")
	void AddModifierInstance(URunModifierBase* NewModifier);

	UFUNCTION(BlueprintCallable, Category = "HexMap|Modifier")
	void ProcessRegionEndModifiers();
#pragma endregion

private:
	// Only tracks gains (Amount > 0) — per-region stats for display
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RunData|Economy", meta = (AllowPrivateAccess = "true"))
	TMap<EResourceType, int32> CurrentHexMapResources;

	// Source of truth for all resource balances (including AP). Setter propagates to Global.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RunData|Economy", meta = (AllowPrivateAccess = "true"))
	TMap<EResourceType, int32> CurrentGameResources;

	TArray<FName> BuildBattleCandidateIDs(EHexNodeType NodeType) const;
	TArray<FName> BuildEventCandidateIDs(EHexNodeType NodeType) const;
	FName PickContentIDFromCandidates(const TArray<FName>& CandidateIDs, int32 Seed, FName LastPickedID) const;
};
