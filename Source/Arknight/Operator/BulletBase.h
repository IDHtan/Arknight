// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../DS.h"
#include "../Interface/TargetableInterface.h"
#include "BulletBase.generated.h"

class AOperatorBase;

UCLASS()
class ARKNIGHT_API ABulletBase : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* MeshComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float FlySpeed;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
	FVector SelfCoord;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FVector TargetCoord;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float DamageAmount;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EDamageType DamageType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TScriptInterface<ITargetableInterface> BulletTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AActor* BulletOwner;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	FVector Direction;

public:	

	ABulletBase();

	UFUNCTION(BlueprintCallable)
	void InitializeBullet(
		FVector InSelfCoord, 
		FVector InTargetCoord, 
		float InDamageAmount, 
		EDamageType InDamageType,
		TScriptInterface<ITargetableInterface> InTarget,
		AActor* InInstigator
	);

	virtual void Tick(float DeltaTime) override;

};
