// Fill out your copyright notice in the Description page of Project Settings.


#include "BattlePlayerController.h"
#include "../URougeliteRunSubsystem.h"

void ABattlePlayerController::BeginPlay()
{
	Super::BeginPlay();

	CurrentDeploymentCost = 10;
	LocalRoster.Empty();
	URougeliteRunSubsystem* RunSubsystem = GetGameInstance()->GetSubsystem<URougeliteRunSubsystem>();
	if (RunSubsystem)
	{
		LocalRoster = RunSubsystem->GlobalRoster;
	}
}