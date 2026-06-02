// Fill out your copyright notice in the Description page of Project Settings.


#include "HexExitWidget.h"

#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "../Controller/HexMapSubsystem.h"

void UHexExitWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ConfirmButton)
	{
		ConfirmButton->OnClicked.AddDynamic(this, &UHexExitWidget::OnConfirmClicked);
	}
	if (CancelButton)
	{
		CancelButton->OnClicked.AddDynamic(this, &UHexExitWidget::OnCancelClicked);
	}

	SetVisibility(ESlateVisibility::Collapsed);
}

void UHexExitWidget::Open()
{
	SetVisibility(ESlateVisibility::Visible);
}

void UHexExitWidget::OnConfirmClicked()
{
	UHexMapSubsystem* HexMapSubsystem = GetGameInstance() ? GetGameInstance()->GetSubsystem<UHexMapSubsystem>() : nullptr;
	if (HexMapSubsystem)
	{
		HexMapSubsystem->ConcludeGame();
	}
}

void UHexExitWidget::OnCancelClicked()
{
	SetVisibility(ESlateVisibility::Collapsed);
}
