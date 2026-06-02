// Fill out your copyright notice in the Description page of Project Settings.


#include "HexEventLogicBase.h"

#include "../Controller/HexMapSubsystem.h"
#include "../../URougeliteRunSubsystem.h"

void UHexEventLogicBase::InitializeLogic(UGameInstance* GameInstance)
{
	if (GameInstance)
	{
		MapSubsystemRef = GameInstance->GetSubsystem<UHexMapSubsystem>();
		RunSubsystemRef = GameInstance->GetSubsystem<URougeliteRunSubsystem>();
	}
}

void UHexEventLogicBase::ExecuteOption_Implementation(int32 OptionIndex)
{
	// Blueprint override — subclass this in BP to implement specific event outcomes.
	// Typical BP flow:
	//   1. Read OptionIndex to identify which option the player chose.
	//   2. Instantiate a URunModifierBase subclass (e.g. BP_Modifier_SaveMoney).
	//   3. Configure its Scope (Permanent / CurrentRegion / NextRegion).
	//   4. Call MapSubsystemRef->AddModifierInstance(Modifier).
	//   5. Optionally modify resources via RunSubsystemRef.
}
