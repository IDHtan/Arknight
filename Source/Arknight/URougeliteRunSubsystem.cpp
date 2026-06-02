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
	CheckOperators();
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
		}
		UE_LOG(LogTemp, Warning, TEXT("load success"));
	}
	else
	{
		CurrentSaveGame = Cast<URougeliteSaveGame>(UGameplayStatics::CreateSaveGameObject(URougeliteSaveGame::StaticClass()));
		UE_LOG(LogTemp, Warning, TEXT("no save found, create new"));
	}

	GlobaleResource = CurrentSaveGame ? CurrentSaveGame->GlobalResources : TMap<EResourceType, int32>();
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
		if (Row && !CurrentSaveGame->OperatorLevels.Contains(Row->OperatorName)) {
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

	CurrentSaveGame->GlobalResources.FindOrAdd(Type) += Amount;
	GlobaleResource.FindOrAdd(Type) += Amount;
}

void URougeliteRunSubsystem::AddGameResource(EResourceType Type, int32 Amount)
{
	if (Amount <= 0)
	{
		return;
	}

	if (!CurrentSaveGame)
	{
		UE_LOG(LogTemp, Warning, TEXT("AddGameResource skipped: CurrentSaveGame is null"));
		return;
	}

	CurrentSaveGame->GlobalResources.FindOrAdd(Type) += Amount;
	GlobaleResource.FindOrAdd(Type) += Amount;
}


