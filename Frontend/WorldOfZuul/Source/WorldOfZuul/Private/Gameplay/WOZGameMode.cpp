// Fill out your copyright notice in the Description page of Project Settings.

#include "Gameplay/WOZGameMode.h"
#include "Gameplay/WOZGameRoom.h"

void AWOZGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	GenerateMap(FIntPoint(3, 3));
}

void AWOZGameMode::GenerateMap(const FIntPoint& HalfSize)
{
	for (int i = -HalfSize.X; i <= HalfSize.X;i++)
	{
		for (int j = -HalfSize.Y; j <= HalfSize.Y;j++)
		{
			AWOZGameRoom* Room = AWOZGameRoom::CreateRoom(this,FIntPoint(i, j), GameplayData);
			if (Room)
			{
				Rooms.Emplace(Room);
			}
		}
	}
}

void AWOZGameMode::GenerateMap(const TArray<FWOZGameRoomData>& RoomData)
{
	for (const FWOZGameRoomData& Data : RoomData)
	{
		AWOZGameRoom* Room = AWOZGameRoom::CreateRoom(this,Data.Position, GameplayData);
		if (Room)
		{
			Rooms.Emplace(Room);
		}
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
