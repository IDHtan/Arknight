// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../RunModifierBase.h"
#include "RunModifier_GainMultiplier.generated.h"

// Generic resource gain multiplier modifier.
// [储财/DoubleGain] bEmergenyOnly=false, Multiplier=2.0 — always ×2
// [强袭/Assault]   bEmergenyOnly=true,  Multiplier=1.5 —> ×3 on emergency only
UCLASS()
class ARKNIGHT_API URunModifier_GainMultiplier : public URunModifierBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modifier")
	float Multiplier = 1.0f;

	// When true, only applies if this is an emergency combat
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modifier")
	bool bEmergenyOnly = false;

	virtual int32 QueryResourceGainMultiplier(int32 Amount, bool bIsEmergency) const override
	{
		if (bEmergenyOnly && !bIsEmergency)
		{
			return Amount;
		}
		return FMath::RoundToInt(Amount * Multiplier);
	}
};
