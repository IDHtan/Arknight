
#include "DeployPanel.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"
#include "../../Operator/OperatorBase.h"
#include "../Controller/BattlePlayerController.h"

void UDeployWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (Up)
	{
		Up->OnClicked.AddDynamic(this, &UDeployWidget::OnUpClicked);
	}
	if (Down)
	{
		Down->OnClicked.AddDynamic(this, &UDeployWidget::OnDownClicked);
	}
	if (Left)
	{
		Left->OnClicked.AddDynamic(this, &UDeployWidget::OnLeftClicked);
	}
	if (Right)
	{
		Right->OnClicked.AddDynamic(this, &UDeployWidget::OnRightClicked);
	}
}

void UDeployWidget::OnShow(FName OpName, ADeployableCell* DeployableCell)
{
	OperatorName = OpName;
	TargetCell = DeployableCell;

	if (!TargetCell)
	{
		UE_LOG(LogTemp, Warning, TEXT("DeployPanel OnShow failed: DeployableCell is null"));
		return;
	}

	ABattlePlayerController* BattlePC = Cast<ABattlePlayerController>(GetOwningPlayer());
	if (!BattlePC)
	{
		BattlePC = Cast<ABattlePlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	}
	if (!BattlePC)
	{
		UE_LOG(LogTemp, Warning, TEXT("DeployPanel OnShow failed: OwningPlayer is not ABattlePlayerController, OpName=%s"), *OpName.ToString());
		return;
	}

	FOperatorLocalRosterData* OperatorData = BattlePC->LocalRoster.FindByPredicate(
		[&OpName](const FOperatorLocalRosterData& Data) {
			return Data.OperatorName == OpName;
		});
	if (!OperatorData || !OperatorData->OperatorClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("DeployPanel OnShow failed: Operator not found or OperatorClass null, OpName=%s"), *OpName.ToString());
		return;
	}

	AOperatorBase* DefaultOperator = OperatorData->OperatorClass->GetDefaultObject<AOperatorBase>();
	if(!DefaultOperator)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to find operator class for %s"), *OpName.ToString());
		return;
	}

	if(ImageAvatar)
	{
		if (DefaultOperator->AvatarImage)
		{
			ImageAvatar->SetBrushFromTexture(DefaultOperator->AvatarImage);
			ImageAvatar->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		UE_LOG(LogTemp, Log, TEXT("Set brush right"));
	}
	UE_LOG(LogTemp, Log, TEXT("Show Deploy Panel function done"));
	SetVisibility(ESlateVisibility::Visible);
}

void UDeployWidget::OnUpClicked()
{
	ABattlePlayerController* BattlePC = Cast<ABattlePlayerController>(GetOwningPlayer());
	if (!BattlePC)
	{
		BattlePC = Cast<ABattlePlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	}
	if (!BattlePC || OperatorName == NAME_None || !TargetCell)
	{
		return;
	}
	BattlePC->Deploy(OperatorName, TargetCell, EDeploymentDirection::Up);
	HidePanel();
}

void UDeployWidget::OnDownClicked()
{
	ABattlePlayerController* BattlePC = Cast<ABattlePlayerController>(GetOwningPlayer());
	if (!BattlePC)
	{
		BattlePC = Cast<ABattlePlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	}
	if (!BattlePC || OperatorName == NAME_None || !TargetCell)
	{
		return;
	}
	BattlePC->Deploy(OperatorName, TargetCell, EDeploymentDirection::Down);
	HidePanel();
}

void UDeployWidget::OnLeftClicked()
{
	ABattlePlayerController* BattlePC = Cast<ABattlePlayerController>(GetOwningPlayer());
	if (!BattlePC)
	{
		BattlePC = Cast<ABattlePlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	}
	if (!BattlePC || OperatorName == NAME_None || !TargetCell)
	{
		return;
	}
	BattlePC->Deploy(OperatorName, TargetCell, EDeploymentDirection::Left);
	HidePanel();
}

void UDeployWidget::OnRightClicked()
{
	ABattlePlayerController* BattlePC = Cast<ABattlePlayerController>(GetOwningPlayer());
	if (!BattlePC)
	{
		BattlePC = Cast<ABattlePlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	}
	if (!BattlePC || OperatorName == NAME_None || !TargetCell)
	{
		return;
	}
	BattlePC->Deploy(OperatorName, TargetCell, EDeploymentDirection::Right);
	HidePanel();
}

void UDeployWidget::HidePanel()
{
	SetVisibility(ESlateVisibility::Collapsed);
	OperatorName = NAME_None;
	TargetCell = nullptr;
}
