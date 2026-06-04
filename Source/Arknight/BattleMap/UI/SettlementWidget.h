// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../../DS.h"
#include "SettlementWidget.generated.h"

class UButton;
class UIcon;
class UTextBlock;
class UWrapBox;

/**
 * End-of-battle settlement panel.
 *
 * Displays resources gained and (for emergency combat) victory/failure text.
 * The confirm button handles AP deduction and determines the next level:
 *   Emergency failure + AP >  3 → -3 AP → OpenLevel("HexMap")
 *   Emergency failure + AP <= 3 → -3 AP → ConcludeGame() (AP ≤ 0)
 *   Normal / victory            → OpenLevel("HexMap")
 */
UCLASS()
class ARKNIGHT_API USettlementWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UWrapBox* IconsContainer;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* VictoryText;

	UPROPERTY(meta = (BindWidget))
	UButton* ConfirmButton;

	UPROPERTY(EditDefaultsOnly, Category = "Settlement")
	TSubclassOf<UIcon> IconWidgetClass;

	// Called by ABattleGameMode::ConcludeBattle()
	void Init(EHexNodeType BattleType,
	          const TMap<EResourceType, int32>& Resources,
	          bool bIsVictory);

	UFUNCTION()
	void OnConfirmClicked();

private:
	EHexNodeType CachedBattleType;
	bool bCachedIsVictory;
};
