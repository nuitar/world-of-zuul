// Fill out your copyright notice in the Description page of Project Settings.

#include "Gameplay/WOZGameRoom.h"
#include "Components/BoxComponent.h"
#include "Gameplay/WOZGameItem.h"
#include "Gameplay/WOZGameplayData.h"
#include "Gameplay/WOZPlayerCharacter.h"
#include "Gameplay/WOZPlayerController.h"
#include "Kismet/KismetMathLibrary.h"


AWOZGameRoom::AWOZGameRoom()
{
	PrimaryActorTick.bCanEverTick = true;

	Floor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Plane"));
	SetRootComponent(Floor);

	Door_East = CreateDefaultSubobject<UBoxComponent>(TEXT("Door_East"));
	Door_East->SetupAttachment(GetRootComponent());

	Door_South = CreateDefaultSubobject<UBoxComponent>(TEXT("Door_South"));
	Door_South->SetupAttachment(GetRootComponent());

	Door_West = CreateDefaultSubobject<UBoxComponent>(TEXT("Door_West"));
	Door_West->SetupAttachment(GetRootComponent());

	Door_North = CreateDefaultSubobject<UBoxComponent>(TEXT("Door_North"));
	Door_North->SetupAttachment(GetRootComponent());
}

AWOZGameRoom* AWOZGameRoom::CreateRoom(UObject* WorldContext, const FWOZGameRoomInfo& Info, const FIntPoint& Pos, UWOZGameplayData* GameplayData)
{
	check(WorldContext && GameplayData && GameplayData->RoomClass && GameplayData->ItemClass);
	AWOZGameRoom* Room = Cast<AWOZGameRoom>(WorldContext->GetWorld()->SpawnActor(GameplayData->RoomClass));
	
	Room->GameplayData = GameplayData;
	Room->SetRoomInfo(Info);
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

AWOZGameRoom* AWOZGameRoom::CreateRoom(UObject* WorldContext, const FWOZGameRoomData& InRoomData, UWOZGameplayData* GameplayData)
{
	check(WorldContext && GameplayData && GameplayData->RoomClass && GameplayData->ItemClass);
	AWOZGameRoom* Room = Cast<AWOZGameRoom>(WorldContext->GetWorld()->SpawnActor(GameplayData->RoomClass));

	Room->GameplayData = GameplayData;
	Room->SetRoomInfo(InRoomData.Info);
	Room->Position = InRoomData.Position;
	const FVector& BaseLocation = GameplayData->RoomSize * FVector(Room->Position.X, Room->Position.Y, 0);
	
	for (const auto& ItemEnum : InRoomData.Items)
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

void AWOZGameRoom::SetRoomInfo(const FWOZGameRoomInfo& Info)
{
	RoomInfo = Info;

	if (!MaterialInstanceDynamic)
	{
		UMaterialInterface* Material = Floor->GetMaterial(0);
		if (Material)
		{
			MaterialInstanceDynamic = UMaterialInstanceDynamic::Create(Material, this);
			Floor->SetMaterial(0, MaterialInstanceDynamic);
		}	
	}
	
	MaterialInstanceDynamic->SetTextureParameterValue(FName("Texture"), RoomInfo.Floor);
}

void AWOZGameRoom::InitDoors(const TArray<FIntPoint>& MapRoomPos)
{
	//East
	if (MapRoomPos.Contains(FIntPoint(Position.X, Position.Y + 1)))
	{
		Door_East->AddRelativeLocation(FVector(0.0, 9.0, 0.0));
		Door_East->SetCollisionProfileName(FName("OverlapAllDynamic"));
		Door_East->OnComponentBeginOverlap.AddDynamic(this, &AWOZGameRoom::OnDoorEastOverlap);
	}
	else
	{
		Door_East->SetCollisionProfileName(FName("BlockAllDynamic"));
	}
	
	//South
	if (MapRoomPos.Contains(FIntPoint(Position.X - 1, Position.Y)))
	{
		Door_South->AddRelativeLocation(FVector(-9.0, 0.0, 0.0));
		Door_South->SetCollisionProfileName(FName("OverlapAllDynamic"));
		Door_South->OnComponentBeginOverlap.AddDynamic(this, &AWOZGameRoom::OnDoorSouthOverlap);
	}
	else
	{
		Door_South->SetCollisionProfileName(FName("BlockAllDynamic"));
	}
	
	//West
	if (MapRoomPos.Contains(FIntPoint(Position.X, Position.Y - 1)))
	{
		Door_West->AddRelativeLocation(FVector(0.0, -9.0, 0.0));
		Door_West->SetCollisionProfileName(FName("OverlapAllDynamic"));
		Door_West->OnComponentBeginOverlap.AddDynamic(this, &AWOZGameRoom::OnDoorWestOverlap);
	}
	else
	{
		Door_West->SetCollisionProfileName(FName("BlockAllDynamic"));
	}

	//North
	if (MapRoomPos.Contains(FIntPoint(Position.X + 1, Position.Y)))
	{
		Door_North->AddRelativeLocation(FVector(9.0, 0.0, 0.0));
		Door_North->SetCollisionProfileName(FName("OverlapAllDynamic"));
		Door_North->OnComponentBeginOverlap.AddDynamic(this, &AWOZGameRoom::OnDoorNorthOverlap);
	}
	else
	{
		Door_North->SetCollisionProfileName(FName("BlockAllDynamic"));
	}
}

void AWOZGameRoom::AddNewItem(TEnumAsByte<EWOZGameItem::Type> ItemEnum)
{
	TArray<FIntPoint> SpawnedPosition;
	for (AWOZGameItem* Item : Items)
	{
		SpawnedPosition.Emplace(Item->GetPosition());
	}

	const FVector& BaseLocation = GameplayData->RoomSize * FVector(Position.X, Position.Y, 0);
	const int32 MaxItemRowColumn = (int32)(0.75f * GameplayData->RoomSize / GameplayData->ItemSize / 2) - 1;
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
			Item->SetActorScale3D(FVector(GameplayData->Items.FindRef(ItemEnum).Scale));
		}
	}
	while (false);
}

void AWOZGameRoom::AddNewItems(const TArray<TEnumAsByte<EWOZGameItem::Type>>& InItemEnums)
{
	TArray<TEnumAsByte<EWOZGameItem::Type>> ItemEnums = InItemEnums;

	//每个房间最多只能有一个传送门
	if (ItemEnums.Contains(EWOZGameItem::TeleportDoor))
	{
		ItemEnums.Remove(EWOZGameItem::TeleportDoor);
		ItemEnums.Emplace(EWOZGameItem::TeleportDoor);
	}
	ItemEnums.Sort();

	TArray<FIntPoint> SpawnedPosition;
	for (AWOZGameItem* Item : Items)
	{
		SpawnedPosition.Emplace(Item->GetPosition());
	}

	const FVector& BaseLocation = GameplayData->RoomSize * FVector(Position.X, Position.Y, 0);
	
	const int32 MaxItemRowColumn = (int32)(0.75f * GameplayData->RoomSize / GameplayData->ItemSize / 2) - 1;
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

		//传送门不能在正中间
		if (ItemEnum == EWOZGameItem::TeleportDoor && RandPosition == FIntPoint(0, 0))
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
			Item->SetActorScale3D(FVector(GameplayData->Items.FindRef(ItemEnum).Scale));
		}
	}
}

void AWOZGameRoom::RemoveItem(AWOZGameItem* ItemToRemove)
{
	if (!ItemToRemove || !Items.Contains(ItemToRemove)) return;;
	ItemToRemove->Destroy();
	Items.RemoveSingle(ItemToRemove);
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

TArray<EWOZGameItem::Type> AWOZGameRoom::GetAllItemEnums() const
{
	TArray<EWOZGameItem::Type> ItemEnums;
	for (AWOZGameItem* Item : Items)
	{
		ItemEnums.Emplace(Item->GetItemEnum());
	}

	return ItemEnums;
}

FWOZGameRoomData AWOZGameRoom::GetRoomData()
{
	FWOZGameRoomData RoomData;
	RoomData.Info = RoomInfo;
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

void AWOZGameRoom::OnDoorEastOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	DoorOverlap(OtherActor, EWOZGameRoomDirection::East);
}

void AWOZGameRoom::OnDoorSouthOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	DoorOverlap(OtherActor, EWOZGameRoomDirection::South);
}

void AWOZGameRoom::OnDoorWestOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	DoorOverlap(OtherActor, EWOZGameRoomDirection::West);
}

void AWOZGameRoom::OnDoorNorthOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	DoorOverlap(OtherActor, EWOZGameRoomDirection::North);
}

void AWOZGameRoom::DoorOverlap(AActor* Actor, EWOZGameRoomDirection::Type Direction)
{
	if (AWOZPlayerCharacter* PlayerCharacter = Cast<AWOZPlayerCharacter>(Actor))
	{
		if (AWOZPlayerController* PlayerController = Cast<AWOZPlayerController>(PlayerCharacter->GetController()))
		{
			check(GameplayData);
			FString Str = GameplayData->GetStringCommand(EWOZCommand::Go) + " ";
			Str += GameplayData->GetStringDirection(Direction);
			PlayerController->ExecuteCommand(Str);
		}
	}
}
