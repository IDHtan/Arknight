// Fill out your copyright notice in the Description page of Project Settings.


#include "OperatorBase.h"
#include "BulletBase.h"
#include "Components/WidgetComponent.h"
#include "Components/Image.h"
#include "Blueprint/UserWidget.h"
#include "Components/BoxComponent.h"
#include "../BattleMap/Cell/DeployableCell.h"
#include "../HexMap/Controller/HexMapSubsystem.h"
#include "../HexMap/Event/RunModifierBase.h"

AOperatorBase::AOperatorBase()
{
 	
	SpriteWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("SpriteWidget"));
	SpriteWidget->SetWidgetSpace(EWidgetSpace::Screen);
	RootComponent = SpriteWidget;
	TargetingComp = CreateDefaultSubobject<UTargetingComponent>(TEXT("TargetingComponent"));

	// Invisible click target at cell surface — no VisualPlacementOffsetZ
	ClickCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("ClickCollider"));
	ClickCollider->SetupAttachment(RootComponent);
	ClickCollider->SetHiddenInGame(true);
	ClickCollider->SetVisibility(false);
	ClickCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ClickCollider->SetCollisionResponseToAllChannels(ECR_Ignore);
	ClickCollider->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
}

void AOperatorBase::OnDeployed(FIntVector2 Location, EDeploymentDirection Direction, int32 InitialLevel, ADeployableCell* DeployCell)
{
	GridLocation = Location;
	FacingDirection = Direction;
	if (IsValid(DeployCell))
	{
		FVector CellBoundsOrigin;
		FVector CellBoundsExtent;
		DeployCell->GetActorBounds(true, CellBoundsOrigin, CellBoundsExtent);

		FVector OperatorBoundsOrigin;
		FVector OperatorBoundsExtent;
		GetActorBounds(true, OperatorBoundsOrigin, OperatorBoundsExtent);

		const float CellTopZ = CellBoundsOrigin.Z + CellBoundsExtent.Z;
		const float OperatorBottomZ = OperatorBoundsOrigin.Z - OperatorBoundsExtent.Z;
		const float PlacementOffsetZ = CellTopZ - OperatorBottomZ + VisualPlacementOffsetZ;
		AddActorWorldOffset(FVector(0.0f, 0.0f, PlacementOffsetZ));

			// ClickCollider stays at cell surface (no VisualPlacementOffsetZ)
			if (ClickCollider)
			{
				ClickCollider->SetRelativeLocation(FVector(0, 0, -PlacementOffsetZ));
			}
	}

	if (SpriteWidget && SpriteWidgetClass && AvatarImage)
	{
		SpriteWidget->SetWidgetClass(SpriteWidgetClass);
		UUserWidget* Widget = SpriteWidget->GetWidget();
		if (Widget)
		{
			if (UImage* Portrait = Cast<UImage>(Widget->GetWidgetFromName(TEXT("PortraitImage"))))
			{
				Portrait->SetBrushFromTexture(AvatarImage);
			}

			// Mirror horizontally when facing Left or Down
			const float FlipX = (Direction == EDeploymentDirection::Left || Direction == EDeploymentDirection::Down) ? -1.0f : 1.0f;
			SpriteWidget->SetWorldScale3D(FVector(FlipX, 1.0f, 1.0f));
		}
	}

	if (TargetingComp)
	{
		TargetingComp->AttackPower = TargetingComp->AttackPower * pow(1.25f, InitialLevel - 1);

		// Apply active attack modifiers (e.g. 人潮人海 +90)
		if (UGameInstance* GI = GetGameInstance())
		{
			if (UHexMapSubsystem* HexMap = GI->GetSubsystem<UHexMapSubsystem>())
			{
				float FinalAttack = TargetingComp->AttackPower;
				for (URunModifierBase* Mod : HexMap->ActiveModifiers)
				{
					if (IsValid(Mod) && Mod->IsActive())
					{
						FinalAttack = Mod->QueryGlobalAttackFlatBonus(FinalAttack);
					}
				}
				TargetingComp->AttackPower = FinalAttack;
			}
		}

		TargetingComp->InitializeAbsoluteRange(GridLocation, FacingDirection);
	}

	//---
	//Deploy Animation here
	//----

	if (TargetingComp)
	{
		GetWorld()->GetTimerManager().SetTimer(TargetingTimerHandle, this, &AOperatorBase::OnTargetingTimerTick, TargetingComp->TargetingInterval, true);
		UE_LOG(LogTemp, Log, TEXT("Operator start targeting"));
	}
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

