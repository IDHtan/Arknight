// Fill out your copyright notice in the Description page of Project Settings.

#include "InfrastructureIcon.h"
#include "Components/TextBlock.h"

void UInfrastructureIcon::SetQuantity(int32 Quantity)
{
	if (QuantityText)
	{
		QuantityText->SetText(FText::AsNumber(Quantity));
	}
}

void UInfrastructureIcon::SetNone()
{
	Super::SetNone();

	if (QuantityText)
	{
		QuantityText->SetText(FText::GetEmpty());
		QuantityText->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UInfrastructureIcon::Update()
{
	Super::Update();

	if (QuantityText)
	{
		QuantityText->SetVisibility(ESlateVisibility::Visible);
	}
}
