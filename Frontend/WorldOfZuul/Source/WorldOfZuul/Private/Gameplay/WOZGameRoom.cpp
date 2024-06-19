// Fill out your copyright notice in the Description page of Project Settings.

#include "Gameplay/WOZGameRoom.h"
#include "Gameplay/WOZGameItem.h"
#include "Gameplay/WOZGameplayData.h"
#include "Kismet/KismetMathLibrary.h"


AWOZGameRoom::AWOZGameRoom()
{
	PrimaryActorTick.bCanEverTick = true;
}

AWOZGameRoom* AWOZGameRoom::CreateRoom(UObject* WorldContext, const FIntPoint& Pos, UWOZGameplayData* GameplayData)
{
	check(WorldContext && GameplayData && GameplayData->RoomClass && GameplayData->ItemClass);
	AWOZGameRoom* Room = Cast<AWOZGameRoom>(WorldContext->GetWorld()->SpawnActor(GameplayData->RoomClass));
	
	Room->GameplayData = GameplayData;
	Room->Position = Pos;
	const FVector& BaseLocation = GameplayData->RoomSize * FVector(Room->Position.X, Room->Position.Y, 0);
	Room->SetActorLocation(BaseLocation);

	TArray<FIntPoint> SpawnedPosition;
	const int32 MaxItemEnumNum = EWOZGameItem::MAX - 1;
	int32 ItemSpawnCount = FMath::RandRange(GameplayData->RoomItemSpawnCountRange.X, GameplayData->RoomItemSpawnCountRange.Y);
	TArray<TEnumAsByte<EWOZGameItem::Type>> ItemsToSpawn;
	while (ItemSpawnCount--)
	{
		const int32 RandItem = FMath::RandRange(1, MaxItemEnumNum);
		const EWOZGameItem::Type RandItemEnum =  (EWOZGameItem::Type)RandItem;

		if (!UKismetMathLibrary::RandomBoolWithWeight(GameplayData->RoomItemSpawnRate.FindRef(RandItemEnum) / 100.f))
		{
			//++ItemSpawnCount;
			continue;
		}

		ItemsToSpawn.Emplace(RandItemEnum);
	}

	Room->AddNewItems(ItemsToSpawn);

	return Room;
}

AWOZGameRoom* AWOZGameRoom::CreateRoom(UObject* WorldContext, FWOZGameRoomData* InRoomData, UWOZGameplayData* GameplayData)
{
	check(WorldContext && GameplayData && GameplayData->RoomClass && GameplayData->ItemClass);
	AWOZGameRoom* Room = Cast<AWOZGameRoom>(WorldContext->GetWorld()->SpawnActor(GameplayData->RoomClass));

	Room->GameplayData = GameplayData;
	Room->Position = InRoomData->Position;
	const FVector& BaseLocation = GameplayData->RoomSize * FVector(Room->Position.X, Room->Position.Y, 0);
	
	for (const auto& ItemEnum : InRoomData->Items)
	{
		AWOZGameItem* Item = AWOZGameItem::CreateItem(Room, ItemEnum.Key, ItemEnum.Value, GameplayData);
		if (Item)
		{
			Room->Items.Emplace(Item);
			Item->SetActorLocation(BaseLocation + FVector(ItemEnum.Value.X, ItemEnum.Value.Y, 64.f));
		}
	}
	
	return Room;
}

void AWOZGameRoom::AddNewItem(TEnumAsByte<EWOZGameItem::Type> ItemEnum)
{
	TArray<FIntPoint> SpawnedPosition;
	for (AWOZGameItem* Item : Items)
	{
		SpawnedPosition.Emplace(Item->GetPosition());
	}

	const FVector& BaseLocation = GameplayData->RoomSize * FVector(Position.X, Position.Y, 0);
	const int32 MaxItemRowColumn = (int32)(GameplayData->RoomSize / GameplayData->ItemSize / 2) - 1;
	do
	{
		FIntPoint RandPosition;
		RandPosition.X = FMath::RandRange(-MaxItemRowColumn, MaxItemRowColumn);
		RandPosition.Y = FMath::RandRange(-MaxItemRowColumn, MaxItemRowColumn);

		if (SpawnedPosition.Contains(RandPosition))
		{
			continue;
		}
		
		AWOZGameItem* Item = AWOZGameItem::CreateItem(this, ItemEnum, FIntPoint(RandPosition.X, RandPosition.Y), GameplayData);
		if (Item)
		{
			SpawnedPosition.Emplace(RandPosition);
			Items.Emplace(Item);
			Item->SetActorLocation(BaseLocation + FVector(RandPosition.X * GameplayData->ItemSize, RandPosition.Y * GameplayData->ItemSize, 64.f));
		}
	}
	while (false);
}

void AWOZGameRoom::AddNewItems(const TArray<TEnumAsByte<EWOZGameItem::Type>>& ItemEnums)
{
	TArray<FIntPoint> SpawnedPosition;
	for (AWOZGameItem* Item : Items)
	{
		SpawnedPosition.Emplace(Item->GetPosition());
	}

	const FVector& BaseLocation = GameplayData->RoomSize * FVector(Position.X, Position.Y, 0);
	
	const int32 MaxItemRowColumn = (int32)(GameplayData->RoomSize / GameplayData->ItemSize / 2) - 1;
	for (int32 i = 0;i < ItemEnums.Num();i++)
	{
		const EWOZGameItem::Type ItemEnum = ItemEnums[i];
		
		FIntPoint RandPosition;
		RandPosition.X = FMath::RandRange(-MaxItemRowColumn, MaxItemRowColumn);
		RandPosition.Y = FMath::RandRange(-MaxItemRowColumn, MaxItemRowColumn);

		if (SpawnedPosition.Contains(RandPosition))
		{
			--i;
			continue;
		}
		
		AWOZGameItem* Item = AWOZGameItem::CreateItem(this, ItemEnum, FIntPoint(RandPosition.X, RandPosition.Y), GameplayData);
		if (Item)
		{
			SpawnedPosition.Emplace(RandPosition);
			Items.Emplace(Item);
			Item->SetActorLocation(BaseLocation + FVector(RandPosition.X * GameplayData->ItemSize, RandPosition.Y * GameplayData->ItemSize, 64.f));
		}
	}
}

void AWOZGameRoom::RemoveItems(const TArray<AWOZGameItem*>& ItemsToRemove)
{
	for (AWOZGameItem* Item : ItemsToRemove)
	{
		if (!Item || !Items.Contains(Item)) continue;
		Item->Destroy();
		Items.RemoveSingle(Item);
	}
}

void AWOZGameRoom::ReplaceItem(AWOZGameItem* OldItem, EWOZGameItem::Type NewItemEnum)
{
	if (!OldItem || !Items.Contains(OldItem)) return;

	OldItem->UpdateItem(NewItemEnum, GameplayData);
}

FWOZGameRoomData AWOZGameRoom::GetRoomData()
{
	FWOZGameRoomData RoomData;
	RoomData.Position = Position;

	for (AWOZGameItem* Item : Items)
	{
		if (Item)
		{
			RoomData.Items.Emplace(Item->GetItemEnum(), Item->GetPosition());
		}
	}

	return RoomData;
}
