// Fill out your copyright notice in the Description page of Project Settings.

#include "CraftingPanel.h"

#include "Components/Button.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/WrapBox.h"
#include "Engine/DataTable.h"
#include "InfrastructureIcon.h"
#include "../HexMap/Shop/Icon.h"
#include "../URougeliteRunSubsystem.h"
#include "../RougeliteSettings.h"

void UCraftingPanel::NativeConstruct()
{
	Super::NativeConstruct();

	RunSubsystem = GetGameInstance() ? GetGameInstance()->GetSubsystem<URougeliteRunSubsystem>() : nullptr;

	if (IncreaseButton)
	{
		IncreaseButton->OnClicked.AddDynamic(this, &UCraftingPanel::OnIncreaseClicked);
	}
	if (DecreaseButton)
	{
		DecreaseButton->OnClicked.AddDynamic(this, &UCraftingPanel::OnDecreaseClicked);
	}
	if (ConfirmButton)
	{
		ConfirmButton->OnClicked.AddDynamic(this, &UCraftingPanel::OnConfirmClicked);
	}
	if (ExitButton)
	{
		ExitButton->OnClicked.AddDynamic(this, &UCraftingPanel::Close);
	}

	SetVisibility(ESlateVisibility::Collapsed);
}

void UCraftingPanel::Open()
{
	PopulateResourceList();
	bHasValidRecipe = false;
	CurrentProductQty = 0;
	SetIcon();
	SetQuantityTexts();
	SetVisibility(ESlateVisibility::Visible);
}

void UCraftingPanel::Close()
{
	SetVisibility(ESlateVisibility::Collapsed);

	if (IconsContainer)
	{
		IconsContainer->ClearChildren();
	}
	ResourceIcons.Empty();
}

void UCraftingPanel::PopulateResourceList()
{
	if (!IconsContainer || !IconWidgetClass || !RunSubsystem)
	{
		return;
	}

	IconsContainer->ClearChildren();
	IconsContainer->SetInnerSlotPadding(SlotPadding);
	ResourceIcons.Empty();


	if (!RunSubsystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("CraftingPanel::PopulateResourceList: RunSubsystem is null"));
		return;
	}

	for (const TPair<EResourceType, int32>& Pair : RunSubsystem->GlobalResource)
	{
		if (!EResourceTypeMeta::IsStorable(Pair.Key))
		{
			continue;
		}
		if (Pair.Value <= 0)
		{
			continue;
		}

		UInfrastructureIcon* IconWidget = CreateWidget<UInfrastructureIcon>(this, IconWidgetClass);
		if (!IconWidget)
		{
			continue;
		}

		IconWidget->SetResourceType(Pair.Key);
		IconWidget->SetQuantity(Pair.Value);
		IconWidget->OnIconClicked.AddDynamic(this, &UCraftingPanel::OnResourceSelected);

		USizeBox* SizeBox = NewObject<USizeBox>(this);
		SizeBox->SetWidthOverride(IconSize.X);
		SizeBox->SetHeightOverride(IconSize.Y);
		SizeBox->AddChild(IconWidget);

		IconsContainer->AddChildToWrapBox(SizeBox);
		ResourceIcons.Add(IconWidget);
	}
}

void UCraftingPanel::SetIcon()
{
	if(!bHasValidRecipe)
	{
		CostIcon->SetNone();
		ProductIcon->SetNone();
		return;
	}
		
	CostIcon->SetResourceType(CurrentRecipe.InputType);
	ProductIcon->SetResourceType(CurrentRecipe.OutputType);	
}

void UCraftingPanel::SetQuantityTexts()
{
	if (!bHasValidRecipe)
	{
		CostQuantityText->SetText(FText::AsNumber(0));
		ProductQuantityText->SetText(FText::AsNumber(0));
		DecreaseButton->SetIsEnabled(false);
		IncreaseButton->SetIsEnabled(false);
		ConfirmButton->SetIsEnabled(false);
		return;
	}

	const int32 CostQty = CurrentProductQty * CurrentRecipe.InputPerOutput;
	CostQuantityText->SetText(FText::AsNumber(CostQty));
	ProductQuantityText->SetText(FText::AsNumber(CurrentProductQty));

	if(CurrentProductQty <= 0)
	{
		DecreaseButton->SetIsEnabled(false);
		ConfirmButton->SetIsEnabled(false);
	}
	else
	{
		DecreaseButton->SetIsEnabled(true);
		ConfirmButton->SetIsEnabled(true);
	}

	const int32 Owned = RunSubsystem ? RunSubsystem->GetGlobalResourceAmount(CurrentRecipe.InputType) : 0;
	const int32 MaxProduct = Owned / CurrentRecipe.InputPerOutput;
	if(CurrentProductQty >= MaxProduct)
	{
		IncreaseButton->SetIsEnabled(false);
	}
	else
	{
		IncreaseButton->SetIsEnabled(true);
	}
}

void UCraftingPanel::OnResourceSelected(EResourceType Type)
{
	SelectedInputType = Type;

	bHasValidRecipe = false;
	const URougeliteSettings* Settings = GetDefault<URougeliteSettings>();
	UDataTable* Table = Settings ? Settings->CraftRecipeTable.LoadSynchronous() : nullptr;
	if (Table)
	{
		TArray<FInfrastructureCraftRecipe*> AllRows;
		Table->GetAllRows<FInfrastructureCraftRecipe>(TEXT("CraftingPanel"), AllRows);
		for (const FInfrastructureCraftRecipe* Row : AllRows)
		{
			if (Row && Row->InputType == Type)
			{
				CurrentRecipe = *Row;
				bHasValidRecipe = true;
				break;
			}
		}

	}

	CurrentProductQty = 0;
	SetIcon();
	SetQuantityTexts();
}

void UCraftingPanel::OnIncreaseClicked()
{
	if (!bHasValidRecipe)
	{
		return;
	}

	CurrentProductQty++;
	SetQuantityTexts();
}

void UCraftingPanel::OnDecreaseClicked()
{
	if (!bHasValidRecipe)
	{
		return;
	}

	CurrentProductQty--;
	SetQuantityTexts();
}

void UCraftingPanel::OnConfirmClicked()
{
	if (!bHasValidRecipe || CurrentProductQty <= 0 || !RunSubsystem)
	{
		return;
	}

	const int32 CostQty = CurrentProductQty * CurrentRecipe.InputPerOutput;

	if (!RunSubsystem->TryConsumeResource(CurrentRecipe.InputType, CostQty))
	{
		UE_LOG(LogTemp, Warning, TEXT("CraftingPanel::OnConfirmClicked: insufficient resources"));
		return;
	}

	RunSubsystem->AddResource(CurrentRecipe.OutputType, CurrentProductQty);

	PopulateResourceList();

	bHasValidRecipe = false;
	CurrentProductQty = 0;
	
	SetIcon();
	SetQuantityTexts();
}