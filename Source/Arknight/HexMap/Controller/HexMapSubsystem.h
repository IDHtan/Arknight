// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "../../DS.h"
#include "HexMapSubsystem.generated.h"

class URunModifierBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAPChangedSignature, int32, NewAP);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHexNodeStatesRefreshedSignature);

/**
 * 
 */
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
#pragma endregion

#pragma region DataRecording
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RunData|Economy")
	int32 CurrentHexMapConsumedAP = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RunData|Economy")
	int32 CurrentGameConsumedAP = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RunData|Economy")
	TMap<EResourceType, int32> CurrentBattleResources;

	//only used to count how much resource gained instead of lost, 
	//for end-of-run summary. Actual balance is tracked in CurrentGameResources.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RunData|Economy")
	TMap<EResourceType, int32> CurrentHexMapResources; 

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RunData|Economy")
	TMap<EResourceType, int32> CurrentGameResources;
#pragma endregion

public:
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

	UFUNCTION(BlueprintCallable, Category = "HexMap|Flow")
	FHexNodeTriggerResult TriggerNodeContent(FIntVector2 TargetCoord);

	UFUNCTION(BlueprintCallable, Category = "RunLogic")
	void PrepareForBattle(FName LevelID);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "RunLogic")
	FHexEventConfig PrepareForEvent(FName EventID) const;

	UFUNCTION(BlueprintCallable, Category = "RunLogic")
	FName ConsumePendingBattleID();

	UFUNCTION(BlueprintCallable, Category = "RunLogic")
	void ConcludeBattle();

	UFUNCTION(BlueprintCallable, Category = "RunLogic")
	void ConcludeGame();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "RunLogic")
	int32 GetCurrentAP() const;

	UFUNCTION(BlueprintCallable, Category = "RunLogic")
	void ChangeCurrentAP(int32 APDelta);

	UFUNCTION(BlueprintCallable, Category = "RunLogic")
	void AddBattleResource(EResourceType Type, int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "RunLogic")
	void AddHexMapResource(EResourceType Type, int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "RunLogic")
	void AddGameResource(EResourceType Type, int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "HexMap|Modifier")
	void AddModifierInstance(URunModifierBase* NewModifier);

	UFUNCTION(BlueprintCallable, Category = "HexMap|Modifier")
	void ProcessRegionEndModifiers();

private:
	TArray<FName> BuildBattleCandidateIDs(EHexNodeType NodeType) const;

	TArray<FName> BuildEventCandidateIDs(EHexNodeType NodeType) const;

	FName PickContentIDFromCandidates(const TArray<FName>& CandidateIDs, int32 Seed, FName LastPickedID) const;
};
