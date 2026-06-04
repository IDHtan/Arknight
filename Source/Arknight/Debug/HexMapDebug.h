// HexMapDebug.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "HexMapDebug.generated.h"

UCLASS()
class ARKNIGHT_API AHexMapDebug : public AGameModeBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	int32 initialAP = 10;

protected:
	virtual void BeginPlay() override;
};