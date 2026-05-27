// LvlDebugGameMode.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LvlDebugGameMode.generated.h"

UCLASS()
class ARKNIGHT_API ALvlDebugGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	FName LevelID = NAME_None;

protected:
	virtual void BeginPlay() override;
};