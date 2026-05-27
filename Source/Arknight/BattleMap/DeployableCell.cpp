// Fill out your copyright notice in the Description page of Project Settings.


#include "../BattleMap/DeployableCell.h"

bool ADeployableCell::SetOccupyingOperator(AOperatorBase* Operator)
{
	if (bIsOccupied)
	{
		return false;
	}
	OccupyingOperator = Operator;
	bIsOccupied = true;
	return true;
}

bool ADeployableCell::CanDeploy(EOperatorDeployType DeployType)
{
	switch (DeployType)
	{
	case EOperatorDeployType::Melee:
		if(ECellType::DeployableGround == LogicalCellType)
		{
			EnableVisualEffect();
			return true;
		}
		break;
	case EOperatorDeployType::Ranged:
		if(ECellType::DeployableHighGround == LogicalCellType)
		{
			EnableVisualEffect();
			return true;
		}
		break;
	case EOperatorDeployType::Any:
		EnableVisualEffect();
		return true;
	}

	DisableVisualEffect();
	return false;
}
