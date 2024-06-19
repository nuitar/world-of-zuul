// Fill out your copyright notice in the Description page of Project Settings.

#include "Gameplay/WOZGameplayData.h"
#include "Kismet/KismetMathLibrary.h"

FString UWOZGameplayData::GetItemNameString(EWOZGameItem::Type ItemEnum) const
{
	return Items.FindRef(ItemEnum).Name.ToString();
}

FString UWOZGameplayData::GetItemDefaultCommandString(EWOZGameItem::Type ItemEnum) const
{
	return Items.FindRef(ItemEnum).Strings[0];
}

EWOZGameItemType::Type UWOZGameplayData::GetItemType(EWOZGameItem::Type ItemEnum) const
{
	return Items.FindRef(ItemEnum).Type;
}

EWOZGameItem::Type UWOZGameplayData::GetItemByString(const FString& Str)
{
	for (const TTuple<TEnumAsByte<EWOZGameItem::Type>, FWOZGameItemInfo>& Result : Items)
	{
		if (Result.Value.Strings.Contains(Str))
		{
			return Result.Key;
			break;
		}
	}

	return EWOZGameItem::None;
}

TMap<EWOZGameItem::Type, int32> UWOZGameplayData::GetItemCounterMap(const TArray<TEnumAsByte<EWOZGameItem::Type>>& ItemEnums)
{
	TMap<EWOZGameItem::Type, int32> Counter;
	for (EWOZGameItem::Type Enum : ItemEnums)
	{
		if (Counter.Contains(Enum))
		{
			Counter[Enum]++;
		}
		else
		{
			Counter.Emplace(Enum, 1);
		}
	}

	return Counter;
}

FString UWOZGameplayData::ItemCounterToString(const TMap<EWOZGameItem::Type, int32>& Counter) const
{
	FString RetMsg;
	for (const auto& Tuple : Counter)
	{
		RetMsg += FString::FromInt(Tuple.Value);
		RetMsg += Items.FindRef(Tuple.Key).Quantifier.ToString();
		RetMsg += GetItemNameString(Tuple.Key) + "\n";
	}
	RetMsg.RemoveFromEnd("\n");

	return RetMsg;
}

FString UWOZGameplayData::GetStringCommand(EWOZCommand::Type Command)
{
	for (const auto& StringCommand : StringCommands)
	{
		if (StringCommand.Value == Command)
		{
			return StringCommand.Key;
		}
	}

	return FString();
}

TArray<TEnumAsByte<EWOZGameItem::Type>> UWOZGameplayData::GetTreasureFromBox(EWOZGameItem::Type TreasureBox) const
{
	TArray<TEnumAsByte<EWOZGameItem::Type>> Treasure;
	int32 Count = 0;
	TMap<TEnumAsByte<EWOZGameItem::Type>, float> Rate;
	switch (TreasureBox)
	{
	case EWOZGameItem::GoldenTreasureBox:
		Count = FMath::RandRange(GoldenTreasureRange.X, GoldenTreasureRange.Y);
		Rate = GoldenTreasureRate;
		break;

	case EWOZGameItem::SilverTreasureBox:
		Count = FMath::RandRange(SilverTreasureRange.X, SilverTreasureRange.Y);
		Rate = SilverTreasureRate;
		break;

	case EWOZGameItem::CopperTreasureBox:
		Count = FMath::RandRange(CopperTreasureRange.X, CopperTreasureRange.Y);
		Rate = CopperTreasureRate;
		break;
		
	default: ;
		return TArray<TEnumAsByte<EWOZGameItem::Type>>{};
	}

	const int32 MaxItemEnumNum = EWOZGameItem::MAX - 1;
	
	while (Count--)
	{
		const int32 RandItem = FMath::RandRange(1, MaxItemEnumNum);
		const EWOZGameItem::Type RandItemEnum =  (EWOZGameItem::Type)RandItem;

		if (!UKismetMathLibrary::RandomBoolWithWeight(Rate.FindRef(RandItemEnum) / 100.f))
		{
			//++Count;
			continue;
		}

		Treasure.Emplace(RandItemEnum);
	}

	Treasure.Sort();
	return Treasure;
}

int32 UWOZGameplayData::GetScoreFromBox(EWOZGameItem::Type TreasureBox) const
{
	switch (TreasureBox)
	{
	case EWOZGameItem::GoldenTreasureBox:
		return FMath::RandRange(GoldenTreasureBoxScoreRange.X, GoldenTreasureBoxScoreRange.Y);
		
	case EWOZGameItem::SilverTreasureBox:
		return FMath::RandRange(SilverTreasureBoxScoreRange.X, SilverTreasureBoxScoreRange.Y);

	case EWOZGameItem::CopperTreasureBox:
		return FMath::RandRange(CopperTreasureBoxScoreRange.X, CopperTreasureBoxScoreRange.Y);

	default: ;
	}

	return 0;
}

bool UWOZGameplayData::IsTakeableItem(EWOZGameItem::Type Item) const
{
	const EWOZGameItemType::Type ItemType = GetItemType(Item);
	switch (ItemType)
	{
	case EWOZGameItemType::Key:
	case EWOZGameItemType::Food:
		return true;
		
	default: ;
	}

	return false;
}

bool UWOZGameplayData::IsOpenableItem(EWOZGameItem::Type Item) const
{
	const EWOZGameItemType::Type ItemType = GetItemType(Item);
	switch (ItemType)
	{
	case EWOZGameItemType::TreasureBox:
		return true;
		
	default: ;
	}

	return false;
}

bool UWOZGameplayData::IsUseableItem(EWOZGameItem::Type Item) const
{
	const EWOZGameItemType::Type ItemType = GetItemType(Item);
	switch (ItemType)
	{
	case EWOZGameItemType::Key:
		return true;
		
	default: ;
	}

	return false;
}

bool UWOZGameplayData::IsEatableItem(EWOZGameItem::Type Item) const
{
	const EWOZGameItemType::Type ItemType = GetItemType(Item);
	switch (ItemType)
	{
	case EWOZGameItemType::Food:
		return true;
		
	default: ;
	}

	return false;
}
