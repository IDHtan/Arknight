// Fill out your copyright notice in the Description page of Project Settings.


#include "OperatorDetailWidget.h"
#include "../../Operator/OperatorBase.h"
#include "../../DS.h"
#include "../Controller/BattlePlayerController.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/GridPanel.h"
#include "Components/GridSlot.h"
#include "Components/Button.h"
#include "Kismet/KismetTextLibrary.h"
#include "../../Component/TargetingComponent.h"

void UOperatorDetailWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (ButtonRetreat)
	{
		ButtonRetreat->OnClicked.AddDynamic(this, &UOperatorDetailWidget::OnRetreatClicked);
	}

	OperatorInfo = nullptr;
}

void UOperatorDetailWidget::UpdateAndShow(FName OperatorName)
{
	if (OperatorName.IsNone()) return;

	FOperatorLocalRosterData* OperatorRoster = 
		Cast<ABattlePlayerController>(GetOwningPlayer())->
		LocalRoster.FindByPredicate(
			[OperatorName](const FOperatorLocalRosterData& Data)
			{
				return Data.OperatorName == OperatorName;
			});
	if (!OperatorRoster) return;

	OperatorInfo = nullptr;
	if(OperatorRoster->bIsDeployed && OperatorRoster->OperatorInstance)
	{
		OperatorInfo = OperatorRoster->OperatorInstance;
	}
	else
	{
		if (OperatorRoster->OperatorClass)
		{
			OperatorInfo = OperatorRoster->OperatorClass.GetDefaultObject();
		}
	}
	if (!OperatorInfo) return;

	TextName->SetText(FText::FromName(OperatorInfo->OperatorName));
	TextDescription->SetText(OperatorInfo->OperatorDescription);
	if (OperatorInfo->TargetingComp && GridBlockClass)
	{
		TextAtk->SetText(FText::Format(NSLOCTEXT("UI", "Atk", "攻击力: {0}"), FText::AsNumber(OperatorInfo->TargetingComp->AttackPower)));
		TextAspd->SetText(FText::Format(NSLOCTEXT("UI", "Atk", "攻击速度: {0}"), FText::AsNumber(100.f / OperatorInfo->TargetingComp->AttackSpeed)));

		GridAttackRange->ClearChildren();
		const int32 CenterRow = 3;
		const int32 CenterCol = 3;
		const FMargin GridPadding(5.f);
		UUserWidget* SelfBlock = CreateWidget<UUserWidget>(this, GridBlockClass);
		if (SelfBlock)
		{
			SelfBlock->SetColorAndOpacity(FLinearColor::White);
			if (UGridSlot* SelfSlot = GridAttackRange->AddChildToGrid(SelfBlock, CenterRow, CenterCol))
			{
				SelfSlot->SetPadding(GridPadding);
			}
		}
		for (const FIntVector2& Offset : OperatorInfo->TargetingComp->BaseAttackRange)
		{
			if (Offset == FIntVector2::ZeroValue)
			{
				continue;
			}

			UUserWidget* RangeBlock = CreateWidget<UUserWidget>(this, GridBlockClass);
			if (RangeBlock)
			{
				RangeBlock->SetColorAndOpacity(FLinearColor::Red);
				const int32 TargetRow = CenterRow - Offset.Y;
				const int32 TargetCol = CenterCol + Offset.X;
				if (UGridSlot* RangeSlot = GridAttackRange->AddChildToGrid(RangeBlock, TargetRow, TargetCol))
				{
					RangeSlot->SetPadding(GridPadding);
				}
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Grid Block class not set"));
	}

	if (OperatorInfo->AvatarImage)
	{
		ImgAvatar->SetBrushFromTexture(OperatorInfo->AvatarImage);
	}

	if(OperatorRoster->bIsDeployed)
	{
		ButtonRetreat->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		ButtonRetreat->SetVisibility(ESlateVisibility::Collapsed);
	}

	UE_LOG(LogTemp, Log, TEXT("Showing operator detail panel for %s"), *OperatorInfo->OperatorName.ToString());
	SetVisibility(ESlateVisibility::Visible);
	if (SlideInAnim)
	{
		PlayAnimation(SlideInAnim);
	}

}

void UOperatorDetailWidget::HidePanel()
{
	SetVisibility(ESlateVisibility::Collapsed);
}

void UOperatorDetailWidget::OnRetreatClicked()
{
	if (OperatorInfo)
	{
		UE_LOG(LogTemp, Log, TEXT("Retreat button clicked for operator: %s"), *OperatorInfo->OperatorName.ToString());
		if(ABattlePlayerController* PC = Cast<ABattlePlayerController>(GetOwningPlayer()))
		{
			PC->Retreat(OperatorInfo);
		}
		SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Retreat button clicked but OperatorInfo is null"));
	}
}

