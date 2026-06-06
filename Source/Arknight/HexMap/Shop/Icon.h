// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../../DS.h"
#include "Icon.generated.h"

class UButton;
class UImage;
class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnIconClickedSignature, EResourceType, ResourceType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnIconButtonClickedSignature);

/**
 * Shared icon widget used by both Shop and Backpack systems.
 * Displays an icon from IconImageTable for a given resource type.
 *
 * Two usage modes:
 *  1) Editor: set LinkedResourceType + bHasLinkedResourceType in Details panel.
 *     NativeConstruct auto-loads the icon.
 *  2) Dynamic: call SetResourceType(Type) after creation.
 *
 * When clicked, broadcasts OnIconClicked with the LinkedResourceType.
 * The owning panel binds to this delegate and handles its own logic.
 * The Icon itself contains zero business logic.
 */
UCLASS()
class ARKNIGHT_API UIcon : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintAssignable, Category = "Icon|Events")
	FOnIconClickedSignature OnIconClicked;

	UPROPERTY(BlueprintAssignable, Category = "Icon|Events")
	FOnIconButtonClickedSignature OnIconButtonClicked;

	UPROPERTY(meta = (BindWidget))
	UButton* IconButton;

	UPROPERTY(meta = (BindWidget))
	UImage* IconImage;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* NameText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Icon")
	bool bHasLinkedResourceType = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Icon")
	EResourceType LinkedResourceType;

public:
	UFUNCTION(BlueprintCallable, Category = "Icon")
	void SetResourceType(EResourceType Type);

	UFUNCTION(BlueprintCallable, Category = "Icon")
	virtual void SetNone();

	UFUNCTION(BlueprintCallable, Category = "Icon")
	virtual void Update();

	UFUNCTION()
	void HandleIconButtonClicked();
};
