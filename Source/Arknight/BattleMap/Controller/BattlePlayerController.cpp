// Fill out your copyright notice in the Description page of Project Settings.


#include "BattlePlayerController.h"
#include "../../URougeliteRunSubsystem.h"
#include "../../Operator/OperatorBase.h"
#include "../BattleMapManager.h"
#include "Kismet/GameplayStatics.h"
#include "../Cell/DeployableCell.h"
#include "../Cell/ResourceCell.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "../UI/BattleHUDWidget.h"

ABattlePlayerController::ABattlePlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABattlePlayerController::BeginPlay()
{
	Super::BeginPlay();

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

	SelectedOperator = nullptr;
	SelectedOperatorCardName = NAME_None;
	SelectedResourceCell = nullptr;
	SelectedDeployableCell = nullptr;

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
			LocalData.OperatorInstance = nullptr;
			LocalData.bIsDeployed = false;
			AOperatorBase* DefaultOperator = RosterData.OperatorClass->GetDefaultObject<AOperatorBase>();
			if (DefaultOperator)
			{
				LocalData.CurrentDeploymentCost = DefaultOperator->DeploymentCost;
				LocalData.RedeployTime = DefaultOperator->RedeployTime;
				LocalData.RedeployCooldown = 0.0f;
			}
			LocalRoster.Add(LocalData);
		}
		LocalRoster.Sort([](const FOperatorLocalRosterData& A, const FOperatorLocalRosterData& B) {
			return A.CurrentDeploymentCost < B.CurrentDeploymentCost; // Sort by deployment cost ascending
			});
	}

	GetWorld()->GetTimerManager().SetTimer(
		IncreaseCostNaturallyTimerHandle, 
		[this](){this->IncreaseCost(1);}
		, 1.0f, true);

	if (HUDWidgetClass != nullptr)
	{
		HUDWidgetInstance = CreateWidget<UBattleHUDWidget>(this, HUDWidgetClass);
		if (HUDWidgetInstance)
		{
			HUDWidgetInstance->AddToViewport();

			//more initialization here to be writen in the future, like bind event to hud widget
		}
	}
}

void ABattlePlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetTimerManager().ClearTimer(IncreaseCostNaturallyTimerHandle);
	Super::EndPlay(EndPlayReason);
}

void ABattlePlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	for (FOperatorLocalRosterData& OperatorData : LocalRoster)
	{
		if (!OperatorData.bIsDeployed&& OperatorData.RedeployCooldown > 0.0f)
		{
			OperatorData.RedeployCooldown = FMath::Max(0.0f, OperatorData.RedeployCooldown - DeltaSeconds);
			if (OperatorData.RedeployCooldown == 0.0f)
			{
				UE_LOG(LogTemp, Log, TEXT("Operator %s is ready for redeployment"), *OperatorData.OperatorName.ToString());
			}
		}
	}
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

void ABattlePlayerController::IncreaseCost(int32 Amount)
{
	CurrentDeploymentCost += Amount;
	OnCostChanged.Broadcast(CurrentDeploymentCost);
}

void ABattlePlayerController::OnClickStarted()
{
	if (!HUDWidgetInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("HUDWidgetInstance is null"));
		return;
	}

	FHitResult HitResult;
	bool bHit = GetHitResultUnderCursor(ECC_Visibility, false, HitResult);

	if (!bHit || !HitResult.GetActor())
	{
		UE_LOG(LogTemp, Log, TEXT("Nothing is Clicked"));
		SelectedOperator = nullptr;
		SelectedOperatorCardName = NAME_None;
		SelectedResourceCell = nullptr;
		SelectedDeployableCell = nullptr;
		if (HUDWidgetInstance)
		{
			HUDWidgetInstance->HideAllPanels();
		}
		OperatorCardChanged(SelectedOperatorCardName);
	}

	SelectedOperator = Cast<AOperatorBase>(HitResult.GetActor());
	SelectedResourceCell = Cast<AResourceCell>(HitResult.GetActor());
	SelectedDeployableCell = Cast<ADeployableCell>(HitResult.GetActor());

	if(SelectedOperatorCardName!= NAME_None)
	{
		if(SelectedDeployableCell)
		{
			FOperatorLocalRosterData* OperatorData = LocalRoster.FindByPredicate(
				[this](const FOperatorLocalRosterData& Data) {
					return Data.OperatorName == this->SelectedOperatorCardName;
				});
			if (!OperatorData->bIsDeployed &&
				OperatorData->RedeployCooldown <= 0.0f &&
				OperatorData->CurrentDeploymentCost <= this->CurrentDeploymentCost &&
				!SelectedDeployableCell->bIsOccupied)
			{
				UE_LOG(LogTemp, Log, TEXT("Calling ShowDeployPanel"));
				HUDWidgetInstance->ShowDeployPanel(SelectedOperatorCardName, SelectedDeployableCell);
				SelectedOperatorCardName = NAME_None;
				OperatorCardChanged(SelectedOperatorCardName);
				SelectedDeployableCell = nullptr;
				return;
			}
		}
		UE_LOG(LogTemp, Log, TEXT("Broadcasting operatorcard to lower"));
		HUDWidgetInstance->HideAllPanels();
		SelectedOperatorCardName = NAME_None;
		OperatorCardChanged(SelectedOperatorCardName);		
	}	
	SelectedDeployableCell = nullptr;

	if (SelectedOperator)
	{
		UE_LOG(LogTemp, Warning, TEXT("Selected Operator: %s"), *SelectedOperator->OperatorName.ToString());
		HUDWidgetInstance->ShowOperatorSelected(SelectedOperator);
	}
	else if(SelectedResourceCell)
	{
		UE_LOG(LogTemp, Warning, TEXT("Selected Resource Cell: %s"), *SelectedResourceCell->GetName());
		HUDWidgetInstance->ShowCellSelected(SelectedResourceCell);
	}
}

void ABattlePlayerController::OperatorCardChanged(FName OperatorName)
{
	SelectedOperatorCardName = OperatorName;
	OnOperatorCardClicked.Broadcast(OperatorName);
}

// the cell to deploy is known, the cell can be deploy is known, check operator deploy cost and deploy
void ABattlePlayerController::Deploy(FName OperatorName, ADeployableCell* DeployCell, EDeploymentDirection DeployDirection)
{
	FOperatorLocalRosterData* OperatorData = LocalRoster.FindByPredicate(
		[&OperatorName](const FOperatorLocalRosterData& Data) {
			return Data.OperatorName == OperatorName;
		});
	if (OperatorData && OperatorData->OperatorClass)
	{
		AOperatorBase* DefaultOperator = OperatorData->OperatorClass->GetDefaultObject<AOperatorBase>();
		if (!DefaultOperator || CurrentDeploymentCost < DefaultOperator->DeploymentCost)
		{
			return;
		}

		FVector CellWorldLocation =DeployCell->GetActorLocation();
		FVector OpSpawnLocation = FVector(CellWorldLocation.X, CellWorldLocation.Y, (DeployCell->LogicalCellType == ECellType::DeployableGround) ? 50.0f : 75.0f);
		AOperatorBase* SpawnedOperator = GetWorld()->SpawnActor<AOperatorBase>(OperatorData->OperatorClass, OpSpawnLocation, FRotator::ZeroRotator);
		if (SpawnedOperator)
		{
			UE_LOG(LogTemp, Log, TEXT("Operator deployed: %s"), *SpawnedOperator->OperatorName.ToString());
			SpawnedOperator->OnDeployed(DeployCell->GridCoordinate, DeployDirection, OperatorData->OperatorLevel);
			DeployCell->SetOccupyingOperator(SpawnedOperator);
			OperatorData->bIsDeployed = true;
			OperatorData->OperatorInstance = SpawnedOperator;
			IncreaseCost(-DefaultOperator->DeploymentCost);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to spawn operator: %s"), *OperatorName.ToString());
		}
		HUDWidgetInstance->HideAllPanels();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Operator class not found for: %s"), *OperatorName.ToString());
	}
}

void ABattlePlayerController::Retreat(AOperatorBase* Operator)
{
	FOperatorLocalRosterData* OperatorData = LocalRoster.FindByPredicate([&Operator](const FOperatorLocalRosterData& Data) {
		return Data.OperatorName == Operator->OperatorName;
	});
	if (OperatorData)
	{
		IncreaseCost(OperatorData->CurrentDeploymentCost * 0.5f);
		OperatorData->CurrentDeploymentCost = FMath::Min(
			OperatorData->CurrentDeploymentCost * 1.5f, 
			OperatorData->OperatorInstance->DeploymentCost * 2.0f);

		if (ABattleMapManager* MapManager = Cast<ABattleMapManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ABattleMapManager::StaticClass())))
		{
			if (ABattleCell* Cell = MapManager->GetCellFromCoordinate(OperatorData->OperatorInstance->GridLocation))
			{
				if (ADeployableCell* DeployCell = Cast<ADeployableCell>(Cell))
				{
					DeployCell->SetOccupyingOperator(nullptr);
				}
			}
		}

		OperatorData->OperatorInstance->OnRetreat();
		UE_LOG(LogTemp, Log, TEXT("Operator retreated: %s"), *OperatorData->OperatorName.ToString());

		OperatorData->bIsDeployed = false;
		OperatorData->OperatorInstance = nullptr;
		OperatorData->RedeployCooldown = OperatorData->RedeployTime;				
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Operator data not found for: %s"), *Operator->OperatorName.ToString());
	}

}
