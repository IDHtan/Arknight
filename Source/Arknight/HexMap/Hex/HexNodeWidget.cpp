// Fill out your copyright notice in the Description page of Project Settings.


#include "HexNodeWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Engine/DataTable.h"
#include "../Controller/HexMapPlayerController.h"
#include "../Controller/HexMapSubsystem.h"
#include "../../RougeliteSettings.h"

void UHexNodeWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (NodeButton)
	{
		NodeButton->OnClicked.AddDynamic(this, &UHexNodeWidget::OnClicked);
	}
}

void UHexNodeWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	UHexMapSubsystem* HexMapSubsystem = GetGameInstance() ? GetGameInstance()->GetSubsystem<UHexMapSubsystem>() : nullptr;
	if (!HexMapSubsystem)
	{
		return;
	}

	const FHexNodeData Data = HexMapSubsystem->GetNodeInfo(LogicalCoordinate);
	if (Data.NodeState == EHexNodeState::Reachable)
	{
		SetRenderScale(FVector2D(HoverScale, HoverScale));
	}
}

void UHexNodeWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	SetRenderScale(FVector2D(1.0f, 1.0f));
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

	// Load icon table from settings
	const URougeliteSettings* Settings = GetDefault<URougeliteSettings>();
	UDataTable* IconTable = Settings ? Settings->IconImageTable.LoadSynchronous() : nullptr;
	if (!IconTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("HexNodeWidget::UpdateVisual: IconImageTable not found in settings"));
		return;
	}

	if (Data.NodeState == EHexNodeState::Masked)
	{
		const FIconImageRow* Row = IconTable->FindRow<FIconImageRow>(FName(TEXT("Masked")), TEXT("UpdateVisual"));
		if (Row)
		{
			NodeIcon->SetBrushFromSoftTexture(Row->IconTexture);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("HexNodeWidget::UpdateVisual: 'Masked' row not found in IconImageTable"));
		}
		NodeButton->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	// DataTable row names must match enum value names, e.g. "Combat_Normal", "Event", "Shop"
	if (NodeIcon && IconTable)
	{
		const UEnum* EnumPtr = StaticEnum<EHexNodeType>();
		const FName RowName = FName(*EnumPtr->GetNameStringByValue(static_cast<int64>(Data.NodeType)));
		const FIconImageRow* Row = IconTable->FindRow<FIconImageRow>(RowName, TEXT("UpdateVisual"));
		if (Row)
		{
			NodeIcon->SetBrushFromSoftTexture(Row->IconTexture);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("HexNodeWidget::UpdateVisual: Row '%s' not found in IconImageTable"), *RowName.ToString());
		}
	}

	if (NodeIcon)
	{
		FLinearColor StateTint = FLinearColor::White;
		switch (Data.NodeState)
		{
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
			break;
		}

		NodeIcon->SetColorAndOpacity(StateTint);
	}

	if (NodeButton)
	{
		if (Data.NodeState == EHexNodeState::Reachable)
		{
			NodeButton->SetVisibility(ESlateVisibility::Visible);

			FButtonStyle Style = NodeButton->GetStyle();
			const FSlateColor Transparent = FSlateColor(FLinearColor(1.0f, 1.0f, 1.0f, 0.0f));
			Style.Normal.TintColor = Transparent;
			Style.Hovered.TintColor = Transparent;
			Style.Pressed.TintColor = Transparent;
			Style.Disabled.TintColor = Transparent;
			NodeButton->SetStyle(Style);
		}
		else
		{
			NodeButton->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UHexNodeWidget::OnClicked()
{
	AHexMapPlayerController* HexPC = Cast<AHexMapPlayerController>(GetOwningPlayer());
	if (!HexPC)
	{
		UE_LOG(LogTemp, Warning, TEXT("HexNodeWidget::OnClicked: Owning player is not AHexMapPlayerController"));
		return;
	}

	HexPC->HandleNodeClicked(LogicalCoordinate);
}

