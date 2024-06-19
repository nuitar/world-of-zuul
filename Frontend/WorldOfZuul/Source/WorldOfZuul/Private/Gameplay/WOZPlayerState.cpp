// Fill out your copyright notice in the Description page of Project Settings.

#include "Gameplay/WOZPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

AWOZPlayerState::AWOZPlayerState()
{
	SetReplicates(true);
}

AWOZPlayerState* AWOZPlayerState::Get(const UObject* WorldContext)
{
	if (!WorldContext) return nullptr;

	return Cast<AWOZPlayerState>(UGameplayStatics::GetPlayerState(WorldContext, 0));
}

void AWOZPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AWOZPlayerState, BagItems, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AWOZPlayerState, MaxWeight, COND_OwnerOnly);
}

void AWOZPlayerState::AddCommandProcessMsg(const FWOZCommandReplyMsg& Msg)
{
	
}

void AWOZPlayerState::AddScore(int32 Add)
{
	GameScore += Add;
	
	if (HasAuthority())
	{
		OnScoreUpdated.Broadcast(this);
	}
}

void AWOZPlayerState::AddBagItem(EWOZGameItem::Type ItemToAdd)
{
	BagItems.Emplace(ItemToAdd);

	if (HasAuthority())
	{
		OnBagItemUpdated.Broadcast(this);
	}
}

void AWOZPlayerState::AddBagItems(TArray<TEnumAsByte<EWOZGameItem::Type>> ItemsToAdd)
{
	if (ItemsToAdd.IsEmpty()) return;
	BagItems.Append(ItemsToAdd);
	BagItems.Sort();

	if (HasAuthority())
	{
		OnBagItemUpdated.Broadcast(this);
	}
}

void AWOZPlayerState::RemoveBagItem(EWOZGameItem::Type ItemToRemove)
{
	if (!BagItems.Contains(ItemToRemove)) return;
	
	BagItems.RemoveSingle(ItemToRemove);
	
	if (HasAuthority())
	{
		OnBagItemUpdated.Broadcast(this);
	}
}

void AWOZPlayerState::RemoveBagItems(TArray<TEnumAsByte<EWOZGameItem::Type>> ItemsToRemove)
{
	if (ItemsToRemove.IsEmpty()) return;
	for (const TEnumAsByte<EWOZGameItem::Type>& Item : ItemsToRemove)
	{
		BagItems.RemoveSingle(Item);
	}

	if (HasAuthority())
	{
		OnBagItemUpdated.Broadcast(this);
	}
}

void AWOZPlayerState::PushRoomPosition(const FIntPoint& Pos)
{
	RoomPositionHistory.Emplace(Pos);

	if (HasAuthority())
	{
		OnRoomPositionHistoryUpdated.Broadcast(this);
	}
}

void AWOZPlayerState::PopRoomPosition()
{
	if (RoomPositionHistory.Num() > 1)
	{
		RoomPositionHistory.RemoveAt(RoomPositionHistory.Num() - 1);
	}

	if (HasAuthority())
	{
		OnRoomPositionHistoryUpdated.Broadcast(this);
	}
}

int32 AWOZPlayerState::GetCurrentWeight(UWOZGameplayData* GameplayData)
{
	check(GameplayData);

	int32 WeightSum = 0;
	for (TEnumAsByte<EWOZGameItem::Type> Item : BagItems)
	{
		WeightSum += GameplayData->Items.FindRef(Item).Weight;
	}

	return WeightSum;
}

void AWOZPlayerState::AddMaxWeight(int32 Add)
{
	MaxWeight += Add;

	if (HasAuthority())
	{
		OnMaxWeightUpdated.Broadcast(this);
	}
}

void AWOZPlayerState::OnRep_BagItems()
{
	OnBagItemUpdated.Broadcast(this);
}

void AWOZPlayerState::OnRep_MaxWeight()
{
	OnMaxWeightUpdated.Broadcast(this);
}

void AWOZPlayerState::OnRep_RoomPositionHistory()
{
	OnRoomPositionHistoryUpdated.Broadcast(this);
}

void AWOZPlayerState::OnRep_GameScore()
{
	OnScoreUpdated.Broadcast(this);
}
