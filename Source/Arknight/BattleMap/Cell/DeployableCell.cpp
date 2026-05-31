// Fill out your copyright notice in the Description page of Project Settings.


#include "DeployableCell.h"

void ADeployableCell::SetOccupyingOperator(AOperatorBase* Operator)
{
	if (Operator) 
	{
		OccupyingOperator = Operator;
		bIsOccupied = true;
	}
	else
	{
		OccupyingOperator = nullptr;
		bIsOccupied = false;
	}
	
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
