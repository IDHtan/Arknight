
#include "DeployPanel.h"
#include "Components/Button.h"
#include "Components/Image.h"
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

	TSubclassOf<AOperatorBase> OperatorClass=
		Cast<ABattlePlayerController>(GetOwningPlayer())->
		LocalRoster.FindByPredicate(
			[&OpName](const FOperatorLocalRosterData& Data) {
				return Data.OperatorName == OpName;
			})->
		OperatorClass;
	AOperatorBase* DefaultOperator = OperatorClass->GetDefaultObject<AOperatorBase>();
	if(!DefaultOperator)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to find operator class for %s"), *OpName.ToString());
		return;
	}

	if(ImageAvatar)
	{
		ImageAvatar->SetBrushFromTexture(DefaultOperator->AvatarImage);
	}
}

void UDeployWidget::OnUpClicked()
{
	Cast<ABattlePlayerController>(GetOwningPlayer())->Deploy(OperatorName, TargetCell, EDeploymentDirection::Up);
	HidePanel();
}

void UDeployWidget::OnDownClicked()
{
	Cast<ABattlePlayerController>(GetOwningPlayer())->Deploy(OperatorName, TargetCell, EDeploymentDirection::Down);
	HidePanel();
}

void UDeployWidget::OnLeftClicked()
{
	Cast<ABattlePlayerController>(GetOwningPlayer())->Deploy(OperatorName, TargetCell, EDeploymentDirection::Left);
	HidePanel();
}

void UDeployWidget::OnRightClicked()
{
	Cast<ABattlePlayerController>(GetOwningPlayer())->Deploy(OperatorName, TargetCell, EDeploymentDirection::Right);
	HidePanel();
}

void UDeployWidget::HidePanel()
{
	SetVisibility(ESlateVisibility::Collapsed);
	OperatorName = NAME_None;
	TargetCell = nullptr;
}
