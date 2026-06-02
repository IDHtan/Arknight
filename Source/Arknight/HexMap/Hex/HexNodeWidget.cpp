// Fill out your copyright notice in the Description page of Project Settings.


#include "HexNodeWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "../Controller/HexMapPlayerController.h"
#include "../Controller/HexMapSubsystem.h"

void UHexNodeWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (NodeButton)
	{
		NodeButton->OnClicked.AddDynamic(this, &UHexNodeWidget::OnClicked);
	}

	UpdateVisual();
}

void UHexNodeWidget::InitWidgetData(const FHexNodeData& Data)
{
	LogicalCoordinate = Data.GridCoordinate;
	UpdateVisual();
}

void UHexNodeWidget::UpdateVisual()
{
	UHexMapSubsystem* HexMapSubsystem = GetGameInstance() ? GetGameInstance()->GetSubsystem<UHexMapSubsystem>() : nullptr;
	if (!HexMapSubsystem)
	{
		return;
	}

	const FHexNodeData Data = HexMapSubsystem->GetNodeInfo(LogicalCoordinate);

	if (FogImage)
	{
		FogImage->SetVisibility(Data.NodeState == EHexNodeState::Masked ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}

	if (NodeButton)
	{
		NodeButton->SetIsEnabled(Data.NodeState == EHexNodeState::Reachable);
	}

	if (NodeIcon)
	{
		FLinearColor TypeTint = FLinearColor::White;
		switch (Data.NodeType)
		{
		case EHexNodeType::Entrance:
			TypeTint = FLinearColor(0.9f, 0.9f, 1.0f, 1.0f);
			break;
		case EHexNodeType::Combat_Normal:
			TypeTint = FLinearColor(1.0f, 0.4f, 0.4f, 1.0f);
			break;
		case EHexNodeType::Combat_Emergency:
			TypeTint = FLinearColor(1.0f, 0.1f, 0.1f, 1.0f);
			break;
		case EHexNodeType::Event:
			TypeTint = FLinearColor(0.6f, 0.8f, 1.0f, 1.0f);
			break;
		case EHexNodeType::Reward:
			TypeTint = FLinearColor(0.8f, 1.0f, 0.6f, 1.0f);
			break;
		case EHexNodeType::Shop:
			TypeTint = FLinearColor(0.2f, 0.7f, 1.0f, 1.0f);
			break;
		case EHexNodeType::Teleport:
			TypeTint = FLinearColor(0.7f, 0.4f, 1.0f, 1.0f);
			break;
		default:
			TypeTint = FLinearColor::White;
			break;
		}

		FLinearColor StateTint = FLinearColor::White;
		switch (Data.NodeState)
		{
		case EHexNodeState::Masked:
			StateTint = FLinearColor(0.35f, 0.35f, 0.35f, 0.4f);
			break;
		case EHexNodeState::Unmasked:
			StateTint = FLinearColor(0.8f, 0.8f, 0.8f, 1.0f);
			break;
		case EHexNodeState::Reachable:
			StateTint = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
			break;
		case EHexNodeState::Cleared:
			StateTint = FLinearColor(0.5f, 0.5f, 0.5f, 0.7f);
			break;
		default:
			StateTint = FLinearColor::White;
			break;
		}

		NodeIcon->SetColorAndOpacity(TypeTint * StateTint);
	}
}

void UHexNodeWidget::OnClicked()
{
	AHexMapPlayerController* HexPC = Cast<AHexMapPlayerController>(GetOwningPlayer());
	if (!HexPC)
	{
		return;
	}

	HexPC->HandleNodeClicked(LogicalCoordinate);
}

