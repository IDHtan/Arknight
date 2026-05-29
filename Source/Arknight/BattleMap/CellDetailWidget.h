// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CellDetailWidget.generated.h"

/**
 * 
 */
UCLASS()
class ARKNIGHT_API UCellDetailWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TextCellType;

	UPROPERTY(meta = (BindWidget))
	class UImage* ImageGrain;

	UPROPERTY(meta = (BindWidget))
	class UImage* ImageWood;

	UPROPERTY(meta = (BindWidget))
	class UImage* ImageRock;

	UPROPERTY(meta = (BindWidget))
	class UImage* ImageMetal;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TextAmount;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TextHealth;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TextDefense;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TextArtsResistance;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* ProgressBarHealth;

	UPROPERTY()
	class AResourceCell* TargetResource;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	class UWidgetAnimation* SlideInAnim;

public:
	UFUNCTION(BlueprintCallable)
	void UpdateAndShow(AResourceCell* ResourceInfo);

	UFUNCTION(BlueprintCallable)
	void HidePanel();

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
};
