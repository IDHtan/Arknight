// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleHUDWidget.h"
#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "Components/TextBlock.h"
#include "Components/HorizontalBox.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h" 
#include "ExitWidget.h"
#include "OperatorDetailWidget.h"
#include "CellDetailWidget.h"
#include "OperatorCardWidget.h"
#include "DeployPanel.h"
#include "../Cell/DeployableCell.h"
#include "../Controller/BattlePlayerController.h"
#include "../../Operator/OperatorBase.h"
#include "../../DS.h"

void UBattleHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

#pragma region Setups
	if (PauseCheckBox)
	{
		PauseCheckBox->OnCheckStateChanged.AddDynamic(this, &UBattleHUDWidget::ChangePauseState);
	}

	if (SpeedCheckBox)
	{
		SpeedCheckBox->OnCheckStateChanged.AddDynamic(this, &UBattleHUDWidget::ChangeSpeedState);
	}

	if (ExitButton)
	{
		ExitButton->OnClicked.AddDynamic(this, &UBattleHUDWidget::OnExitButtonClicked);
	}

	BattlePC = Cast<ABattlePlayerController>(GetOwningPlayer());
	if (BattlePC)
	{
		BattlePC->OnCostChanged.AddDynamic(this, &UBattleHUDWidget::OnCostUpdated);
		OnCostUpdated(BattlePC->CurrentDeploymentCost);
	}
#pragma endregion

	bIsGamePaused = false;
	CurrentSpeed = EGameSpeedState::Speed_1x;

	for(const FOperatorLocalRosterData& LocalData : BattlePC->LocalRoster)
	{
		UOperatorCardWidget* NewCard = CreateWidget<UOperatorCardWidget>(this, OperatorCardWidgetClass);
		if (NewCard)
		{
			NewCard->SetVisibility(ESlateVisibility::Visible);
			NewCard->OnCreated(LocalData);
			RosterPanel->AddChild(NewCard);
		}
	}

#pragma region InitializeVisibility
	if (ExitConfirmPanel)
	{
		ExitConfirmPanel->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (OperatorDetailPanel)
	{
		OperatorDetailPanel->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (CellDetailPanel)
	{
		CellDetailPanel->SetVisibility(ESlateVisibility::Collapsed);
	}
#pragma endregion	
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
	HideAllPanels();
	if(OperatorDetailPanel)
	{
		UE_LOG(LogTemp, Log, TEXT("Showing details for operator"));
		OperatorDetailPanel->UpdateAndShow(SelectedOp->OperatorName);
	}
}

void UBattleHUDWidget::ShowCellSelected(AResourceCell* SelectedCell)
{
	HideAllPanels();
	if(CellDetailPanel)
	{
		UE_LOG(LogTemp, Log, TEXT("Showing details for cell"));
		CellDetailPanel->UpdateAndShow(SelectedCell);
	}
}

void UBattleHUDWidget::HideAllPanels()
{
	if(OperatorDetailPanel)
	{
		OperatorDetailPanel->HidePanel();
	}
	if(CellDetailPanel)
	{
		CellDetailPanel->HidePanel();
	}
	if(DeployPanel)
	{
		DeployPanel->HidePanel();
	}
}

void UBattleHUDWidget::ShowDeployPanel(FName OperatorName, ADeployableCell* DeployableCell)
{
	HideAllPanels();
	if(DeployPanel)
	{
		UE_LOG(LogTemp, Log, TEXT("Showing deploy panel for operator: %s"), *OperatorName.ToString());
		
		FVector TargetWorldPos = DeployableCell->GetActorLocation();
		FVector2D ScreenPos;
		UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(
			BattlePC,
			TargetWorldPos,
			ScreenPos,
			false
		);
		if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(DeployPanel->Slot))
		{
			CanvasSlot->SetPosition(ScreenPos);
		}

		DeployPanel->OnShow(OperatorName, DeployableCell);
		DeployPanel->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("DeployPanel is not set in the HUD widget"));
	}
}
