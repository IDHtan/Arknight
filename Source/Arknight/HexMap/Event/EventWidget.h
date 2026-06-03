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
 *
 * Supports single-stage and multi-stage events:
 *   Single (MultiStageNumber == 0): all options shown at once.
 *   Multi  (MultiStageNumber >  0): options shown per stage via ShowStage().
 *
 * OpenEvent() loads data + initializes logic + displays stage 0.
 * HandleOptionSelected() forwards to logic via ExecuteOption(),
 * then auto-advances to next stage or closes the panel.
 */
UCLASS()
class ARKNIGHT_API UEventWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Event|State")
	FHexEventConfig CachedConfig;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Event|State")
	int32 CurrentStage = 0;

public:
	// Load event data, init logic, display first stage
	UFUNCTION(BlueprintCallable, Category = "Event")
	void OpenEvent(const FHexEventConfig& Config);

	// Display options for a specific stage (internal, called by HandleOptionSelected)
	UFUNCTION(BlueprintCallable, Category = "Event")
	void ShowStage(int32 StageIndex);

	// Called when player clicks an option; auto-advances or closes
	UFUNCTION(BlueprintCallable, Category = "Event")
	void HandleOptionSelected(int32 OptionIndex);

private:
	int32 GetTotalStages() const;
};
