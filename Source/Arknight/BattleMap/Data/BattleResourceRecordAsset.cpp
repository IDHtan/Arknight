#include "BattleResourceRecordAsset.h"

void UBattleResourceRecordAsset::ResetRecord()
{
	ResourceGains.Empty();
	ResourceGains.Add(EResourceType::Wood, 0);
	ResourceGains.Add(EResourceType::Rock, 0);
	ResourceGains.Add(EResourceType::Metal, 0);
	ResourceGains.Add(EResourceType::Grain, 0);
}

void UBattleResourceRecordAsset::AddResourceGain(EResourceType ResourceType, int32 Amount)
{
	if (Amount <= 0)
	{
		return;
	}

	ResourceGains.FindOrAdd(ResourceType) += Amount;
}

int32 UBattleResourceRecordAsset::GetResourceGain(EResourceType ResourceType) const
{
	if (const int32* FoundAmount = ResourceGains.Find(ResourceType))
	{
		return *FoundAmount;
	}

	return 0;
}
