// Fill out your copyright notice in the Description page of Project Settings.


#include "OperatorBase.h"
#include "BulletBase.h"
#include "Components/WidgetComponent.h"

AOperatorBase::AOperatorBase()
{
 	
	PrimaryActorTick.bCanEverTick = false;

	SpriteComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SpriteComponent"));
	TargetingComp = CreateDefaultSubobject<UTargetingComponent>(TEXT("TargetingComponent"));
	ActionMenuComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("ActionMenuComp"));
	ActionMenuComp->SetupAttachment(RootComponent);
	ActionMenuComp->SetWidgetSpace(EWidgetSpace::Screen);
	ActionMenuComp->SetVisibility(false);
	ActionMenuComp->SetDrawAtDesiredSize(true);
}

void AOperatorBase::OnDeployed(FIntVector2 Location, EDeploymentDirection Direction, int32 InitialLevel)
{
	GridLocation = Location;
	FacingDirection = Direction;
	TargetingComp->AttackPower = TargetingComp->AttackPower * pow(1.25f, InitialLevel - 1);
	TargetingComp->InitializeAbsoluteRange(GridLocation, FacingDirection);

	//---
	//Deploy Animation here
	//----

	GetWorld()->GetTimerManager().SetTimer(TargetingTimerHandle, this, &AOperatorBase::OnTargetingTimerTick, TargetingComp->TargetingInterval, true);
	UE_LOG(LogTemp, Log, TEXT("Operator start targeting"));
}

void AOperatorBase::OnTargetingTimerTick()
{
	Target = TargetingComp->PerformGridScan();
	if (Target.Num() > 0)
	{
		if (!GetWorld()->GetTimerManager().IsTimerActive(AttackingTimerHandle))
		{
			ExecuteAttack();
			GetWorld()->GetTimerManager().SetTimer(AttackingTimerHandle, this, &AOperatorBase::ExecuteAttack, TargetingComp->AttackSpeed, true);
		}
	}
	else
	{
		if (GetWorld()->GetTimerManager().IsTimerActive(AttackingTimerHandle))
		{
			GetWorld()->GetTimerManager().ClearTimer(AttackingTimerHandle);
		}
	}
}

void AOperatorBase::OnRetreat()
{
	//---
	//Retreat Animation here
	//----

	GetWorld()->GetTimerManager().ClearTimer(TargetingTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(AttackingTimerHandle);

	Destroy();
}

void AOperatorBase::ExecuteAttack()
{
	//---
	//Attack Animation here
	//----
	for(const auto& Targetable : Target)
	{
		if(!IsValid(Targetable.GetObject()))
		{
			continue;
		}
		switch (AttackType) {
		case EOperatorAttackType::Melee:
			ITargetableInterface::Execute_TakeCombatDamage(Targetable.GetObject(), TargetingComp->AttackPower, TargetingComp->DamageType, this);
			break;
		case EOperatorAttackType::Ranged:
			if(BulletClass)
			{
				ABulletBase* Bullet = GetWorld()->SpawnActor<ABulletBase>(BulletClass, GetActorLocation(), FRotator::ZeroRotator);
				if (Bullet)
				{
					Bullet->InitializeBullet(
						GetActorLocation(), 
						Cast<AActor>(Targetable.GetObject())->GetActorLocation(), 
						TargetingComp->AttackPower, 
						TargetingComp->DamageType, 
						Targetable,
						this
					);
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Failed to spawn bullet for %s"), *GetName());
					ITargetableInterface::Execute_TakeCombatDamage(Targetable.GetObject(), TargetingComp->AttackPower, TargetingComp->DamageType, this);
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("BulletClass is not set for %s"), *GetName());
				ITargetableInterface::Execute_TakeCombatDamage(Targetable.GetObject(), TargetingComp->AttackPower, TargetingComp->DamageType, this);
			}
			break;
		}
		
	}
	
}

