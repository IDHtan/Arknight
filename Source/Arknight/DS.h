#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h" 
#include "DS.generated.h" 


class AOperatorBase;
class ABattleCell;

UENUM(BlueprintType)
enum class ECellType : uint8
{
	DeployableGround UMETA(DisplayName = "Deployable Ground"),
	BlockedGround UMETA(DisplayName = "Blocked Ground"),
	DeployableHighGround UMETA(DisplayName = "Deployable High Ground"),
	BlockedHighGround UMETA(DisplayName = "Blocked High Ground"),
	ResourceSpawner UMETA(DisplayName = "Resource Spawner"),
	Functional UMETA(DisplayName = "Functional")
};

UENUM(BlueprintType)
enum class EResourceType : uint8
{
	Wood UMETA(DisplayName = "Wood"),
	Rock UMETA(DisplayName = "Rock"),
	Metal UMETA(DisplayName = "Metal"),
	Grain UMETA(DisplayName = "Grain")
};

UENUM(BlueprintType)
enum class EOperatorDeployType : uint8
{
	Melee UMETA(DisplayName = "Near"),
	Ranged UMETA(DisplayName = "Far"),
	Any UMETA(DisplayName = "Any")
};

UENUM(BlueprintType)
enum class EOperatorAttackType :uint8
{
	Melee UMETA(DisplayName = "Near"),
	Ranged UMETA(DisplayName = "Far")
};

UENUM(BlueprintType)
enum class EOperatorClassType : uint8
{
	Vanguard UMETA(DisplayName = "Vanguard"),
	Guard UMETA(DisplayName = "Guard"),
	Defender UMETA(DisplayName = "Defender"),
	Sniper UMETA(DisplayName = "Sniper"),
	Caster UMETA(DisplayName = "Caster"),
	Medic UMETA(DisplayName = "Medic"),
	Supporter UMETA(DisplayName = "Supporter"),
	Specialist UMETA(DisplayName = "Specialist")
};

UENUM(BlueprintType)
enum class EDeploymentDirection : uint8
{
	Up UMETA(DisplayName = "Up"),
	Down UMETA(DisplayName = "Down"),
	Left UMETA(DisplayName = "Left"),
	Right UMETA(DisplayName = "Right")
};

UENUM(BlueprintType)
enum class EDamageType : uint8
{
	Physical UMETA(DisplayName = "Physical"),
	Arts UMETA(DisplayName = "Arts"),
	TrueDamage UMETA(DisplayName = "True Damage")
};

UENUM(BlueprintType)
enum class EGameSpeedState : uint8
{
	Speed_1x UMETA(DisplayName = "1x"),
	Speed_2x UMETA(DisplayName = "2x"),
	Speed_05 UMETA(DisplayName = "0.5x"),
	Speed_025 UMETA(DisplayName = "0.25x")
};

USTRUCT(BlueprintType)
struct FOperatorRosterData
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Roster")
	FName OperatorName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Roster")
	TSubclassOf<AOperatorBase> OperatorClass = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Roster")
	int32 OperatorLevel = 1;
};

USTRUCT(BlueprintType)
struct FOperatorLocalRosterData
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Roster")
	FName OperatorName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Roster")
	TSubclassOf<AOperatorBase> OperatorClass = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Roster")
	AOperatorBase* OperatorInstance = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Roster")
	bool bIsDeployed = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Roster")
	EOperatorDeployType DeployType = EOperatorDeployType::Any;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Roster")
	int32 OperatorLevel = 1;	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Roster")
	int32 CurrentDeploymentCost = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Roster")
	float RedeployTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Roster")
	float RedeployCooldown = 0.0f;
};

USTRUCT(BlueprintType)
struct FOperatorDictionary : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Operator Config")
	FName OperatorName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Operator Config")
	TSubclassOf<AOperatorBase> OperatorClass = nullptr;
};

USTRUCT(BlueprintType)
struct FLevelLayoutConfig : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Level Config")
	int32 GridX = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level Config")
	int32 GridY = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level Config")
	TArray<TSubclassOf<ABattleCell>> LayoutData;

};

