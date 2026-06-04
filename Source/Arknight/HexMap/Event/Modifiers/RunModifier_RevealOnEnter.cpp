// Fill out your copyright notice in the Description page of Project Settings.


#include "RunModifier_RevealOnEnter.h"
#include "../../Controller/HexMapSubsystem.h"
#include "../../../URougeliteRunSubsystem.h"

void URunModifier_RevealOnEnter::ExecuteDelayedResourceReward(URougeliteRunSubsystem* RunSubsystem)
{
	if (!RunSubsystem) return;

	UGameInstance* GI = RunSubsystem->GetGameInstance();
	UHexMapSubsystem* HexMap = GI ? GI->GetSubsystem<UHexMapSubsystem>() : nullptr;
	if (HexMap)
	{
		HexMap->RevealAllRegionNodes();
	}
}
