#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "../../DS.h"
#include "BattleResourceRecordAsset.generated.h"

UCLASS(BlueprintType)
class ARKNIGHT_API UBattleResourceRecordAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BattleResult")
	TMap<EResourceType, int32> ResourceGains;

public:
	UFUNCTION(BlueprintCallable, Category = "BattleResult")
	void ResetRecord();

	UFUNCTION(BlueprintCallable, Category = "BattleResult")
	void AddResourceGain(EResourceType ResourceType, int32 Amount);

	UFUNCTION(BlueprintPure, Category = "BattleResult")
	int32 GetResourceGain(EResourceType ResourceType) const;
};
