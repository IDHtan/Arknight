// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../../DS.h"
#include "OperatorCardWidget.generated.h"

/**
 * 
 */
UCLASS()
class ARKNIGHT_API UOperatorCardWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UImage* ImageAvatar;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TextCost;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* ProgressBarCoolDown;

	UPROPERTY(meta = (BindWidget))
	class UButton* HitBox;

public:
	const FOperatorLocalRosterData* OperatorData; //a little bit dangerous

	int32 CurrentCost = 0;

	UPROPERTY()
	bool bIsSelected = false;
	
public:
	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void OnCreated(const FOperatorLocalRosterData& OperatorRD);

	UFUNCTION(BlueprintCallable)
	void OnClicked();

	UFUNCTION(BlueprintCallable)
	void OnClickedCardChanged(FName OperatorName);

	UFUNCTION(BlueprintCallable)
	void OnCostUpdated(int32 NewCost);

};
