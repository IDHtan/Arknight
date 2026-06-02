// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../../DS.h"
#include "EventOptionWidget.generated.h"

class UButton;
class UTextBlock;

/**
 * Single clickable option widget inside the event panel.
 * Displays title and description for one event option.
 * On click, notifies the parent UEventWidget (no core logic contained here).
 */
UCLASS()
class ARKNIGHT_API UEventOptionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TitleText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* DescText;

	UPROPERTY(meta = (BindWidget))
	UButton* OptionButton;

	UPROPERTY(BlueprintReadOnly)
	int32 OptionIndex = -1;

public:
	UFUNCTION(BlueprintCallable)
	void InitOption(const FHexEventOption& OptionData, int32 Index);

	UFUNCTION()
	void OnClicked();
};
