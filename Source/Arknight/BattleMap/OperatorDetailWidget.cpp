// Fill out your copyright notice in the Description page of Project Settings.


#include "../BattleMap/OperatorDetailWidget.h"
#include "../Operator/OperatorBase.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/GridPanel.h"
#include "Kismet/KismetTextLibrary.h"
#include "../Component/TargetingComponent.h"

void UOperatorDetailWidget::UpdateAndShow(AOperatorBase* OperatorInfo)
{
	if (!OperatorInfo) return;

	TextName->SetText(FText::FromName(OperatorInfo->OperatorName));
	TextDescription->SetText(OperatorInfo->OperatorDescription);
	if (OperatorInfo->TargetingComp)
	{
		TextAtk->SetText(FText::Format(NSLOCTEXT("UI", "Atk", "攻击力: {0}"), FText::AsNumber(OperatorInfo->TargetingComp->AttackPower)));
		TextAspd->SetText(FText::Format(NSLOCTEXT("UI", "Atk", "攻击速度: {0}"), FText::AsNumber(100.f / OperatorInfo->TargetingComp->AttackSpeed)));

		GridAttackRange->ClearChildren();
		int32 CenterRow = 3, CenterCol = 1;

		UUserWidget* SelfBlock = CreateWidget<UUserWidget>(this, GridBlockClass);
		if(SelfBlock)
		{
			SelfBlock->SetColorAndOpacity(FLinearColor::White);
			GridAttackRange->AddChildToGrid(SelfBlock, CenterRow, CenterCol);
		}
		for (auto& Offset : OperatorInfo->TargetingComp->BaseAttackRange)
		{
			UUserWidget* RangeBlock = CreateWidget<UUserWidget>(this, GridBlockClass);
			if (RangeBlock)
			{
				RangeBlock->SetColorAndOpacity(FLinearColor::Red);
				int32 TargetRow = CenterRow - Offset.Y;
				int32 TargetCol = CenterCol + Offset.X;
				GridAttackRange->AddChildToGrid(RangeBlock, TargetRow, TargetCol);
			}
		}
	}

	if (OperatorInfo->AvatarImage)
	{
		ImgAvatar->SetBrushFromTexture(OperatorInfo->AvatarImage);
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