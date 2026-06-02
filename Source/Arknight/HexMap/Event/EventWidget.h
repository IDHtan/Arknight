// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../../DS.h"
#include "EventWidget.generated.h"

class UImage;
class UTextBlock;
class UHorizontalBox;
class UEventOptionWidget;
class UHexEventLogicBase;

/**
 * Event panel — renders the full event screen from FHexEventConfig.
 * Dynamically creates UEventOptionWidget instances for each option.
 * Handles player selection by forwarding to UHexEventLogicBase.
 */
UCLASS()
class ARKNIGHT_API UEventWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	UImage* EventImage;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* EventTitleText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* EventDescText;

	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* OptionsContainer;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UEventOptionWidget> OptionWidgetClass;

	UPROPERTY(Transient)
	UHexEventLogicBase* ActiveLogicInstance;

public:
	UFUNCTION(BlueprintCallable)
	void OpenEvent(const FHexEventConfig& Config);

	UFUNCTION(BlueprintCallable)
	void HandleOptionSelected(int32 OptionIndex);
};
