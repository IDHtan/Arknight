// Fill out your copyright notice in the Description page of Project Settings.


#include "URougeliteRunSubsystem.h"
#include "Kismet/GameplayStatics.h"

void URougeliteRunSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	SaveSlotName = TEXT("MainSaveSlot");
	PendingBattleLevelID = NAME_None;
	CurrentAP = 0;

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
		UE_LOG(LogTemp, Warning, TEXT("load success"));
	}
	else
	{
		CurrentSaveGame = Cast<URougeliteSaveGame>(UGameplayStatics::CreateSaveGameObject(URougeliteSaveGame::StaticClass()));
		UE_LOG(LogTemp, Warning, TEXT("no save found, create new"));
	}
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
	CurrentSaveGame->OperatorLevels[OperatorName]++;
	FOperatorRosterData* Founded = GlobalRoster.FindByPredicate([OperatorName](const FOperatorRosterData& Data)
		{
			return Data.OperatorName == OperatorName;
		});
	if (Founded) {
		Founded->OperatorLevel = CurrentSaveGame->OperatorLevels[OperatorName];
	}
}


void URougeliteRunSubsystem::PrepareForBattle(FName LevelID)
{
	UWorld* World = GetWorld();
	if (World)
	{
		UGameplayStatics::OpenLevel(World, LevelID);
	}
}

FName URougeliteRunSubsystem::ConsumePendingBattleID()
{
	if(PendingBattleLevelID !=NAME_None)
	{
		FName Result = PendingBattleLevelID;
		PendingBattleLevelID = NAME_None;
		return Result;
	}
	return NAME_None;
}

void URougeliteRunSubsystem::AddResource(EResourceType Type, int32 Amount)
{
	CurrentSaveGame->GlobalResources.FindOrAdd(Type) += Amount;
}

void URougeliteRunSubsystem::ConcludeBattle() //unfinished, just for testing
{
	
}
