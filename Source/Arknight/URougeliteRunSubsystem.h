// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "DS.h"
#include "RougeliteSaveGame.h"
#include "RougeliteSettings.h"
#include "URougeliteRunSubsystem.generated.h"

class UHexMapSubsystem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMoneyChangedSignature, int32, NewAmount);

/**
 * 
 */
UCLASS()
class ARKNIGHT_API URougeliteRunSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UPROPERTY()
	URougeliteSaveGame* CurrentSaveGame;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "System|SaveLoad")
	FString SaveSlotName = TEXT("MainSaveSlot");

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Roster")
	TArray<FOperatorRosterData> GlobalRoster;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MetaProgression")
	TMap<EResourceType, int32> GlobalResource;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "System|Subsystems")
	UHexMapSubsystem* MapManager = nullptr;

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category = "System|SaveLoad")
	void LoadGame();

	UFUNCTION(BlueprintCallable, Category = "System|SaveLoad")
	void SaveGame();

	UFUNCTION(BlueprintCallable, Category = "System|SaveLoad")
	void CheckOperators();

	UFUNCTION(BlueprintCallable, Category = "MetaProgression")
	void UpgradeOperator(FName OperatorName);

	// Add a new operator to GlobalRoster (used by 人才引进 event)
	UFUNCTION(BlueprintCallable, Category = "MetaProgression")
	void AddOperator(FName OperatorName);

public:
	UFUNCTION(BlueprintCallable, Category = "RunLogic")
	void AddResource(EResourceType Type, int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "RunLogic")
	void AddGameResource(EResourceType Type, int32 Amount);

	// --- BaseBuilding: resource operations ---

	// Broadcast whenever Money amount changes (deposit or withdrawal)
	UPROPERTY(BlueprintAssignable, Category = "BaseBuilding|Events")
	FOnMoneyChangedSignature OnMoneyChanged;

	// Check if enough global resource, deduct if yes. Returns false on insufficient funds.
	UFUNCTION(BlueprintCallable, Category = "BaseBuilding")
	bool TryConsumeResource(EResourceType Type, int32 Amount);

	// Read global resource amount from CurrentSaveGame
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BaseBuilding")
	int32 GetGlobalResourceAmount(EResourceType Type) const;

	// Set global resource to an absolute value (e.g. AP before starting a run)
	UFUNCTION(BlueprintCallable, Category = "BaseBuilding")
	void SetGlobalResourceAmount(EResourceType Type, int32 Amount);

	// --- Exchange stock ---

	// Initialise stock from DT_ExchangeItem on first load
	void InitExchangeStock();

	// Read remaining stock for a resource type
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BaseBuilding")
	int32 GetExchangeStock(EResourceType Type) const;

	// Deduct stock after a purchase
	void ReduceExchangeStock(EResourceType Type, int32 Amount);

	// --- Facility unlock ---

	UFUNCTION(BlueprintCallable, Category = "BaseBuilding")
	void UnlockTradeStation();

	UFUNCTION(BlueprintCallable, Category = "BaseBuilding")
	void UnlockExchange();
};
