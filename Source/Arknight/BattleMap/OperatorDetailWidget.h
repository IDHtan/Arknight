// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OperatorDetailWidget.generated.h"

class AOperatorBase;

/**
 * 
 */
UCLASS()
class ARKNIGHT_API UOperatorDetailWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* TextName;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* TextDescription;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* TextAtk;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* TextAspd;

    UPROPERTY(meta = (BindWidget))
    class UImage* ImgAvatar;

    UPROPERTY(Transient, meta = (BindWidgetAnim))
    class UWidgetAnimation* SlideInAnim;

    UPROPERTY(meta = (BindWidget))
    class UGridPanel* GridAttackRange;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UUserWidget> GridBlockClass;

public:
    UFUNCTION(BlueprintCallable, Category = "UI")
    void UpdateAndShow(AOperatorBase* OperatorInfo);

    UFUNCTION(BlueprintCallable, Category = "UI")
    void HidePanel();
};
