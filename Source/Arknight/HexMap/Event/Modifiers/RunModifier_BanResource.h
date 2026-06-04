// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../RunModifierBase.h"
#include "RunModifier_BanResource.generated.h"

// [我要存钱] — bans acquisition of a specific resource type
UCLASS()
class ARKNIGHT_API URunModifier_BanResource : public URunModifierBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modifier")
	EResourceType BannedType;

	virtual bool QueryBanResourceAcquisition(EResourceType Type) const override
	{
		return Type == BannedType;
	}
};
