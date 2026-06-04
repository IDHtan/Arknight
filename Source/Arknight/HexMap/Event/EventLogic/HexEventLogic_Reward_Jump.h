// 跃迁
#pragma once
#include "CoreMinimal.h"
#include "../HexEventLogicBase.h"
#include "../../Controller/HexMapPlayerController.h"
#include "../../Controller/HexMapSubsystem.h"
#include "../../Hex/HexMapHUDWidget.h"
#include "../Modifiers/RunModifier_RevealOnEnter.h"
#include "HexEventLogic_Reward_Jump.generated.h"

UCLASS()
class ARKNIGHT_API UHexEventLogic_Reward_Jump : public UHexEventLogicBase
{
	GENERATED_BODY()
	virtual void ExecuteOption_Implementation(int32 OptionIndex) override
	{
		switch (OptionIndex)
		{
		case 0: // 跃动 — restart current region + 20 AP
			MapSubsystemRef->EnterRegion(MapSubsystemRef->CurrentRegion);
			MapSubsystemRef->ChangeCurrentAP(20);
			break;
		case 1: // 迁徙 — teleport to another region + reveal all nodes
			if (UGameInstance* GI = MapSubsystemRef->GetGameInstance())
			{
				if (APlayerController* PC = GI->GetFirstLocalPlayerController())
				{
					if (AHexMapPlayerController* HexPC = Cast<AHexMapPlayerController>(PC))
					{
						if (HexPC->MapWidgetInstance)
						{
							URunModifier_RevealOnEnter* Mod = NewObject<URunModifier_RevealOnEnter>(this);
							Mod->Scope = EModifierScope::CurrentRegion;
							MapSubsystemRef->AddModifierInstance(Mod);

							HexPC->MapWidgetInstance->OpenTeleportPanel();
						}
					}
				}
			}
			break;
		}
	}
};
