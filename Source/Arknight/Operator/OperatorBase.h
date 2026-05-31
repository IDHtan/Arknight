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

UCLASS()
class ARKNIGHT_API AOperatorBase : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName OperatorName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText OperatorDescription;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UStaticMeshComponent* SpriteComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual")
	float VisualPlacementOffsetZ = 0.0f;

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

public:	
	AOperatorBase();

};
