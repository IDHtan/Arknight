// Fill out your copyright notice in the Description page of Project Settings.


#include "ExitWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "../../HexMap/Controller/HexMapSubsystem.h"
#include "../Controller/BattlePlayerController.h"
#include "BattleHUDWidget.h"

void UExitWidget::NativeConstruct()
{
    Super::NativeConstruct();
    if (ConfirmButton) ConfirmButton->OnClicked.AddDynamic(this, &UExitWidget::OnConfirmClicked);
    if (CancelButton) CancelButton->OnClicked.AddDynamic(this, &UExitWidget::OnCancelClicked);

    SetVisibility(ESlateVisibility::Collapsed);
}

void UExitWidget::Open()
{
	UE_LOG(LogTemp, Log, TEXT("Opening Exit Widget"));
    SetVisibility(ESlateVisibility::Visible);
	UGameplayStatics::SetGamePaused(GetWorld(), true);
}

void UExitWidget::OnConfirmClicked()
{
    UE_LOG(LogTemp, Log, TEXT("Confirm button clicked"));
	UHexMapSubsystem* HexMapSubsystem = GetGameInstance()->GetSubsystem<UHexMapSubsystem>();
    if (HexMapSubsystem)
    {
        HexMapSubsystem->ConcludeBattle();
    }
}

void UExitWidget::OnCancelClicked()
{
    UE_LOG(LogTemp, Log, TEXT("Cancel button clicked"));
    SetVisibility(ESlateVisibility::Collapsed);
    ABattlePlayerController* BattlePC = Cast<ABattlePlayerController>(GetOwningPlayer());
    if (BattlePC && BattlePC->HUDWidgetInstance)
    {
        UGameplayStatics::SetGamePaused(GetWorld(), BattlePC->HUDWidgetInstance->bIsGamePaused);
    }
    else
    {
        UGameplayStatics::SetGamePaused(GetWorld(), false);
    }
}
