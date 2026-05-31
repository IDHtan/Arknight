// Fill out your copyright notice in the Description page of Project Settings.


#include "OperatorCardWidget.h"
#include "Components/CheckBox.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/Button.h"
#include "../../Operator/OperatorBase.h"
#include "../Controller/BattlePlayerController.h"

void UOperatorCardWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (HitBox)
	{
		HitBox->OnClicked.AddDynamic(this, &UOperatorCardWidget::OnClicked);
	}
	ABattlePlayerController* BattlePC = Cast<ABattlePlayerController>(GetOwningPlayer());
	if(BattlePC)
	{
		BattlePC->OnOperatorCardClicked.AddDynamic(this, &UOperatorCardWidget::OnClickedCardChanged);
		BattlePC->OnCostChanged.AddDynamic(this, &UOperatorCardWidget::OnCostUpdated);
	}
}

void UOperatorCardWidget::OnCreated(const FOperatorLocalRosterData& OperatorRD)
{
	OperatorData = &OperatorRD;
	bIsSelected = false;
	CurrentCost = OperatorData->CurrentDeploymentCost;
	if (TextCost)
	{
		TextCost->SetText(FText::AsNumber(CurrentCost));		
	}
	AOperatorBase* DefaultOperator = OperatorData->OperatorClass->GetDefaultObject<AOperatorBase>();
	if (DefaultOperator)
	{
		ImageAvatar->SetBrushFromTexture(DefaultOperator->AvatarImage);
	}
}

void UOperatorCardWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!OperatorData)
	{
		UE_LOG(LogTemp, Warning, TEXT("OperatorData is null in OperatorCardWidget!!!"));
		RemoveFromParent();
		return;
	}

	if (OperatorData->bIsDeployed && Visibility != ESlateVisibility::Collapsed)
	{
		SetVisibility(ESlateVisibility::Collapsed);
	}
	else if (!OperatorData->bIsDeployed && Visibility != ESlateVisibility::Visible)
	{
		SetVisibility(ESlateVisibility::Visible);
	}

	if(OperatorData->CurrentDeploymentCost != CurrentCost)
	{
		CurrentCost = OperatorData->CurrentDeploymentCost;
		TextCost->SetText(FText::AsNumber(CurrentCost));
	}

	if(OperatorData->RedeployCooldown > 0.0f)
	{
		ProgressBarCoolDown->SetPercent(OperatorData->RedeployCooldown / OperatorData->RedeployTime);
	}
	else
	{
		ProgressBarCoolDown->SetVisibility(ESlateVisibility::Collapsed);
	}

}

void UOperatorCardWidget::OnClicked()
{
	bIsSelected = !bIsSelected;
	if (bIsSelected)
	{
		ABattlePlayerController* BattlePC = Cast<ABattlePlayerController>(GetOwningPlayer());
		BattlePC->OperatorCardChanged(OperatorData->OperatorName);
	}
	else
	{
		ABattlePlayerController* BattlePC = Cast<ABattlePlayerController>(GetOwningPlayer());
		BattlePC->OperatorCardChanged(NAME_None);
	}
}

void UOperatorCardWidget::OnClickedCardChanged(FName OperatorName)
{
	if (OperatorData && OperatorData->OperatorName == OperatorName)
	{
		bIsSelected = true;
		SetRenderTranslation(FVector2D(0.0f, -10.0f));
	}
	else
	{
		bIsSelected = false;
		SetRenderTranslation(FVector2D(0.0f, 0.0f));
	}
}

void UOperatorCardWidget::OnCostUpdated(int32 NewCost)
{
	if(NewCost>= CurrentCost)
	{
		SetColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
	}
	else
	{
		SetColorAndOpacity(FLinearColor(0.4f, 0.4f, 0.4f, 1.0f));
	}
}