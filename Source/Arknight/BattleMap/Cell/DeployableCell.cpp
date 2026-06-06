// Fill out your copyright notice in the Description page of Project Settings.


#include "DeployableCell.h"
#include "../../DS.h"
#include "../Controller/BattlePlayerController.h"
#include "Kismet/GameplayStatics.h"

void ADeployableCell::BeginPlay()
{
	Super::BeginPlay();

	// 缓存原始材质并创建动态材质实例（不在构造里做，因为蓝图可能还没设置材质）
	if (CellMeshComponent)
	{
		UMaterialInterface* BaseMaterial = CellMeshComponent->GetMaterial(0);
		if (BaseMaterial)
		{
			DynamicMaterialInstance = UMaterialInstanceDynamic::Create(BaseMaterial, this);
			CellMeshComponent->SetMaterial(0, DynamicMaterialInstance);
		}
	}
}

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

	//***
	//*metion*: this need to reset back after viusal effect is implemented
	//***
	/*ABattlePlayerController* BattlePC = Cast<ABattlePlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (!BattlePC)
	{
		UE_LOG(LogTemp, Warning, TEXT("DeployableCell set up failed: PlayerController not found"));
		return;
	}
	BattlePC->OnOperatorCardClicked.AddDynamic(this, &ADeployableCell::OnOperatorCardClicked);*/
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

void ADeployableCell::OnOperatorCardClicked(FName OperatorName)
{
	ABattlePlayerController* BattlePC = Cast<ABattlePlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if(!BattlePC)
	{
		UE_LOG(LogTemp, Warning, TEXT("DeployableCell OnOperatorCardClicked failed: PlayerController not found, OperatorName=%s"), *OperatorName.ToString());
		return;
	}

	FOperatorLocalRosterData* OperatorData = BattlePC->LocalRoster.FindByPredicate(
		[&OperatorName](const FOperatorLocalRosterData& Data) {
			return Data.OperatorName == OperatorName;
		});
	if(!OperatorData)
	{
		UE_LOG(LogTemp, Warning, TEXT("DeployableCell OnOperatorCardClicked failed: OperatorData not found, OperatorName=%s"), *OperatorName.ToString());
		return;
	}

	if (OperatorData->bIsDeployed)
	{
		return;
	}

	if((OperatorData->DeployType==EOperatorDeployType::Melee && LogicalCellType != ECellType::DeployableGround) ||
		(OperatorData->DeployType==EOperatorDeployType::Ranged && LogicalCellType != ECellType::DeployableHighGround))
	{
		EnableVisualEffect();
	}
	else
	{
		DisableVisualEffect();
	}
}

void ADeployableCell::EnableVisualEffect_Implementation()
{
	if (DynamicMaterialInstance)
	{
		DynamicMaterialInstance->SetVectorParameterValue(TintParameterName, EnabledTintColor);
	}
}

void ADeployableCell::DisableVisualEffect_Implementation()
{
	if (DynamicMaterialInstance)
	{
		DynamicMaterialInstance->SetVectorParameterValue(TintParameterName, FLinearColor::White);
	}
}
