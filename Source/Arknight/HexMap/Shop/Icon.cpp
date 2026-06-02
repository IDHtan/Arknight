// Fill out your copyright notice in the Description page of Project Settings.


#include "Icon.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Engine/DataTable.h"
#include "../../RougeliteSettings.h"

void UIcon::NativeConstruct()
{
	Super::NativeConstruct();

	if (IconButton)
	{
		IconButton->OnClicked.AddDynamic(this, &UIcon::OnIconButtonClicked);

		// Fully transparent across all states — visual handled by IconImage
		FButtonStyle Style = IconButton->WidgetStyle;
		const FSlateColor Transparent = FSlateColor(FLinearColor(1.0f, 1.0f, 1.0f, 0.0f));
		Style.Normal.TintColor = Transparent;
		Style.Hovered.TintColor = Transparent;
		Style.Pressed.TintColor = Transparent;
		Style.Disabled.TintColor = Transparent;
		IconButton->SetStyle(Style);
	}

	// Editor pre-configuration: load icon if type was set, else clear to empty
	if (bHasLinkedResourceType)
	{
		Update();
	}
	else
	{
		if (IconImage)
		{
			IconImage->SetBrushFromTexture(nullptr);
		}
		if (NameText)
		{
			NameText->SetText(FText::GetEmpty());
		}
	}
}

void UIcon::SetResourceType(EResourceType Type)
{
	bHasLinkedResourceType = true;
	LinkedResourceType = Type;
	Update();
}

void UIcon::Update()
{
	if (!bHasLinkedResourceType)
	{
		return;
	}

	const URougeliteSettings* Settings = GetDefault<URougeliteSettings>();
	UDataTable* IconTable = Settings ? Settings->IconImageTable.LoadSynchronous() : nullptr;
	if (!IconTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("Icon::Update: IconImageTable not found in settings"));
		return;
	}

	const UEnum* EnumPtr = StaticEnum<EResourceType>();
	const FName RowName = FName(*EnumPtr->GetNameStringByValue(static_cast<int64>(LinkedResourceType)));

	const FIconImageRow* Row = IconTable->FindRow<FIconImageRow>(RowName, TEXT("Icon::Update"));
	if (Row)
	{
		if (IconImage)
		{
			IconImage->SetBrushFromSoftTexture(Row->IconTexture);
		}
		if (NameText)
		{
			const FText DisplayName = EnumPtr->GetDisplayNameTextByValue(static_cast<int64>(LinkedResourceType));
			NameText->SetText(DisplayName);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Icon::Update: Row '%s' not found in IconImageTable"), *RowName.ToString());
	}
}

void UIcon::OnIconButtonClicked()
{
	// Pure click signal — always broadcast, regardless of type binding
	OnIconButtonClicked.Broadcast();

	// Typed click signal — only broadcast if a resource type is linked
	if (bHasLinkedResourceType)
	{
		OnIconClicked.Broadcast(LinkedResourceType);
	}
}
