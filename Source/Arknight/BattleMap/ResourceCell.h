// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../BattleMap/BattleCell.h"
#include "../Interface/TargetableInterface.h"
#include "ResourceCell.generated.h"

class UHealthComponent;

/**
 * 
 */
UCLASS()
class ARKNIGHT_API AResourceCell : public ABattleCell, public ITargetableInterface
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UHealthComponent* HealthComp;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Resource")
	EResourceType ResourceType;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Resource")
	int32 DropAmount = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Resource")
	TSubclassOf<ABattleCell> DegradeToCellClass;

public:
	AResourceCell();

	void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Resource")
	void OnCellDepleted(AActor* Killer);

	FIntVector2 GetGridPosition_Implementation() const override;

	bool CanBeTargetedBy_Implementation(AActor* Instigator) const override;
	
	void TakeCombatDamage_Implementation(float DamageAmount, EDamageType DamageType, AActor* DamageInstigator) override;
};
