// Fill out your copyright notice in the Description page of Project Settings.


#include "HexMapHUDWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/TextBlock.h"
#include "../Controller/HexMapSubsystem.h"
#include "HexNodeWidget.h"

void UHexMapHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	NodeWidgets.Reset();

	if (InventoryPanel)
	{
		InventoryPanel->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (EventPanel)
	{
		EventPanel->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (ShopPanel)
	{
		ShopPanel->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (TeleportPanel)
	{
		TeleportPanel->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (ExitPanel)
	{
		ExitPanel->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (WidgetTree)
	{
		TArray<UWidget*> AllWidgets;
		WidgetTree->GetAllWidgets(AllWidgets);
		for (UWidget* Widget : AllWidgets)
		{
			if (UHexNodeWidget* NodeWidget = Cast<UHexNodeWidget>(Widget))
			{
				NodeWidgets.Add(NodeWidget);
			}
		}
	}

	HexMapSubsystemP = GetGameInstance() ? GetGameInstance()->GetSubsystem<UHexMapSubsystem>() : nullptr;
	if (HexMapSubsystemP)
	{
		HexMapSubsystemP->OnHexNodeStatesRefreshed.AddDynamic(this, &UHexMapHUDWidget::OnHexNodeStatesRefreshed);
		HexMapSubsystemP->OnAPChanged.AddDynamic(this, &UHexMapHUDWidget::UpdateCurrentAP);

		for (UHexNodeWidget* NodeWidget : NodeWidgets)
		{
			if (!NodeWidget)
			{
				continue;
			}
			NodeWidget->InitWidgetData(HexMapSubsystemP->GetNodeInfo(NodeWidget->LogicalCoordinate));
		}

		UpdateCurrentAP(HexMapSubsystemP->CurrentAP);
	}

	if (OpenBackpackButton)
	{
		OpenBackpackButton->OnClicked.AddDynamic(this, &UHexMapHUDWidget::OnBackpackButtonClicked);
	}
	if (ExitButton)
	{
		ExitButton->OnClicked.AddDynamic(this, &UHexMapHUDWidget::OnExitButtonClicked);
	}

	OnHexNodeStatesRefreshed();
}

void UHexMapHUDWidget::OnHexNodeStatesRefreshed()
{
	RefreshAllNodesAppearance();
}

void UHexMapHUDWidget::UpdateCurrentAP(int32 NewAP)
{
	if (CurrentAP)
	{
		CurrentAP->SetText(FText::AsNumber(NewAP));
	}
}

void UHexMapHUDWidget::RefreshAllNodesAppearance()
{
	for (UHexNodeWidget* NodeWidget : NodeWidgets)
	{
		if (NodeWidget)
		{
			NodeWidget->UpdateVisual();
		}
	}
}

void UHexMapHUDWidget::OpenEventPanel_Implementation(const FHexEventConfig& Config)
{
	if (!EventPanel)
	{
		return;
	}

	EventPanel->SetVisibility(ESlateVisibility::Visible);

	if (UFunction* OpenEventFunction = EventPanel->FindFunction(TEXT("OpenEvent")))
	{
		struct FOpenEventParams
		{
			FHexEventConfig Config;
		};
		FOpenEventParams Params{ Config };
		EventPanel->ProcessEvent(OpenEventFunction, &Params);
	}
}

void UHexMapHUDWidget::OpenShopPanel_Implementation()
{
	if (!ShopPanel)
	{
		return;
	}

	ShopPanel->SetVisibility(ESlateVisibility::Visible);

	if (UFunction* OpenShopFunction = ShopPanel->FindFunction(TEXT("OpenShop")))
	{
		ShopPanel->ProcessEvent(OpenShopFunction, nullptr);
	}
}

void UHexMapHUDWidget::OpenTeleportPanel_Implementation()
{
	if (!TeleportPanel)
	{
		return;
	}

	TeleportPanel->SetVisibility(ESlateVisibility::Visible);

	if (UFunction* InitWidgetDataFunction = TeleportPanel->FindFunction(TEXT("InitWidgetData")))
	{
		struct FInitTeleportParams
		{
			EHexRegionType CurrentRegion;
		};
		const EHexRegionType Region = HexMapSubsystemP ? HexMapSubsystemP->CurrentRegion : EHexRegionType::Wood;
		FInitTeleportParams Params{ Region };
		TeleportPanel->ProcessEvent(InitWidgetDataFunction, &Params);
	}
}

void UHexMapHUDWidget::OnBackpackButtonClicked()
{
	if (!InventoryPanel)
	{
		return;
	}

	InventoryPanel->SetVisibility(ESlateVisibility::Visible);

	if (UFunction* OpenInventoryFunction = InventoryPanel->FindFunction(TEXT("OpenInventory")))
	{
		struct FOpenInventoryParams
		{
			bool bInSelectionMode;
		};
		FOpenInventoryParams Params{ false };
		InventoryPanel->ProcessEvent(OpenInventoryFunction, &Params);
	}
}

void UHexMapHUDWidget::OnExitButtonClicked()
{
	if (!ExitPanel)
	{
		return;
	}

	ExitPanel->SetVisibility(ESlateVisibility::Visible);

	if (UFunction* OpenFunction = ExitPanel->FindFunction(TEXT("Open")))
	{
		ExitPanel->ProcessEvent(OpenFunction, nullptr);
	}
}

