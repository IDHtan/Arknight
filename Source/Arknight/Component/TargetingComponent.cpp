// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetingComponent.h"
#include "Kismet/GameplayStatics.h"
#include "../BattleMap/BattleCell.h"
#include "../Operator/OperatorBase.h"
#include "../Interface/TargetableInterface.h"
#include "../BattleMap/BattleMapManager.h"

// Sets default values for this component's properties
UTargetingComponent::UTargetingComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UTargetingComponent::InitializeAbsoluteRange(FIntVector2 Origin, EDeploymentDirection FacingDirection)
{
	CurrentAbsoluteRange.Empty();
	for (const FIntVector2& RelativePos : BaseAttackRange)
	{
		FIntVector2 AbsolutePos = Origin;
		switch (FacingDirection)
		{
		case EDeploymentDirection::Up:
			AbsolutePos += FIntVector2(-RelativePos.Y,RelativePos.X);
			break;
		case EDeploymentDirection::Right:
			AbsolutePos += FIntVector2(-RelativePos.X, -RelativePos.Y);
			break;
		case EDeploymentDirection::Down:
			AbsolutePos += FIntVector2(RelativePos.Y, -RelativePos.X);
			break;
		case EDeploymentDirection::Left:
			AbsolutePos += RelativePos;
			break;
		default:
			break;
		}
		CurrentAbsoluteRange.Add(AbsolutePos);
	}
}

TArray<TScriptInterface<ITargetableInterface>> UTargetingComponent::PerformGridScan()
{
	ABattleMapManager* BattleMapManager = Cast<ABattleMapManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ABattleMapManager::StaticClass()));
	TArray<TScriptInterface<ITargetableInterface>> ValidTargetCells;

	if (!BattleMapManager)
		return ValidTargetCells;

	for (const FIntVector2& Pos : CurrentAbsoluteRange)
	{
		ABattleCell* TargetCell = BattleMapManager->GetCellFromCoordinate(Pos);
		if (!TargetCell && !TargetCell->Implements<UTargetableInterface>())
			continue;
		if (!ITargetableInterface::Execute_CanBeTargetedBy(TargetCell, GetOwner()))
			continue;
		ValidTargetCells.Add(TScriptInterface<ITargetableInterface>(TargetCell));
	}

	ValidTargetCells.Sort([this](const TScriptInterface<ITargetableInterface>& A, const TScriptInterface<ITargetableInterface>& B)
		{
			return CalculateTargetScore(A) < CalculateTargetScore(B);
		});

	ValidTargetCells.SetNum(MaxTargets);
	return ValidTargetCells;

}

//smaller score means higher priority, default is distance to the target, but can be overridden for more complex logic
float UTargetingComponent::CalculateTargetScore_Implementation(const TScriptInterface<ITargetableInterface>& Target)
{
	FIntVector2 Pos1 = ITargetableInterface::Execute_GetGridPosition(Target.GetObject());
	FIntVector2 Pos2 = Cast<AOperatorBase>(GetOwner())->GridLocation;

	float Distance = FVector2D::Distance(FVector2D(Pos1.X, Pos1.Y), FVector2D(Pos2.X, Pos2.Y));

	return Distance;
}