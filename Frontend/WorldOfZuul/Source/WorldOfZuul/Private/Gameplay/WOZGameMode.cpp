// Fill out your copyright notice in the Description page of Project Settings.

#include "Gameplay/WOZGameMode.h"
#include "Gameplay/WOZGameInstance.h"
#include "Gameplay/WOZGameRoom.h"

void AWOZGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
	
	if (GetNetMode() == NM_Standalone)
	{
		UWOZGameInstance* GameInstance = Cast<UWOZGameInstance>(GetGameInstance());
		check(GameInstance);

		if (!GameInstance->bIsNewGame) return;
		
		GenerateMap(GameInstance->SinglePlayerSaveGameData.RoomDatas);
		return;
	}

	GenerateMap(FIntPoint(4, 4));
}

void AWOZGameMode::GenerateMap(const FIntPoint& HalfSize)
{
	check(GameplayData);

	TArray<FIntPoint> RoomPos;

	int32 RoomInfoMaxIndex = GameplayData->RoomInfos.Num() - 1;
	for (int i = -HalfSize.X; i <= HalfSize.X;i++)
	{
		for (int j = -HalfSize.Y; j <= HalfSize.Y;j++)
		{
			const int32 RandIndex = FMath::RandRange(0, RoomInfoMaxIndex);
			const FWOZGameRoomInfo& RandRoomInfo = GameplayData->RoomInfos[RandIndex];
			AWOZGameRoom* Room = AWOZGameRoom::CreateRoom(this,RandRoomInfo, FIntPoint(i, j), GameplayData);
			if (Room)
			{
				RoomPos.Emplace(FIntPoint(i, j));
				Rooms.Emplace(Room);
			}
		}
	}

	for (AWOZGameRoom* Room : Rooms)
	{
		Room->InitDoors(RoomPos);
	}
}

void AWOZGameMode::GenerateMap(const TArray<FWOZGameRoomData>& RoomData)
{
	TArray<FIntPoint> RoomPos;
	for (const FWOZGameRoomData& Data : RoomData)
	{
		AWOZGameRoom* Room = AWOZGameRoom::CreateRoom(this, Data, GameplayData);
		if (Room)
		{
			RoomPos.Emplace(Data.Position);
			Rooms.Emplace(Room);
		}
	}

	for (AWOZGameRoom* Room : Rooms)
	{
		Room->InitDoors(RoomPos);
	}
}

AWOZGameRoom* AWOZGameMode::GetRoomByPosition(const FIntPoint& Position)
{
	for (AWOZGameRoom* Room : Rooms)
	{
		if (Room->GetRoomData().Position == Position)
		{
			return Room;
		}
	}

	return nullptr;
}
