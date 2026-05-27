// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Component/HealthComponent.h"
#include "../Component/TargetingComponent.h"
#include "../DS.h"
#include "OperatorBase.generated.h"

UCLASS()
class ARKNIGHT_API AOperatorBase : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UStaticMeshComponent* SpriteComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> OperatorAvatar;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UTargetingComponent* TargetingComp;
	
	/*UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UHealthComponent* HealthComp;*/

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "State")
	EDeploymentDirection CurrentFacingDirection;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "State")
	FIntVector2 CurrentGridLocation;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "State")
	TArray<TScriptInterface<ITargetableInterface>> CurrentTarget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Deployment")
	int32 DeploymentCost;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Deployment")
	EOperatorDeployType DeployType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EOperatorClassType OperatorClassType;
public:
	/*UFUNCTION(BlueprintCallable, Category = "Deployment")
	void OnDeployed(FIntVector2 Location, EDeploymentDirection Direction, int32 InitialLevel);
	
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void OnTargetingTimerTick();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ExecuteAttack();*/

public:	
	AOperatorBase();


};
