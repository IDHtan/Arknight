// Fill out your copyright notice in the Description page of Project Settings.


#include "../Operator/BulletBase.h"

// Sets default values
ABulletBase::ABulletBase()
{
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Bullet"));
	RootComponent = MeshComp;
}

void ABulletBase::InitializeBullet(
	FVector InSelfCoord,
	FVector InTargetCoord,
	float InDamageAmount,
	EDamageType InDamageType,
	TScriptInterface<ITargetableInterface> InTarget,
	AActor* InInstigator)
{
	SelfCoord = InSelfCoord;
	TargetCoord = InTargetCoord;
	DamageAmount = InDamageAmount;
	DamageType = InDamageType;
	BulletOwner = InInstigator;
	BulletTarget = InTarget;

	Direction = (TargetCoord - SelfCoord).GetSafeNormal() * FlySpeed;

	SetLifeSpan(7.f);
}

void ABulletBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AddActorWorldOffset(Direction * DeltaTime);

	if((GetActorLocation() - TargetCoord).Size() <= FlySpeed * DeltaTime)
	{
		if (BulletTarget && BulletTarget.GetObject())
		{
			ITargetableInterface::Execute_TakeCombatDamage(BulletTarget.GetObject(), DamageAmount, DamageType, BulletOwner);
		}
		Destroy();
	}
}

