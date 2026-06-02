// Fill out your copyright notice in the Description page of Project Settings.


#include "EventOptionWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

void UEventOptionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (OptionButton)
	{
		OptionButton->OnClicked.AddDynamic(this, &UEventOptionWidget::OnClicked);
	}
}

void UEventOptionWidget::InitOption(const FHexEventOption& OptionData, int32 Index)
{
	OptionIndex = Index;

	if (TitleText)
	{
		TitleText->SetText(OptionData.OptionTitle);
	}
	if (DescText)
	{
		DescText->SetText(OptionData.OptionDescription);
	}

	// Respect availability flag (event logic may override later)
	if (OptionButton)
	{
		OptionButton->SetIsEnabled(OptionData.bIsAvailable);
	}
}

void UEventOptionWidget::OnClicked()
{
	if (OptionIndex >= 0)
	{
		OnOptionSelected.Broadcast(OptionIndex);
	}
}
