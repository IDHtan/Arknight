// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../../DS.h"
#include "EventOptionWidget.generated.h"

class UButton;
class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOptionSelectedSignature, int32, OptionIndex);

/**
 * Single clickable option widget inside the event panel.
 * Displays title and description for one event option.
 * On click, broadcasts OnOptionSelected with its index — zero business logic.
 */
UCLASS()
class ARKNIGHT_API UEventOptionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintAssignable, Category = "Event|Option")
	FOnOptionSelectedSignature OnOptionSelected;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TitleText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* DescText;

	UPROPERTY(meta = (BindWidget))
	UButton* OptionButton;

	UPROPERTY(BlueprintReadOnly)
	int32 OptionIndex = -1;

public:
	UFUNCTION(BlueprintCallable, Category = "Event|Option")
	void InitOption(const FHexEventOption& OptionData, int32 Index);

	UFUNCTION()
	void OnClicked();
};
