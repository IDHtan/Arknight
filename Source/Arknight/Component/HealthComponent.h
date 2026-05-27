// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../DS.h"
#include "HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthZeroSignature, AActor*, Killer);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ARKNIGHT_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MaxHealth;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float CurrentHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float Defense;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float ArtsResistance;

public:
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHealthZeroSignature OnDeathEvent;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ApplyDamage(float BaseDamage, EDamageType DamageType, AActor* Instigator);

	UFUNCTION(BlueprintCallable, Category = "Stats")
	void Heal(float HealAmount);

	UHealthComponent();

	virtual void BeginPlay() override;

		
};
