// Fill out your copyright notice in the Description page of Project Settings.


#include "ResourceCell.h"
#include "../../Component/HealthComponent.h"
#include "../../URougeliteRunSubsystem.h"
#include "../BattleMapManager.h"
#include "Kismet/GameplayStatics.h"

AResourceCell::AResourceCell()
{
	HealthComp = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));

}

void AResourceCell::BeginPlay()
{
	Super::BeginPlay();
	
	if (HealthComp)
	{
		HealthComp->OnDeathEvent.AddDynamic(this, &AResourceCell::OnCellDepleted);
	}
}

void AResourceCell::OnCellDepleted(AActor* Killer)
{
	UE_LOG(LogTemp, Warning, TEXT("Resource Cell Depleted!"));

	DropAmount--;
	if(UGameInstance* GameInstance = GetGameInstance())
	{
		if(URougeliteRunSubsystem* Rougelite = GameInstance->GetSubsystem<URougeliteRunSubsystem>())
		{
			Rougelite->AddResource(ResourceType, 1);
		}
	}

	
	if (DropAmount <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Resource Cell is degrading"));
		ABattleMapManager* MapManager = Cast<ABattleMapManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ABattleMapManager::StaticClass()));
		if (MapManager)
		{
			MapManager->ReplaceCell(GridCoordinate, DegradeToCellClass);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Resource Cell still has %d drops left"), DropAmount);
		HealthComp->CurrentHealth = HealthComp->MaxHealth;
	}
}

FIntVector2 AResourceCell::GetGridPosition_Implementation() const
{
	return GridCoordinate;
}

bool AResourceCell::CanBeTargetedBy_Implementation(AActor* DamageInstigator) const
{
	if (DropAmount > 0&& HealthComp && HealthComp->CurrentHealth > 0) 
	{
		return true;
	}
	else
	{
		return false;
	}
	
}

void AResourceCell::TakeCombatDamage_Implementation(float DamageAmount, EDamageType DamageType, AActor* DamageInstigator)
{
	if (HealthComp)
	{
		HealthComp->ApplyDamage(DamageAmount, DamageType, DamageInstigator);
	}
}

