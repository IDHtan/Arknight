// Fill out your copyright notice in the Description page of Project Settings.


#include "../BattleMap/BattleHUDWidget.h"
#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "ExitWidget.h"
#include "OperatorDetailWidget.h"
#include "CellDetailWidget.h"
#include "BattlePlayerController.h"
#include "../Operator/OperatorBase.h"

void UBattleHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (PauseCheckBox)
	{
		PauseCheckBox->OnCheckStateChanged.AddDynamic(this, &UBattleHUDWidget::ChangePauseState);
	}

	if(SpeedCheckBox)
	{
		SpeedCheckBox->OnCheckStateChanged.AddDynamic(this, &UBattleHUDWidget::ChangeSpeedState);
	}

	if (ExitButton)
	{
		ExitButton->OnClicked.AddDynamic(this, &UBattleHUDWidget::OnExitButtonClicked);
	}

	if (ABattlePlayerController* BattlePC = Cast<ABattlePlayerController>(GetOwningPlayer()))
	{
		BattlePC->OnCostChanged.AddDynamic(this, &UBattleHUDWidget::OnCostUpdated);
		OnCostUpdated(BattlePC->CurrentDeploymentCost);
	}

	bIsGamePaused = false;
	CurrentSpeed = EGameSpeedState::Speed_1x;

	if(ExitConfirmPanel)
	{
		ExitConfirmPanel->SetVisibility(ESlateVisibility::Collapsed);
	}

	if(OperatorDetailPanel)
	{
		OperatorDetailPanel->SetVisibility(ESlateVisibility::Collapsed);
	}

	if(CellDetailPanel)
	{
		CellDetailPanel->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UBattleHUDWidget::ChangePauseState(bool bIsChecked)
{
	UE_LOG(LogTemp, Log, TEXT("Pause state changed: %s"), bIsChecked ? TEXT("Checked") : TEXT("Unchecked"));

	if(bIsChecked)
	{
		UGameplayStatics::SetGamePaused(GetWorld(), true);
		bIsGamePaused = true;
	}
	else
	{
		UGameplayStatics::SetGamePaused(GetWorld(), false);
		bIsGamePaused = false;
	}
}

void UBattleHUDWidget::ChangeSpeedState(bool bIsChecked)
{
	UE_LOG(LogTemp, Log, TEXT("Speed state changed: %s"), bIsChecked ? TEXT("Checked") : TEXT("Unchecked"));
	if(bIsChecked)
	{
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 2.0f);
		CurrentSpeed = EGameSpeedState::Speed_2x;
	}
	else
	{
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);
		CurrentSpeed = EGameSpeedState::Speed_1x;
	}
}

void UBattleHUDWidget::OnExitButtonClicked()
{
	UE_LOG(LogTemp, Log, TEXT("Exit button clicked"));
	if (ExitConfirmPanel)
	{
		ExitConfirmPanel->Open();
	}
}

void UBattleHUDWidget::OnCostUpdated(int32 NewCost)
{
	if (TextCost)
	{
		TextCost->SetText(FText::AsNumber(NewCost));
	}
}

void UBattleHUDWidget::ShowOperatorSelected(AOperatorBase* SelectedOp)
{
	HideAllDetails();
	if(OperatorDetailPanel)
	{
		UE_LOG(LogTemp, Log, TEXT("Showing details for operator"));
		OperatorDetailPanel->UpdateAndShow(SelectedOp);
	}
}

void UBattleHUDWidget::ShowOperatorDetailByClass(TSubclassOf<AOperatorBase> OpClass)
{
	HideAllDetails();
	if(OperatorDetailPanel && !OpClass)
	{
		AOperatorBase* DefaultOp = OpClass->GetDefaultObject<AOperatorBase>();
		if(DefaultOp)
		{
			UE_LOG(LogTemp, Log, TEXT("Showing details for operator"));
			OperatorDetailPanel->UpdateAndShow(DefaultOp);
		}
	}
}

void UBattleHUDWidget::ShowCellSelected(AResourceCell* SelectedCell)
{
	HideAllDetails();
	if(CellDetailPanel)
	{
		UE_LOG(LogTemp, Log, TEXT("Showing details for cell"));
		CellDetailPanel->UpdateAndShow(SelectedCell);
	}
}

void UBattleHUDWidget::HideAllDetails()
{
	if(OperatorDetailPanel)
	{
		OperatorDetailPanel->SetVisibility(ESlateVisibility::Collapsed);
	}
	if(CellDetailPanel)
	{
		CellDetailPanel->SetVisibility(ESlateVisibility::Collapsed);
	}
}