// Fill out your copyright notice in the Description page of Project Settings.


#include "CellDetailWidget.h"
#include "../Cell/ResourceCell.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "../../Component/HealthComponent.h"


void UCellDetailWidget::UpdateAndShow(AResourceCell* ResourceInfo)
{
	if (!ResourceInfo) return;
	TargetResource = ResourceInfo;

	switch (ResourceInfo->ResourceType)
	{
	case EResourceType::Grain:
		TextCellType->SetText(NSLOCTEXT("UI", "Grain", "稻谷"));
		ImageGrain->SetVisibility(ESlateVisibility::Visible);
		ImageWood->SetVisibility(ESlateVisibility::Collapsed);
		ImageRock->SetVisibility(ESlateVisibility::Collapsed);
		ImageMetal->SetVisibility(ESlateVisibility::Collapsed);
		break;
	case EResourceType::Wood:
		TextCellType->SetText(NSLOCTEXT("UI", "Wood", "原木"));
		ImageGrain->SetVisibility(ESlateVisibility::Collapsed);
		ImageWood->SetVisibility(ESlateVisibility::Visible);
		ImageRock->SetVisibility(ESlateVisibility::Collapsed);
		ImageMetal->SetVisibility(ESlateVisibility::Collapsed);
		break;
	case EResourceType::Rock:
		TextCellType->SetText(NSLOCTEXT("UI", "Rock", "原石"));
		ImageGrain->SetVisibility(ESlateVisibility::Collapsed);
		ImageWood->SetVisibility(ESlateVisibility::Collapsed);
		ImageRock->SetVisibility(ESlateVisibility::Visible);
		ImageMetal->SetVisibility(ESlateVisibility::Collapsed);
		break;
	case EResourceType::Metal:
		TextCellType->SetText(NSLOCTEXT("UI", "Metal", "原矿"));
		ImageGrain->SetVisibility(ESlateVisibility::Collapsed);
		ImageWood->SetVisibility(ESlateVisibility::Collapsed);
		ImageRock->SetVisibility(ESlateVisibility::Collapsed);
		ImageMetal->SetVisibility(ESlateVisibility::Visible);
		break;
	}	
	TextAmount->SetText(FText::Format(NSLOCTEXT("UI", "Amount", "剩余数量: {0}"), FText::AsNumber(ResourceInfo->DropAmount)));
	TextHealth->SetText(FText::Format(NSLOCTEXT("UI", "Health", "{0}/{1}"), FText::AsNumber(ResourceInfo->HealthComp->CurrentHealth), FText::AsNumber(ResourceInfo->HealthComp->MaxHealth)));
	TextDefense->SetText(FText::Format(NSLOCTEXT("UI", "Defense", "防御: {0}"), FText::AsNumber(ResourceInfo->HealthComp->Defense)));
	TextArtsResistance->SetText(FText::Format(NSLOCTEXT("UI", "ArtsResist", "法术抗性: {0}"), FText::AsNumber(ResourceInfo->HealthComp->ArtsResistance)));
	
	UE_LOG(LogTemp, Log, TEXT("Showing cell detail panel for Resource Type: %d"), static_cast<int32>(ResourceInfo->ResourceType));
	SetVisibility(ESlateVisibility::Visible);
	if (SlideInAnim)
	{
		PlayAnimation(SlideInAnim);
	}
}

void UCellDetailWidget::HidePanel()
{
	SetVisibility(ESlateVisibility::Collapsed);
}

void UCellDetailWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (TargetResource && IsValid(TargetResource->HealthComp))
	{
		float CurrentHP = TargetResource->HealthComp->CurrentHealth;
		float MaxHP = TargetResource->HealthComp->MaxHealth;

		if (MaxHP > 0.0f)
		{
			ProgressBarHealth->SetPercent(CurrentHP / MaxHP);
		}
	}
	else
	{
		SetVisibility(ESlateVisibility::Collapsed);
	}
}
