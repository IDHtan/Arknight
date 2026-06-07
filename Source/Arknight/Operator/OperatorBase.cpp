// Fill out your copyright notice in the Description page of Project Settings.


#include "OperatorBase.h"
#include "BulletBase.h"
#include "SpritesheetAnimComponent.h"
// [DEPRECATED] Widget-based sprite rendering replaced by Sprite Sheet + Material
// #include "Components/WidgetComponent.h"
// #include "Components/Image.h"
// #include "Blueprint/UserWidget.h"
#include "Components/StaticMeshComponent.h"
#include "../BattleMap/Cell/DeployableCell.h"
#include "../HexMap/Controller/HexMapSubsystem.h"
#include "../HexMap/Event/RunModifierBase.h"
// [DEPRECATED] Camera-facing removed — rotation now fixed via SpriteRotation param
// #include "Camera/PlayerCameraManager.h"
// #include "GameFramework/PlayerController.h"

AOperatorBase::AOperatorBase()
{

	// [DEPRECATED] Widget-based sprite — replaced by SpritePlane + SpriteMaterial (Sprite Sheet)
	// SpriteWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("SpriteWidget"));
	// SpriteWidget->SetWidgetSpace(EWidgetSpace::Screen);
	// RootComponent = SpriteWidget;

	// Sprite Sheet 视觉: Plane 网格体作为根组件
	SpritePlane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SpritePlane"));
	RootComponent = SpritePlane;
	// Plane 旋转由 SpriteRotation 参数控制
	// Plane 自身处理点击碰撞检测（替代旧 ClickCollider）
	SpritePlane->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SpritePlane->SetCollisionResponseToAllChannels(ECR_Ignore);
	SpritePlane->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	// Sprite Sheet 动画状态机
	AnimComp = CreateDefaultSubobject<USpriteSheetAnimComponent>(TEXT("AnimComp"));

	TargetingComp = CreateDefaultSubobject<UTargetingComponent>(TEXT("TargetingComponent"));
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
	}

	// [DEPRECATED] Widget-based portrait rendering — replaced by Sprite Sheet material animation
	/*
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
	*/

	// --- Sprite Sheet 初始化 ---

	// 1. Plane 缩放 + 旋转
	if (SpritePlane)
	{
		SpritePlane->SetWorldScale3D(FVector(SpriteVisualScale));
		SpritePlane->SetWorldRotation(SpriteRotation);
	}

	// 2. FlipU — 只有 Left 朝向做 UV 水平反转
	if (AnimComp)
	{
		AnimComp->SetFlipU(Direction == EDeploymentDirection::Left);
	}

	// 3. 播放部署动画（如果有 Start Clip），否则直接进入待机
	if (AnimComp)
	{
		FName StartClip = GetFacingClipName("Start");
		if (AnimComp->Clips.Contains(StartClip))
		{
			AnimComp->PlayAnimation(StartClip);
		}
		else
		{
			AnimComp->PlayAnimation(GetFacingClipName("Idle"));
		}
	}

	// --- 战斗初始化 ---
	if (TargetingComp)
	{
		TargetingComp->AttackPower = TargetingComp->AttackPower * pow(1.25f, InitialLevel - 1);

		// Apply active attack modifiers
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
			// 开始攻击: 切攻击动画 + 启动攻击计时器
			if (AnimComp)
			{
				AnimComp->SetAttacking(true);
				AnimComp->PlayAnimation(GetFacingClipName("Attack"));
			}
			ExecuteAttack();
			GetWorld()->GetTimerManager().SetTimer(AttackingTimerHandle, this, &AOperatorBase::ExecuteAttack, TargetingComp->AttackSpeed, true);
		}
	}
	else
	{
		if (GetWorld()->GetTimerManager().IsTimerActive(AttackingTimerHandle))
		{
			// 停止攻击: 切回待机动画
			GetWorld()->GetTimerManager().ClearTimer(AttackingTimerHandle);
			if (AnimComp)
			{
				AnimComp->SetAttacking(false);
				AnimComp->PlayAnimation(GetFacingClipName("Idle"));
			}
		}
	}
}

void AOperatorBase::OnRetreat()
{
	GetWorld()->GetTimerManager().ClearTimer(TargetingTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(AttackingTimerHandle);

	Destroy();
}

void AOperatorBase::ExecuteAttack()
{
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

FName AOperatorBase::GetFacingClipName(const FString& BaseAnim) const
{
	// Up = 背面, 其余 = 正面
	bool bFront = (FacingDirection != EDeploymentDirection::Up);
	FString Suffix = bFront ? TEXT("_Front") : TEXT("_Back");
	return FName(BaseAnim + Suffix);
}
