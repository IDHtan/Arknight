// Fill out your copyright notice in the Description page of Project Settings.

#include "MainMenuHUDWidget.h"

#include "Components/Button.h"
#include "BaseBuildingPanel.h"
#include "GamePrepPanel.h"

void UMainMenuHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CollapseAllPanels();

	if (OpenBaseBuildingButton)
	{
		OpenBaseBuildingButton->OnClicked.AddDynamic(this, &UMainMenuHUDWidget::OnBaseBuildingButtonClicked);
	}
	if (OpenGamePrepButton)
	{
		OpenGamePrepButton->OnClicked.AddDynamic(this, &UMainMenuHUDWidget::OnGamePrepButtonClicked);
	}
}

void UMainMenuHUDWidget::CollapseAllPanels()
{
	if (BaseBuildingPanel)
	{
		BaseBuildingPanel->Close();
	}
	if (GamePrepPanel)
	{
		GamePrepPanel->Close();
	}
}

void UMainMenuHUDWidget::OnBaseBuildingButtonClicked()
{
	CollapseAllPanels();
	if(BaseBuildingPanel)
	{
		BaseBuildingPanel->Open();
	}
}

void UMainMenuHUDWidget::OnGamePrepButtonClicked()
{
	CollapseAllPanels();
	if(GamePrepPanel)
	{
		GamePrepPanel->Open();
	}
}
