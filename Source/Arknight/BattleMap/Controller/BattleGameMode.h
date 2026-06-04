// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "../../DS.h"
#include "BattleGameMode.generated.h"

class USettlementWidget;

/**
 * BattleMap entry point and data hub for combat-level state.
 *
 * Responsibilities moved here from UHexMapSubsystem:
 *   - CurrentBattleResources (battle-level lifecycle)
 *   - ConcludeBattle() (resource settlement, victory/failure, timer expiry)
 */
UCLASS()
class ARKNIGHT_API ABattleGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ABattleGameMode();

	virtual void BeginPlay() override;

	// --- Combat type & emergency timer ---

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle|Flow")
	EHexNodeType CurrentBattleType;

	UPROPERTY(EditDefaultsOnly, Category = "Battle|Timer")
	float EmergencyTimeLimit = 300.0f;  // seconds, tunable in editor

	UPROPERTY()
	FTimerHandle EmergencyTimerHandle;

	UFUNCTION()
	void OnEmergencyTimeUp();

	// --- Battle-level resource tracking ---

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle|Economy")
	TMap<EResourceType, int32> CurrentBattleResources;

	UFUNCTION(BlueprintCallable, Category = "Battle|Economy")
	void AddBattleResource(EResourceType Type, int32 Amount);

	// --- Settlement ---

	UPROPERTY(EditDefaultsOnly, Category = "Battle|UI")
	TSubclassOf<USettlementWidget> SettlementWidgetClass;

	UFUNCTION(BlueprintCallable, Category = "Battle|Flow")
	void ConcludeBattle();
};
