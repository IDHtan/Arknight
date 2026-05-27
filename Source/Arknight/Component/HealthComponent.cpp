// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"


UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	CurrentHealth = MaxHealth;
}


void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();


}

void UHealthComponent::ApplyDamage(float BaseDamage, EDamageType DamageType, AActor* Instigator)
{
	if(BaseDamage <= 0.f || CurrentHealth <= 0.f)
	{
		return;
	}
	float FinalDamage = BaseDamage;
	switch (DamageType)
	{
	case EDamageType::Physical:
		FinalDamage = FMath::Max(BaseDamage - Defense, BaseDamage * 0.01f);
		break;
	case EDamageType::Arts:
		FinalDamage = FMath::Max((100.f - ArtsResistance) / 100 * BaseDamage, BaseDamage * 0.01f);
		break;
	default:
		break;
	}
	CurrentHealth -= FinalDamage;
	if (CurrentHealth <= 0.0f)
	{
		CurrentHealth = 0.0f;
		OnDeathEvent.Broadcast(Instigator);
	}
}

void UHealthComponent::Heal(float HealAmount)
{
	if (CurrentHealth + HealAmount > MaxHealth)
	{
		CurrentHealth = MaxHealth;
	}
	else
	{
		CurrentHealth += HealAmount;
	}
}

