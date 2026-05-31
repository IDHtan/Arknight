// Fill out your copyright notice in the Description page of Project Settings.


#include "OperatorDetailWidget.h"
#include "../../Operator/OperatorBase.h"
#include "../../DS.h"
#include "../Controller/BattlePlayerController.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/GridPanel.h"
#include "Components/Button.h"
#include "Kismet/KismetTextLibrary.h"
#include "../../Component/TargetingComponent.h"

void UOperatorDetailWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (ButtonRetreat)
	{
		ButtonRetreat->OnClicked.AddDynamic(this, &UOperatorDetailWidget::HidePanel);
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
	UE_LOG(LogTemp, Log, TEXT("Retreat button clicked for operator: %s"), *OperatorInfo->OperatorName.ToString());
	if (OperatorInfo)
	{
		if(ABattlePlayerController* PC = Cast<ABattlePlayerController>(GetOwningPlayer()))
		{
			PC->Retreat(OperatorInfo);
		}
	}
}

