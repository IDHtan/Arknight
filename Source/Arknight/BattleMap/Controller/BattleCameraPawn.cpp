// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleCameraPawn.h"


ABattleCameraPawn::ABattleCameraPawn()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	RootComponent = RootComp;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	SpringArm->bDoCollisionTest = false;
	
	Camera->SetFieldOfView(20.f);
}

void ABattleCameraPawn::FrameGridToScreen(int32 GridX, int32 GridY, float CellSize)
{
	SetActorLocation(FVector(CellSize * (GridX - 1) / 2, CellSize * (GridY) / 2, 0));
	RootComp->SetWorldRotation(FRotator(0.f, 0.f, 0.f));
	SpringArm->SetRelativeRotation(FRotator(-60.f, 90.f, 0.f));

	float Y = (GridY * 3 + 12) * CellSize;
	float X = (GridX * 3 - 3) * CellSize;

	SpringArm->TargetArmLength = FMath::Max(X,Y);
	
}

