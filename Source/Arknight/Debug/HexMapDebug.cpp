// HexMapDebug.cpp

#include "HexMapDebug.h"
#include "Kismet/GameplayStatics.h"
#include "../URougeliteRunSubsystem.h"
#include "../HexMap/Controller/HexMapSubsystem.h"
#include "Engine/GameInstance.h"

void AHexMapDebug::BeginPlay()
{
	Super::BeginPlay();

	UGameInstance* GI = UGameplayStatics::GetGameInstance(this);
	if (GI)
	{
		if (URougeliteRunSubsystem* Subsystem = GI->GetSubsystem<URougeliteRunSubsystem>())
		{
			Subsystem->GlobalResource.FindOrAdd(EResourceType::AP) = initialAP;
		}

		if(UHexMapSubsystem* HexMapSubsystem = GI->GetSubsystem<UHexMapSubsystem>())
		{
			HexMapSubsystem->InitializeNewRun(FMath::Rand());
			HexMapSubsystem->EnterRegion(EHexRegionType::Wood);
			UGameplayStatics::OpenLevel(this, FName("Lvl_HexMap"));
		}
	}

}
