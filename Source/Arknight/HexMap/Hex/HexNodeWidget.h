// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../../DS.h"
#include "HexNodeWidget.generated.h"

class UButton;
class UImage;

UCLASS()
class ARKNIGHT_API UHexNodeWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HexData")
	FIntVector2 LogicalCoordinate = FIntVector2::ZeroValue;

	UPROPERTY(meta = (BindWidget))
	UButton* NodeButton = nullptr;

	UPROPERTY(meta = (BindWidget))
	UImage* NodeIcon = nullptr;

	UPROPERTY(meta = (BindWidget))
	UImage* FogImage = nullptr;

	UFUNCTION(BlueprintCallable, Category = "HexMap|Node")
	void InitWidgetData(const FHexNodeData& Data);

	UFUNCTION(BlueprintCallable, Category = "HexMap|Node")
	void UpdateVisual();

	UFUNCTION()
	void OnClicked();
};
