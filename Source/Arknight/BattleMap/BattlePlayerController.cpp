// Fill out your copyright notice in the Description page of Project Settings.


#include "BattlePlayerController.h"
#include "../URougeliteRunSubsystem.h"
#include "../Operator/OperatorBase.h"
#include "BattleMapManager.h"
#include "Kismet/GameplayStatics.h"
#include "DeployableCell.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "BattleHUDWidget.h"

void ABattlePlayerController::BeginPlay()
{
	Super::BeginPlay();

	bIsGamePaused = false;
	CurrentSpeed = EGameSpeedState::Speed_1x;

	LocalRoster.Empty();
	URougeliteRunSubsystem* RunSubsystem = GetGameInstance()->GetSubsystem<URougeliteRunSubsystem>();
	if (RunSubsystem)
	{
		LocalRoster.Reserve(RunSubsystem->GlobalRoster.Num());
		for (const FOperatorRosterData& RosterData : RunSubsystem->GlobalRoster)
		{
			FOperatorLocalRosterData LocalData;
			LocalData.OperatorName = RosterData.OperatorName;
			LocalData.OperatorClass = RosterData.OperatorClass;
			LocalData.OperatorLevel = RosterData.OperatorLevel;
			LocalData.bIsDeployed = false;
			LocalRoster.Add(LocalData);
		}

		RefreshLocalRosterDeployability();
	}

	bShowMouseCursor = true;
	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	SetInputMode(InputMode);

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		if (BattleMappingContext)
		{
			Subsystem->AddMappingContext(BattleMappingContext, 0);
		}
	}

	if (HUDWidgetClass != nullptr)
	{
		HUDWidgetInstance = CreateWidget<UBattleHUDWidget>(this, HUDWidgetClass);
		if (HUDWidgetInstance)
		{
			HUDWidgetInstance->AddToViewport();

			//more initialization here to be writen in the future, like bind event to hud widget
		}
	}

	GetWorld()->GetTimerManager().SetTimer(
		IncreaseCostNaturallyTimerHandle, 
		[this](){this->IncreaseCost(1);}
		, 1.0f, true);
}

void ABattlePlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	GetWorld()->GetTimerManager().ClearTimer(IncreaseCostNaturallyTimerHandle);
}

void ABattlePlayerController::IncreaseCost(int32 Amount)
{
	CurrentDeploymentCost += Amount;
	RefreshLocalRosterDeployability();
	OnCostChanged.Broadcast(CurrentDeploymentCost);
}

void ABattlePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent))
	{
		if (ClickAction)
		{
			EnhancedInput->BindAction(ClickAction, ETriggerEvent::Started, this, &ABattlePlayerController::OnClickStarted);
		}
	}
}

void ABattlePlayerController::OnClickStarted()
{
	if (bIsGamePaused)
	{
		return;
	}

	FHitResult HitResult;
	bool bHit = GetHitResultUnderCursor(ECC_Visibility, false, HitResult);

	if (bHit && HitResult.GetActor())
	{
		SelectedOperator = Cast<AOperatorBase>(HitResult.GetActor());
		SelectedCell = Cast<ABattleCell>(HitResult.GetActor());
		if(SelectedOperator)
		{
			UE_LOG(LogTemp, Warning, TEXT("Selected Operator: %s"), *SelectedOperator->GetName());
			//call out ui to show operator info and possible actions
		}
		else if(SelectedCell)
		{
			UE_LOG(LogTemp, Warning, TEXT("Selected Cell: %s"), *SelectedCell->GetName());
			//call out ui to show cell info and possible actions
		}
		else
		{
			SelectedOperator = nullptr;
			SelectedCell = nullptr;
		}
	}
	else
	{
		SelectedOperator = nullptr;
		SelectedCell = nullptr;
	}
}

// the cell to deploy is known, the cel can be deploy is known, check operator deploy cost and deploy
void ABattlePlayerController::ExecuteDeployment(FName OperatorName, ADeployableCell* DeployCell, EDeploymentDirection DeployDirection)
{
	RefreshLocalRosterDeployability();

	FOperatorLocalRosterData* OperatorData = LocalRoster.FindByPredicate([&OperatorName](const FOperatorLocalRosterData& Data) {
		return Data.OperatorName == OperatorName;
	});
	if (OperatorData && OperatorData->OperatorClass && OperatorData->bCanDeploy)
	{
		AOperatorBase* DefaultOperator = OperatorData->OperatorClass->GetDefaultObject<AOperatorBase>();
		if (!DefaultOperator || CurrentDeploymentCost < DefaultOperator->DeploymentCost)
		{
			RefreshLocalRosterDeployability();
			return;
		}

		FVector CellWorldLocation =DeployCell->GetActorLocation();
		FVector OpSpawnLocation = FVector(CellWorldLocation.X, CellWorldLocation.Y, (DeployCell->LogicalCellType == ECellType::DeployableGround) ? 50.0f : 75.0f);
		AOperatorBase* SpawnedOperator = GetWorld()->SpawnActor<AOperatorBase>(OperatorData->OperatorClass, OpSpawnLocation, FRotator::ZeroRotator);
		if (SpawnedOperator)
		{
			SpawnedOperator->OnDeployed(DeployCell->GridCoordinate, DeployDirection, OperatorData->OperatorLevel);
			
			OperatorData->bIsDeployed = true;
			IncreaseCost(-DefaultOperator->DeploymentCost);

			OnRosterUpdated.Broadcast(LocalRoster);
		}
	}
}

void ABattlePlayerController::RefreshLocalRosterDeployability()
{
	for (FOperatorLocalRosterData& RosterData : LocalRoster)
	{
		const AOperatorBase* DefaultOperator = RosterData.OperatorClass
			? RosterData.OperatorClass->GetDefaultObject<AOperatorBase>()
			: nullptr;

		RosterData.bCanDeploy = !RosterData.bIsDeployed
			&& DefaultOperator
			&& CurrentDeploymentCost >= DefaultOperator->DeploymentCost;
	}
}
