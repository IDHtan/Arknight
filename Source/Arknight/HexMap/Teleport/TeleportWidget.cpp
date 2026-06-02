// Fill out your copyright notice in the Description page of Project Settings.


#include "TeleportWidget.h"

#include "Components/Button.h"
#include "../Controller/HexMapSubsystem.h"

void UTeleportWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (TeleportButton1)
	{
		TeleportButton1->OnClicked.AddDynamic(this, &UTeleportWidget::On1Clicked);
	}
	if (TeleportButton2)
	{
		TeleportButton2->OnClicked.AddDynamic(this, &UTeleportWidget::On2Clicked);
	}
	if (TeleportButton3)
	{
		TeleportButton3->OnClicked.AddDynamic(this, &UTeleportWidget::On3Clicked);
	}
	if (TeleportButton4)
	{
		TeleportButton4->OnClicked.AddDynamic(this, &UTeleportWidget::On4Clicked);
	}
	if (ConfirmButton)
	{
		ConfirmButton->OnClicked.AddDynamic(this, &UTeleportWidget::OnConfirmClicked);
		ConfirmButton->SetIsEnabled(false);
	}

	SetVisibility(ESlateVisibility::Collapsed);
}

void UTeleportWidget::InitWidgetData(EHexRegionType CurrentRegion)
{
	ClickedRegionType = CurrentRegion;

	// Reset all buttons to default scale
	if (TeleportButton1) TeleportButton1->SetRenderScale(FVector2D(1.0f, 1.0f));
	if (TeleportButton2) TeleportButton2->SetRenderScale(FVector2D(1.0f, 1.0f));
	if (TeleportButton3) TeleportButton3->SetRenderScale(FVector2D(1.0f, 1.0f));
	if (TeleportButton4) TeleportButton4->SetRenderScale(FVector2D(1.0f, 1.0f));

	// Hide the button for the current region (cannot teleport to self)
	if (TeleportButton1) TeleportButton1->SetVisibility(CurrentRegion == EHexRegionType::Wood   ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
	if (TeleportButton2) TeleportButton2->SetVisibility(CurrentRegion == EHexRegionType::Rock   ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
	if (TeleportButton3) TeleportButton3->SetVisibility(CurrentRegion == EHexRegionType::Metal  ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
	if (TeleportButton4) TeleportButton4->SetVisibility(CurrentRegion == EHexRegionType::Grain  ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);

	// Disable confirm until a new region is selected
	if (ConfirmButton)
	{
		ConfirmButton->SetIsEnabled(false);
	}
}

void UTeleportWidget::On1Clicked()
{
	OnOneButtonClicked(EHexRegionType::Wood);
}

void UTeleportWidget::On2Clicked()
{
	OnOneButtonClicked(EHexRegionType::Rock);
}

void UTeleportWidget::On3Clicked()
{
	OnOneButtonClicked(EHexRegionType::Metal);
}

void UTeleportWidget::On4Clicked()
{
	OnOneButtonClicked(EHexRegionType::Grain);
}

void UTeleportWidget::OnOneButtonClicked(EHexRegionType RegionType)
{
	ClickedRegionType = RegionType;

	// Highlight selected: scale up; others: reset to normal
	const FVector2D SelectedScale(1.2f, 1.2f);
	const FVector2D DefaultScale(1.0f, 1.0f);

	if (TeleportButton1) TeleportButton1->SetRenderScale(RegionType == EHexRegionType::Wood   ? SelectedScale : DefaultScale);
	if (TeleportButton2) TeleportButton2->SetRenderScale(RegionType == EHexRegionType::Rock   ? SelectedScale : DefaultScale);
	if (TeleportButton3) TeleportButton3->SetRenderScale(RegionType == EHexRegionType::Metal  ? SelectedScale : DefaultScale);
	if (TeleportButton4) TeleportButton4->SetRenderScale(RegionType == EHexRegionType::Grain  ? SelectedScale : DefaultScale);

	if (ConfirmButton)
	{
		ConfirmButton->SetIsEnabled(true);
	}
}

void UTeleportWidget::OnConfirmClicked()
{
	UHexMapSubsystem* HexMapSubsystem = GetGameInstance() ? GetGameInstance()->GetSubsystem<UHexMapSubsystem>() : nullptr;
	if (!HexMapSubsystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("TeleportWidget::OnConfirmClicked: HexMapSubsystem is null"));
		return;
	}

	if (ClickedRegionType == HexMapSubsystem->CurrentRegion)
	{
		UE_LOG(LogTemp, Warning, TEXT("TeleportWidget::OnConfirmClicked: Cannot teleport to current region"));
		return;
	}

	HexMapSubsystem->EnterRegion(ClickedRegionType);
	SetVisibility(ESlateVisibility::Collapsed);
}
