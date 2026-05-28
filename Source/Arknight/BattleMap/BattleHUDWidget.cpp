// Fill out your copyright notice in the Description page of Project Settings.


#include "../BattleMap/BattleHUDWidget.h"
#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "Kismet/GameplayStatics.h"
#include "ExitWidget.h"

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

	bIsGamePaused = false;
	CurrentSpeed = EGameSpeedState::Speed_1x;

	if(ExitConfirmPanel)
	{
		ExitConfirmPanel->SetVisibility(ESlateVisibility::Collapsed);
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

