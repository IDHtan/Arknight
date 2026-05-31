#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DeployPanel.generated.h"

/**
 * 
 */
UCLASS()
class ARKNIGHT_API UDeployWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UButton* Up;

	UPROPERTY(meta = (BindWidget))
	class UButton* Down;

	UPROPERTY(meta = (BindWidget))
	class UButton* Left;

	UPROPERTY(meta = (BindWidget))
	class UButton* Right;

	UPROPERTY(meta = (BindWidget))
	class UImage* ImageAvatar;

	UPROPERTY()
	FName OperatorName;

	UPROPERTY()
	class ADeployableCell* TargetCell;

public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	void OnUpClicked();

	UFUNCTION(BlueprintCallable)
	void OnDownClicked();

	UFUNCTION(BlueprintCallable)
	void OnLeftClicked();

	UFUNCTION(BlueprintCallable)
	void OnRightClicked();

	UFUNCTION(BlueprintCallable)
	void OnShow(FName OpName, ADeployableCell* DeployableCell);

	UFUNCTION(BlueprintCallable)
	void HidePanel();
};