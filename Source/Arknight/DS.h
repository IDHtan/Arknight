#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h" 
#include "DS.generated.h" 


class AOperatorBase;
class ABattleCell;
class UHexEventLogicBase;
class UTexture2D;

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
	Wood = 0 UMETA(DisplayName = "Wood"),
	Rock = 1 UMETA(DisplayName = "Rock"),
	Grain = 2 UMETA(DisplayName = "Grain"),
	Metal = 3 UMETA(DisplayName = "Metal"),
	Food = 4 UMETA(DisplayName = "Food"),
	HighQualityFood = 5 UMETA(DisplayName = "HighQualityFood"),
	AP = 6 UMETA(DisplayName = "AP")
};

UENUM(BlueprintType)
enum class EHexRegionType : uint8
{
	Wood UMETA(DisplayName = "Wood"),
	Rock UMETA(DisplayName = "Rock"),
	Grain UMETA(DisplayName = "Grain"),
	Metal UMETA(DisplayName = "Metal")
};

// Behavior tags for EResourceType — single source of truth for per-type branching.
// Each system queries the tag it needs instead of hardcoding if (Type != AP) everywhere.
namespace EResourceTypeMeta
{
	// Can be held and displayed in the backpack inventory
	inline bool IsStorable(EResourceType Type) { return Type != EResourceType::AP; }

	// Can be spent as cost in shop transactions (pay this to get something else)
	inline bool IsTradeableAsCost(EResourceType Type) { return Type != EResourceType::AP; }

	// Map region type to its primary resource
	inline EResourceType GetRegionPrimaryResource(EHexRegionType Region)
	{
		switch (Region)
		{
		case EHexRegionType::Wood:  return EResourceType::Wood;
		case EHexRegionType::Rock:  return EResourceType::Rock;
		case EHexRegionType::Metal: return EResourceType::Metal;
		case EHexRegionType::Grain: return EResourceType::Grain;
		default: return EResourceType::Wood;
		}
	}
}

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Roster")
	bool bCanJoinBattle = true;
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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level Config")
	FName LevelID = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hex Event")
	EHexRegionType TargetRegion = EHexRegionType::Wood;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Level Config")
	int32 GridX = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level Config")
	int32 GridY = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level Config")
	TArray<TSubclassOf<ABattleCell>> LayoutData;

};

UENUM(BlueprintType)
enum class EHexNodeState : uint8
{
	Masked UMETA(DisplayName = "Masked"),
	Unmasked UMETA(DisplayName = "Unmasked"),
	Reachable UMETA(DisplayName = "Reachable"),
	Cleared UMETA(DisplayName = "Cleared")
};

UENUM(BlueprintType)
enum class EHexNodeType : uint8
{
	None UMETA(DisplayName = "None"),
	Entrance UMETA(DisplayName = "Entrance"),
	Combat_Normal UMETA(DisplayName = "Combat_Normal"),
	Combat_Emergency UMETA(DisplayName = "Combat_Emergency"),
	Event UMETA(DisplayName = "Event"),
	Reward UMETA(DisplayName = "Reward"),
	Shop UMETA(DisplayName = "Shop"),
	Teleport UMETA(DisplayName = "Teleport")
};

UENUM(BlueprintType)
enum class EModifierScope : uint8
{
	Permanent UMETA(DisplayName = "Permanent"),
	CurrentRegion UMETA(DisplayName = "CurrentRegion"),
	NextRegion UMETA(DisplayName = "NextRegion")
};

USTRUCT(BlueprintType)
struct FIconImageRow : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Icon Config")
	FName ResourceType = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Icon Config")
	TSoftObjectPtr<UTexture2D> IconTexture = nullptr;
};

USTRUCT(BlueprintType)
struct FHexNodeData
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HexMap")
	FIntVector2 GridCoordinate = FIntVector2::ZeroValue;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HexMap")
	EHexNodeType NodeType = EHexNodeType::Entrance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HexMap")
	EHexNodeState NodeState = EHexNodeState::Masked;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HexMap")
	bool bHasBeenVisited = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HexMap")
	int32 ContentSeed = 0;
};

USTRUCT(BlueprintType)
struct FHexRegionData
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HexMap")
	EHexRegionType RegionType = EHexRegionType::Wood;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HexMap")
	TMap<FIntVector2, FHexNodeData> Nodes;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HexMap")
	bool bIsGenerated = false;
};

USTRUCT(BlueprintType)
struct FHexNodeTriggerResult
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HexMap")
	EHexNodeType NodeType = EHexNodeType::None;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HexMap")
	FName ContentID = NAME_None;
};

USTRUCT(BlueprintType)
struct FHexEventOption
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hex Event")
	FText OptionTitle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hex Event")
	FText OptionDescription;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hex Event")
	bool bIsAvailable = true;
};

USTRUCT(BlueprintType)
struct FHexEventConfig : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hex Event")
	FName EventID = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hex Event")
	EHexRegionType TargetRegion = EHexRegionType::Wood;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hex Event")
	int32 MultiStageNumber = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hex Event")
	TArray<int32> OptionPerStage = TArray<int32>();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hex Event")
	TSoftObjectPtr<UTexture2D> EventImage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hex Event")
	TArray<FText> EventTitle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hex Event")
	TArray<FText> EventDescription;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hex Event")
	TArray<FHexEventOption> Options;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hex Event")
	TSubclassOf<UHexEventLogicBase> LogicClass = nullptr;
};
