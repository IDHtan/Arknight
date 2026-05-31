// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleHUDWidget.h"
#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "Components/TextBlock.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
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

	if (BattlePC && RosterPanel && OperatorCardWidgetClass)
	{
		for (const FOperatorLocalRosterData& LocalData : BattlePC->LocalRoster)
		{
			UOperatorCardWidget* NewCard = CreateWidget<UOperatorCardWidget>(this, OperatorCardWidgetClass);
			if (NewCard)
			{
				NewCard->SetVisibility(ESlateVisibility::Visible);
				NewCard->OnCreated(LocalData);
				UHorizontalBoxSlot* BoxSlot = RosterPanel->AddChildToHorizontalBox(NewCard);
				if (BoxSlot)
				{
					BoxSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
					BoxSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 0.0f));
					BoxSlot->SetVerticalAlignment(VAlign_Bottom);
				}
			}
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
	if(DeployPanel)
	{
		DeployPanel->SetVisibility(ESlateVisibility::Collapsed);
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
		OperatorDetailPanel->UpdateAndShow(SelectedOp->OperatorName);
	}
}

void UBattleHUDWidget::ShowOperatorCardSelected(FName OperatorName)
{
	HideAllPanels();
	if(OperatorDetailPanel)
	{
		OperatorDetailPanel->UpdateAndShow(OperatorName);
	}
}

void UBattleHUDWidget::ShowCellSelected(AResourceCell* SelectedCell)
{
	HideAllPanels();
	if(CellDetailPanel)
	{
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
	if(DeployPanel)
	{
		UE_LOG(LogTemp, Log, TEXT("Showing deploy panel for operator: %s"), *OperatorName.ToString());

		if (!DeployableCell)
		{
			UE_LOG(LogTemp, Warning, TEXT("ShowDeployPanel failed: DeployableCell is null"));
			return;
		}
		if (!BattlePC)
		{
			UE_LOG(LogTemp, Warning, TEXT("ShowDeployPanel failed: BattlePC is null"));
			return;
		}

		FVector TargetWorldPos = DeployableCell->GetActorLocation();
		FVector2D ScreenPos;
		UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(
			BattlePC,
			TargetWorldPos,
			ScreenPos,
			false
		);
		UE_LOG(LogTemp, Warning, TEXT("Projected world position %s to screen position %s"), *TargetWorldPos.ToString(), *ScreenPos.ToString());

		if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(DeployPanel->Slot))
		{
			CanvasSlot->SetPosition(ScreenPos);
		}
		DeployPanel->SetVisibility(ESlateVisibility::Visible);
		DeployPanel->OnShow(OperatorName, DeployableCell);

	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("DeployPanel is not set in the HUD widget"));
	}
}

void UBattleHUDWidget::RefreshOperatorCards()
{
	if (!RosterPanel)
	{
		return;
	}

	for (int32 Index = 0; Index < RosterPanel->GetChildrenCount(); ++Index)
	{
		if (UOperatorCardWidget* OperatorCard = Cast<UOperatorCardWidget>(RosterPanel->GetChildAt(Index)))
		{
			OperatorCard->RefreshCardState();
		}
	}
}
