// Fill out your copyright notice in the Description page of Project Settings.


#include "ShopWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Icon.h"
#include "ShopTransactionSubPanel.h"
#include "../Controller/HexMapSubsystem.h"

void UShopWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Collect pre-configured resource icons via WidgetTree scan.
	// Icons with bHasLinkedResourceType=true were pre-set in UMG designer
	// with their specific resource type. APButton has bHasLinkedResourceType=false.
	if (WidgetTree)
	{
		TArray<UWidget*> AllWidgets;
		WidgetTree->GetAllWidgets(AllWidgets);
		for (UWidget* Widget : AllWidgets)
		{
			UIcon* Icon = Cast<UIcon>(Widget);
			if (Icon && Icon->bHasLinkedResourceType)
			{
				ResourceButtons.Add(Icon);
				Icon->OnIconClicked.AddDynamic(this, &UShopWidget::OnResourceButtonClicked);
			}
		}
	}

	if (ExitButton)
	{
		ExitButton->OnClicked.AddDynamic(this, &UShopWidget::CloseShop);
	}

	HexMapSubsystemP = GetGameInstance() ? GetGameInstance()->GetSubsystem<UHexMapSubsystem>() : nullptr;

	if (TransactionPanel)
	{
		TransactionPanel->SetVisibility(ESlateVisibility::Collapsed);
	}

	SetVisibility(ESlateVisibility::Collapsed);
}

void UShopWidget::OpenShop()
{
	if (!HexMapSubsystemP)
	{
		UE_LOG(LogTemp, Warning, TEXT("ShopWidget::OpenShop: HexMapSubsystemP is null"));
		return;
	}

	if (APButton && (!APButton->bHasLinkedResourceType || APButton->LinkedResourceType != EResourceType::AP))
	{
		APButton->SetResourceType(EResourceType::AP);
		APButton->OnIconClicked.AddDynamic(this, &UShopWidget::OnAPButtonClicked);
	}

	for (UIcon* Icon : ResourceButtons)
	{
		if (Icon)
		{
			Icon->Update();
		}
	}

	if (TransactionPanel)
	{
		TransactionPanel->SetVisibility(ESlateVisibility::Collapsed);
	}

	SetVisibility(ESlateVisibility::Visible);
}

void UShopWidget::OnResourceButtonClicked(EResourceType TargetResourceType)
{
	if (!TransactionPanel)
	{
		return;
	}

	TransactionPanel->ShowTransaction(TargetResourceType);
	TransactionPanel->SetVisibility(ESlateVisibility::Visible);
}

void UShopWidget::OnAPButtonClicked()
{
	if (!TransactionPanel)
	{
		return;
	}

	TransactionPanel->ShowAPTransaction();
	TransactionPanel->SetVisibility(ESlateVisibility::Visible);
}

void UShopWidget::CloseShop()
{
	SetVisibility(ESlateVisibility::Collapsed);
}
