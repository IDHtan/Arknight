// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/WrapBox.h"
#include "Engine/DataTable.h"
#include "../Controller/HexMapSubsystem.h"
#include "../Shop/Icon.h"
#include "../../RougeliteSettings.h"

void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ExitButton)
	{
		ExitButton->OnClicked.AddDynamic(this, &UInventoryWidget::CloseInventory);
	}
	if (ConfirmButton)
	{
		ConfirmButton->OnClicked.AddDynamic(this, &UInventoryWidget::OnConfirmClicked);
	}

	SetVisibility(ESlateVisibility::Collapsed);
}

void UInventoryWidget::OpenInventory(bool bInSelectionMode)
{
	bIsSelectionMode = bInSelectionMode;
	bHasSelectedResource = false;

	if (IconsContainer)
	{
		IconsContainer->ClearChildren();
		IconsContainer->SetInnerSlotPadding(SlotPadding);
	}
	ResourceButtons.Empty();

	// Read current resources from subsystem
	UHexMapSubsystem* HexMapSubsystem = GetGameInstance() ? GetGameInstance()->GetSubsystem<UHexMapSubsystem>() : nullptr;
	if (!HexMapSubsystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryWidget::OpenInventory: HexMapSubsystem is null"));
		return;
	}

	if (!IconWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryWidget::OpenInventory: IconWidgetClass is not set"));
		return;
	}

	const TMap<EResourceType, int32>& Resources = HexMapSubsystem->GetAllResources();
	for (const TPair<EResourceType, int32>& Pair : Resources)
	{
		// Skip resources that shouldn't appear in the backpack (e.g. AP)
		if (!EResourceTypeMeta::IsStorable(Pair.Key))
		{
			continue;
		}

		if (Pair.Value <= 0)
		{
			continue;
		}

		UIcon* IconWidget = CreateWidget<UIcon>(this, IconWidgetClass);
		if (!IconWidget)
		{
			UE_LOG(LogTemp, Warning, TEXT("InventoryWidget::OpenInventory: failed to create Icon widget for resource %d"), static_cast<int32>(Pair.Key));
			continue;
		}

		IconWidget->SetResourceType(Pair.Key);
		IconWidget->OnIconClicked.AddDynamic(this, &UInventoryWidget::OnResourceButtonClicked);

		// WrapBox → SizeBox → Icon: force uniform icon size, auto-wrap by WrapBox
		USizeBox* SizeBox = NewObject<USizeBox>(this);
		SizeBox->SetWidthOverride(IconSize.X);
		SizeBox->SetHeightOverride(IconSize.Y);
		SizeBox->AddChild(IconWidget);

		if (IconsContainer)
		{
			IconsContainer->AddChildToWrapBox(SizeBox);
			ResourceButtons.Add(IconWidget);
		}
	}

	if (ConfirmButton)
	{
		ConfirmButton->SetVisibility(bIsSelectionMode ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
		ConfirmButton->SetIsEnabled(false);
	}

	if (ResourceIcon)
	{
		ResourceIcon->SetBrushFromTexture(nullptr);
	}
	if (ResourceNameText)
	{
		ResourceNameText->SetText(FText::GetEmpty());
	}
	if (ResourceNumberText)
	{
		ResourceNumberText->SetText(FText::GetEmpty());
	}

	SetVisibility(ESlateVisibility::Visible);
}

void UInventoryWidget::OnResourceButtonClicked(EResourceType Type)
{
	CurrentSelectedResource = Type;
	bHasSelectedResource = true;

	UHexMapSubsystem* HexMapSubsystem = GetGameInstance() ? GetGameInstance()->GetSubsystem<UHexMapSubsystem>() : nullptr;
	const int32 Quantity = HexMapSubsystem ? HexMapSubsystem->GetResource(Type) : 0;

	if (ResourceNameText)
	{
		const UEnum* EnumPtr = StaticEnum<EResourceType>();
		ResourceNameText->SetText(EnumPtr->GetDisplayNameTextByValue(static_cast<int64>(Type)));
	}
	if (ResourceNumberText)
	{
		ResourceNumberText->SetText(FText::AsNumber(Quantity));
	}
	if (ResourceIcon)
	{
		const URougeliteSettings* Settings = GetDefault<URougeliteSettings>();
		UDataTable* IconTable = Settings ? Settings->IconImageTable.LoadSynchronous() : nullptr;
		if (IconTable)
		{
			const UEnum* EnumPtr = StaticEnum<EResourceType>();
			const FName RowName = FName(*EnumPtr->GetNameStringByValue(static_cast<int64>(Type)));
			const FIconImageRow* Row = IconTable->FindRow<FIconImageRow>(RowName, TEXT("InventoryWidget"));
			if (Row)
			{
				ResourceIcon->SetBrushFromSoftTexture(Row->IconTexture);
			}
		}
	}

	// Enable confirm in selection mode
	if (bIsSelectionMode && ConfirmButton)
	{
		ConfirmButton->SetIsEnabled(true);
	}
}

void UInventoryWidget::OnConfirmClicked()
{
	if (!bIsSelectionMode || !bHasSelectedResource)
	{
		return;
	}

	OnResourceConfirmed.Broadcast(CurrentSelectedResource);
	CloseInventory();
}

void UInventoryWidget::CloseInventory()
{
	SetVisibility(ESlateVisibility::Collapsed);

	// Clean up dynamically spawned icons
	if (IconsContainer)
	{
		IconsContainer->ClearChildren();
	}
	ResourceButtons.Empty();

	bHasSelectedResource = false;
	bIsSelectionMode = false;
}
