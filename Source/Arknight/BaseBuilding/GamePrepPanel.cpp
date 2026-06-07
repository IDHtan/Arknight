// Fill out your copyright notice in the Description page of Project Settings.

#include "GamePrepPanel.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "../URougeliteRunSubsystem.h"
#include "../HexMap/Controller/HexMapSubsystem.h"
#include "../HexMap/Shop/Icon.h"

void UGamePrepPanel::NativeConstruct()
{
	Super::NativeConstruct();

	RunSubsystem = GetGameInstance() ? GetGameInstance()->GetSubsystem<URougeliteRunSubsystem>() : nullptr;
	HexMapSubsystem = GetGameInstance() ? GetGameInstance()->GetSubsystem<UHexMapSubsystem>() : nullptr;

	if (FoodIncreaseButton)
	{
		FoodIncreaseButton->OnClicked.AddDynamic(this, &UGamePrepPanel::OnFoodIncreaseClicked);
	}
	if (FoodDecreaseButton)
	{
		FoodDecreaseButton->OnClicked.AddDynamic(this, &UGamePrepPanel::OnFoodDecreaseClicked);
	}
	if (HQFoodIncreaseButton)
	{
		HQFoodIncreaseButton->OnClicked.AddDynamic(this, &UGamePrepPanel::OnHQFoodIncreaseClicked);
	}
	if (HQFoodDecreaseButton)
	{
		HQFoodDecreaseButton->OnClicked.AddDynamic(this, &UGamePrepPanel::OnHQFoodDecreaseClicked);
	}
	if (ConfirmButton)
	{
		ConfirmButton->OnClicked.AddDynamic(this, &UGamePrepPanel::OnConfirmClicked);
	}
	if (ExitButton)
	{
		ExitButton->OnClicked.AddDynamic(this, &UGamePrepPanel::Close);
	}

	SetVisibility(ESlateVisibility::Collapsed);
}

void UGamePrepPanel::Open()
{
	FoodToConsume = 0;
	HQFoodToConsume = 0;
	CalculatedAP = 0;

	RefreshOwnedDisplay();
	RefreshConsumeDisplay();

	SetVisibility(ESlateVisibility::Visible);
}

void UGamePrepPanel::Close()
{
	SetVisibility(ESlateVisibility::Collapsed);
}

void UGamePrepPanel::RefreshOwnedDisplay()
{
	OwnedFood = RunSubsystem ? RunSubsystem->GetGlobalResourceAmount(EResourceType::Food) : 0;
	OwnedHQFood = RunSubsystem ? RunSubsystem->GetGlobalResourceAmount(EResourceType::HighQualityFood) : 0;

	if (FoodOwnedText)
	{
		FoodOwnedText->SetText(FText::AsNumber(OwnedFood));
	}
	if (HQFoodOwnedText)
	{
		HQFoodOwnedText->SetText(FText::AsNumber(OwnedHQFood));
	}

	// Clamp current consume amounts to owned amounts
	FoodToConsume = FMath::Min(FoodToConsume, OwnedFood);
	HQFoodToConsume = FMath::Min(HQFoodToConsume, OwnedHQFood);
}

void UGamePrepPanel::RefreshConsumeDisplay()
{
	CalculateAP();

	if (FoodConsumeText)
	{
		FoodConsumeText->SetText(FText::AsNumber(FoodToConsume));
	}
	if (HQFoodConsumeText)
	{
		HQFoodConsumeText->SetText(FText::AsNumber(HQFoodToConsume));
	}
	if (APGainText)
	{
		APGainText->SetText(FText::AsNumber(CalculatedAP));
	}

	// Disable confirm if no AP would be gained
	if (ConfirmButton)
	{
		ConfirmButton->SetIsEnabled(CalculatedAP > 0);
	}
}

void UGamePrepPanel::CalculateAP()
{
	CalculatedAP = (FoodToConsume * APPerFood) + (HQFoodToConsume * APPerHighQualityFood);
}

void UGamePrepPanel::OnFoodIncreaseClicked()
{
	FoodToConsume = FMath::Min(FoodToConsume + 1, OwnedFood);
	RefreshConsumeDisplay();
}

void UGamePrepPanel::OnFoodDecreaseClicked()
{
	FoodToConsume = FMath::Max(FoodToConsume - 1, 0);
	RefreshConsumeDisplay();
}

void UGamePrepPanel::OnHQFoodIncreaseClicked()
{
	HQFoodToConsume = FMath::Min(HQFoodToConsume + 1, OwnedHQFood);
	RefreshConsumeDisplay();
}

void UGamePrepPanel::OnHQFoodDecreaseClicked()
{
	HQFoodToConsume = FMath::Max(HQFoodToConsume - 1, 0);
	RefreshConsumeDisplay();
}

void UGamePrepPanel::OnConfirmClicked()
{
	CalculateAP();
	if (CalculatedAP <= 0)
	{
		return;
	}

	if (!RunSubsystem || !HexMapSubsystem)
	{
		UE_LOG(LogTemp, Error, TEXT("GamePrepPanel::OnConfirmClicked: RunSubsystem or HexMapSubsystem is null"));
		return;
	}

	// Deduct consumed food
	if (FoodToConsume > 0)
	{
		RunSubsystem->TryConsumeResource(EResourceType::Food, FoodToConsume);
	}
	if (HQFoodToConsume > 0)
	{
		RunSubsystem->TryConsumeResource(EResourceType::HighQualityFood, HQFoodToConsume);
	}

	// Set AP for the new run (GlobalResources[AP] is read by InitializeNewRun)
	RunSubsystem->SetGlobalResourceAmount(EResourceType::AP, CalculatedAP);

	// Initialise HexMap run
	const int32 Seed = FMath::Rand();
	HexMapSubsystem->InitializeNewRun(Seed);
	HexMapSubsystem->EnterRegion(StartingRegion);

	// Open HexMap level
	UGameplayStatics::OpenLevel(this, TEXT("/Game/Maps/Lvl_HexMap"));
}
