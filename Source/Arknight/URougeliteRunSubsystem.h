// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "DS.h"
#include "RougeliteSaveGame.h"
#include "RougeliteSettings.h"
#include "URougeliteRunSubsystem.generated.h"

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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RunData|Flow")
	FName PendingBattleLevelID;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RunData|Economy")
	int32 CurrentAP;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Roster")
	TArray<FOperatorRosterData> GlobalRoster;

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

public:
	UFUNCTION(BlueprintCallable, Category = "RunLogic")
	void PrepareForBattle(FName LevelID);

	UFUNCTION(BlueprintCallable, Category = "RunLogic")
	FName ConsumePendingBattleID();

	UFUNCTION(BlueprintCallable, Category = "RunLogic")
	void AddResource(EResourceType Type, int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "RunLogic")
	void ConcludeBattle();
};
