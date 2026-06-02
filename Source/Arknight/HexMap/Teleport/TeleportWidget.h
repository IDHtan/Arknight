// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../../DS.h"
#include "TeleportWidget.generated.h"

class UButton;

/**
 * Teleport interaction panel.
 * Allows the player to choose a destination region for teleport travel.
 * One button per region type; the button for the current region is hidden.
 */
UCLASS()
class ARKNIGHT_API UTeleportWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UButton* TeleportButton1;   // Wood

	UPROPERTY(meta = (BindWidget))
	UButton* TeleportButton2;   // Rock

	UPROPERTY(meta = (BindWidget))
	UButton* TeleportButton3;   // Metal

	UPROPERTY(meta = (BindWidget))
	UButton* TeleportButton4;   // Grain

	UPROPERTY(meta = (BindWidget))
	UButton* ConfirmButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	EHexRegionType ClickedRegionType;

public:
	UFUNCTION(BlueprintCallable)
	void InitWidgetData(EHexRegionType CurrentRegion);

	UFUNCTION(BlueprintCallable)
	void On1Clicked();

	UFUNCTION(BlueprintCallable)
	void On2Clicked();

	UFUNCTION(BlueprintCallable)
	void On3Clicked();

	UFUNCTION(BlueprintCallable)
	void On4Clicked();

	UFUNCTION(BlueprintCallable)
	void OnOneButtonClicked(EHexRegionType RegionType);//set ClickedRegionType, and detemine which button to larger itself

	UFUNCTION()
	void OnConfirmClicked();
};
