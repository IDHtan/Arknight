// Fill out your copyright notice in the Description page of Project Settings.


#include "URougeliteRunSubsystem.h"
#include "HexMap/Controller/HexMapSubsystem.h"
#include "Kismet/GameplayStatics.h"

void URougeliteRunSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Collection.InitializeDependency(UHexMapSubsystem::StaticClass());
	Super::Initialize(Collection);

	SaveSlotName = TEXT("MainSaveSlot");
	MapManager = GetGameInstance() ? GetGameInstance()->GetSubsystem<UHexMapSubsystem>() : nullptr;

	LoadGame();
}

void URougeliteRunSubsystem::Deinitialize()
{
	SaveGame();
	Super::Deinitialize();
}

void URougeliteRunSubsystem::LoadGame()
{
	if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, 0)) {
		USaveGame* LoadedData = UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0);
		CurrentSaveGame = Cast<URougeliteSaveGame>(LoadedData);
		if (!CurrentSaveGame)
		{
			CurrentSaveGame = Cast<URougeliteSaveGame>(UGameplayStatics::CreateSaveGameObject(URougeliteSaveGame::StaticClass()));
			CurrentSaveGame->GlobalResources.Add(EResourceType::Food, 20);
		}
		UE_LOG(LogTemp, Warning, TEXT("load success"));
	}
	else
	{
		CurrentSaveGame = Cast<URougeliteSaveGame>(UGameplayStatics::CreateSaveGameObject(URougeliteSaveGame::StaticClass()));
		UE_LOG(LogTemp, Warning, TEXT("no save found, create new"));
	}

	GlobalResource = CurrentSaveGame ? CurrentSaveGame->GlobalResources : TMap<EResourceType, int32>();

	InitExchangeStock();
	CheckOperators();
}

void URougeliteRunSubsystem::SaveGame()
{
	if (CurrentSaveGame)
	{
		UGameplayStatics::SaveGameToSlot(CurrentSaveGame, SaveSlotName, 0);
		UE_LOG(LogTemp, Warning, TEXT("save success"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("no save game to save"));
	}
}

void URougeliteRunSubsystem::CheckOperators()
{
	if (!CurrentSaveGame)
	{
		UE_LOG(LogTemp, Warning, TEXT("CheckOperators skipped: CurrentSaveGame is null"));
		return;
	}

	const URougeliteSettings* Settings = GetDefault<URougeliteSettings>();
	UDataTable* Table = Settings->OperatorDictTable.LoadSynchronous();
	if (!Table)
	{
		UE_LOG(LogTemp, Warning, TEXT("OperatorDictTable not found"));
		return;
	}

	TArray<FOperatorDictionary*> AllRows;
	Table->GetAllRows<FOperatorDictionary>(TEXT("CheckOperators"), AllRows);
	for (const auto& Row : AllRows){
		if (Row && !CurrentSaveGame->OperatorLevels.Contains(Row->OperatorName))
		{
			CurrentSaveGame->OperatorLevels.Add(Row->OperatorName, 1);
			UE_LOG(LogTemp, Warning, TEXT("Added new operator: %s"), *Row->OperatorName.ToString());
		}
	}

	GlobalRoster.Empty();
	for (const auto& pair : CurrentSaveGame->OperatorLevels) {
		FOperatorDictionary* RowData = Table->FindRow<FOperatorDictionary>(pair.Key, TEXT("CheckOperators"));
		if (RowData) {
			FOperatorRosterData NewData;
			NewData.OperatorName = pair.Key;
			NewData.OperatorClass = RowData->OperatorClass;
			NewData.OperatorLevel = pair.Value;
			GlobalRoster.Add(NewData);
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("Operator %s not found in OperatorDictTable"), *pair.Key.ToString());
			CurrentSaveGame->OperatorLevels.Remove(pair.Key);
		}
	}

	if (FOperatorRosterData* Amiya = GlobalRoster.FindByPredicate(
		[](const FOperatorRosterData& D) { return D.OperatorName == FName(TEXT("Amiya")); }))
	{
		Amiya->bCanJoinBattle = false;
	}
}

void URougeliteRunSubsystem::UpgradeOperator(FName OperatorName)
{
	if (!CurrentSaveGame || !CurrentSaveGame->OperatorLevels.Contains(OperatorName))
	{
		UE_LOG(LogTemp, Warning, TEXT("UpgradeOperator failed: %s not found"), *OperatorName.ToString());
		return;
	}

	CurrentSaveGame->OperatorLevels[OperatorName]++;
	FOperatorRosterData* Founded = GlobalRoster.FindByPredicate([OperatorName](const FOperatorRosterData& Data)
		{
			return Data.OperatorName == OperatorName;
		});
	if (Founded) {
		Founded->OperatorLevel = CurrentSaveGame->OperatorLevels[OperatorName];
	}
}

void URougeliteRunSubsystem::AddOperator(FName OperatorName)
{
	// "Recruit" an operator by enabling them for battle. Does NOT modify
	// the roster array — only flips bCanJoinBattle on an existing entry.
	FOperatorRosterData* Found = GlobalRoster.FindByPredicate(
		[&OperatorName](const FOperatorRosterData& D) { return D.OperatorName == OperatorName; });

	if (!Found)
	{
		UE_LOG(LogTemp, Warning, TEXT("AddOperator: '%s' not found in GlobalRoster"), *OperatorName.ToString());
		return;
	}

	Found->bCanJoinBattle = true;

	// Persist to save
	if (CurrentSaveGame && !CurrentSaveGame->OperatorLevels.Contains(OperatorName))
	{
		CurrentSaveGame->OperatorLevels.Add(OperatorName, 1);
	}

	UE_LOG(LogTemp, Log, TEXT("AddOperator: '%s' unlocked for battle"), *OperatorName.ToString());
}

void URougeliteRunSubsystem::AddResource(EResourceType Type, int32 Amount)
{
	if (Amount <= 0)
	{
		return;
	}

	if (!CurrentSaveGame)
	{
		UE_LOG(LogTemp, Warning, TEXT("AddResource skipped: CurrentSaveGame is null"));
		return;
	}

	int32& Ref = CurrentSaveGame->GlobalResources.FindOrAdd(Type);
	Ref += Amount;
	GlobalResource.FindOrAdd(Type) =Ref;

	// Broadcast Money change so panels auto-refresh
	if (Type == EResourceType::Money)
	{
		OnMoneyChanged.Broadcast(Ref);
	}
}

void URougeliteRunSubsystem::AddGameResource(EResourceType Type, int32 Amount)
{
	AddResource(Type, Amount);
	UE_LOG(LogTemp, Log, TEXT("URougeliteRunSubsystem::AddGameResource: this function is deprecated, use AddResource instead"));
}

// ============================================================================
// BaseBuilding — Resource operations
// ============================================================================

bool URougeliteRunSubsystem::TryConsumeResource(EResourceType Type, int32 Amount)
{
	if (Amount <= 0 || !CurrentSaveGame)
	{
		return false;
	}

	int32* CurrentAmount = CurrentSaveGame->GlobalResources.Find(Type);
	if (!CurrentAmount || *CurrentAmount < Amount)
	{
		return false;
	}

	*CurrentAmount -= Amount;
	GlobalResource.FindOrAdd(Type) -= *CurrentAmount;

	// Broadcast Money change so panels auto-refresh
	if (Type == EResourceType::Money)
	{
		OnMoneyChanged.Broadcast(*CurrentAmount);
	}

	return true;
}

int32 URougeliteRunSubsystem::GetGlobalResourceAmount(EResourceType Type) const
{
	if (!CurrentSaveGame)
	{
		return 0;
	}
	return CurrentSaveGame->GlobalResources.FindRef(Type);
}

void URougeliteRunSubsystem::SetGlobalResourceAmount(EResourceType Type, int32 Amount)
{
	if (!CurrentSaveGame)
	{
		return;
	}

	CurrentSaveGame->GlobalResources.FindOrAdd(Type) = Amount;
	GlobalResource.FindOrAdd(Type) = Amount;

	// Broadcast Money change if applicable
	if (Type == EResourceType::Money)
	{
		OnMoneyChanged.Broadcast(Amount);
	}
}

// ============================================================================
// BaseBuilding — Exchange stock
// ============================================================================

void URougeliteRunSubsystem::InitExchangeStock()
{
	if (!CurrentSaveGame)
	{
		return;
	}

	// Only initialise if empty (first load or new save)
	if (CurrentSaveGame->ExchangeLimitedStock.Num() > 0)
	{
		return;
	}

	const URougeliteSettings* Settings = GetDefault<URougeliteSettings>();
	UDataTable* Table = Settings->ExchangeItemTable.LoadSynchronous();
	if (!Table)
	{
		UE_LOG(LogTemp, Warning, TEXT("InitExchangeStock: ExchangeItemTable not found"));
		return;
	}

	TArray<FInfrastructureExchangeItem*> AllRows;
	Table->GetAllRows<FInfrastructureExchangeItem>(TEXT("InitExchangeStock"), AllRows);
	for (const FInfrastructureExchangeItem* Row : AllRows)
	{
		if (Row && Row->bIsLimited)
		{
			CurrentSaveGame->ExchangeLimitedStock.Add(Row->ResourceType, Row->InitialStock);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("InitExchangeStock: initialised %d limited items"), CurrentSaveGame->ExchangeLimitedStock.Num());
}

int32 URougeliteRunSubsystem::GetExchangeStock(EResourceType Type) const
{
	if (!CurrentSaveGame)
	{
		return 0;
	}
	return CurrentSaveGame->ExchangeLimitedStock.FindRef(Type);
}

void URougeliteRunSubsystem::ReduceExchangeStock(EResourceType Type, int32 Amount)
{
	if (!CurrentSaveGame || Amount <= 0)
	{
		return;
	}

	int32* Stock = CurrentSaveGame->ExchangeLimitedStock.Find(Type);
	if (Stock && *Stock >= Amount)
	{
		*Stock -= Amount;
	}
}

// ============================================================================
// BaseBuilding — Facility unlock
// ============================================================================

void URougeliteRunSubsystem::UnlockTradeStation()
{
	if (!CurrentSaveGame)
	{
		return;
	}
	CurrentSaveGame->bHasUnlockedTradeStation = true;
	SaveGame();
}

void URougeliteRunSubsystem::UnlockExchange()
{
	if (!CurrentSaveGame)
	{
		return;
	}
	CurrentSaveGame->bHasUnlockedExchange = true;
	SaveGame();
}


