// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Component/HealthComponent.h"
#include "../Component/TargetingComponent.h"
#include "../DS.h"
#include "OperatorBase.generated.h"

class ABulletBase;
class ADeployableCell;
class USpriteSheetAnimComponent;

UCLASS()
class ARKNIGHT_API AOperatorBase : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName OperatorName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText OperatorDescription;

	// [DEPRECATED] World→screen projected portrait. Widget instance set in OnDeployed.
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	// class UWidgetComponent* SpriteWidget;

	// [DEPRECATED] UMG class containing a UImage named "PortraitImage". Set in BP.
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	// TSubclassOf<UUserWidget> SpriteWidgetClass;

	// Sprite Sheet 视觉载体: Plane 网格体，StaticMesh 在蓝图中设为 /Engine/BasicShapes/Plane
	// Plane 自身处理点击碰撞，替代旧 ClickCollider
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual")
	UStaticMeshComponent* SpritePlane;

	// Sprite Sheet 动画状态机
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual")
	class USpriteSheetAnimComponent* AnimComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual")
	float VisualPlacementOffsetZ = 0.0f;

	// SpritePlane 统一缩放，编辑器可调
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Visual")
	float SpriteVisualScale = 2.0f;

	// SpritePlane 固定旋转 (Pitch, Yaw, Roll)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Visual")
	FRotator SpriteRotation = FRotator(50.0f, 0.0f, 180.0f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UTexture2D* AvatarImage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<ABulletBase> BulletClass;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UTargetingComponent* TargetingComp;
	
	/*UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UHealthComponent* HealthComp;*/

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "State")
	EDeploymentDirection FacingDirection;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "State")
	FIntVector2 GridLocation;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "State")
	TArray<TScriptInterface<ITargetableInterface>> Target;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 DeploymentCost;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EOperatorDeployType DeployType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float RedeployTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EOperatorAttackType AttackType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EOperatorClassType OperatorClassType;

public:
	UFUNCTION(BlueprintCallable, Category = "Deployment")
	void OnDeployed(FIntVector2 Location, EDeploymentDirection Direction, int32 InitialLevel, ADeployableCell* DeployCell);

	UFUNCTION(BlueprintCallable, Category = "Deployment")
	void OnRetreat();
	
	FTimerHandle TargetingTimerHandle;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void OnTargetingTimerTick();

	FTimerHandle AttackingTimerHandle;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ExecuteAttack();

	// 根据 FacingDirection 拼接 Clip 名 (如 "Idle" → "Idle_Front")
	FName GetFacingClipName(const FString& BaseAnim) const;

public:	
	AOperatorBase();

};
