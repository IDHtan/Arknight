// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "../../DS.h"
#include "RunModifierBase.generated.h"

class URougeliteRunSubsystem;

/**
 * 
 */
UCLASS(Blueprintable)
class ARKNIGHT_API URunModifierBase : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modifier")
	EModifierScope Scope = EModifierScope::Permanent;

	//[无限火力]移动不消耗AP
	virtual bool QueryNoMovementCost() const { return false; }

	//[我要存钱]禁止获得原石
	virtual bool QueryBanResourceAcquisition(EResourceType Type) const { return false; }
		
	//[我要存钱]资源获得倍率/[强袭]紧急三倍
	virtual float QueryResourceGainMultiplier(EResourceType Type, float OriginalAmount) const { return OriginalAmount; }

	//[驰援险地]跨区域时触发
	virtual void ExecuteDelayedResourceReward(URougeliteRunSubsystem* RunSubsystem) {}

	// [人潮人海]全局攻击加成
	virtual float QueryGlobalAttackFlatBonus(float BaseAttack) const { return BaseAttack; }

	// [巧击]禁用战斗时间限制
	virtual bool QueryDisableCombatTimeLimit() const { return false; }
};
