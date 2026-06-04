// 转徙 (reused by Wood/Rock/Grain/Metal)
#pragma once
#include "CoreMinimal.h"
#include "../HexEventLogicBase.h"
#include "../../Controller/HexMapPlayerController.h"
#include "../../Controller/HexMapSubsystem.h"
#include "../../Hex/HexMapHUDWidget.h"
#include "HexEventLogic_Migration.generated.h"

UCLASS()
class ARKNIGHT_API UHexEventLogic_Migration : public UHexEventLogicBase
{
	GENERATED_BODY()
	virtual void ExecuteOption_Implementation(int32 OptionIndex) override
	{
		switch (OptionIndex)
		{
		case 0: { // 迁途 — open teleport panel
			if (UGameInstance* GI = MapSubsystemRef->GetGameInstance())
			{
				if (APlayerController* PC = GI->GetFirstLocalPlayerController())
				{
					if (AHexMapPlayerController* HexPC = Cast<AHexMapPlayerController>(PC))
					{
						if (HexPC->MapWidgetInstance)
							HexPC->MapWidgetInstance->OpenTeleportPanel();
					}
				}
			}
			break; }
		case 1: MapSubsystemRef->ChangeCurrentAP(4); break; // 定居
		}
	}
};
