// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuHUDWidget.generated.h"

class UButton;
class UBaseBuildingPanel;
class UGamePrepPanel;

/**
 */
UCLASS()
class ARKNIGHT_API UMainMenuHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UButton* OpenBaseBuildingButton;

	UPROPERTY(meta = (BindWidget))
	UButton* OpenGamePrepButton;

	UPROPERTY(meta = (BindWidget))
	UBaseBuildingPanel* BaseBuildingPanel;

	UPROPERTY(meta = (BindWidget))
	UGamePrepPanel* GamePrepPanel;

	UFUNCTION()
	void OnBaseBuildingButtonClicked();

	UFUNCTION()
	void OnGamePrepButtonClicked();

private:
	void CollapseAllPanels();
};
