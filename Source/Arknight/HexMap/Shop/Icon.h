// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../../DS.h"
#include "Icon.generated.h"

class UButton;
class UImage;
class UTextBlock;

/**
 * Shared icon widget used by both Shop and Backpack systems.
 * Displays an icon from IconImageTable and optionally binds a resource type
 * for business logic (shop transactions, backpack selection).
 */
UCLASS()
class ARKNIGHT_API UIcon : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	UButton* IconButton;

	UPROPERTY(meta = (BindWidget))
	UImage* IconImage;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* NameText;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FName IconRowName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bHasLinkedResourceType = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EResourceType LinkedResourceType;

public:
	UFUNCTION(BlueprintCallable)
	void SetIconByRowName(FName RowName);

	UFUNCTION(BlueprintCallable)
	void BindResourceType(EResourceType Type, FName RowName);
};
