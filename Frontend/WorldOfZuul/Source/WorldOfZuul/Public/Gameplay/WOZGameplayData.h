// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WOZGameplayData.generated.h"

class AWOZGameRoom;
class AWOZGameItem;

UENUM()
namespace EWOZCommand
{
	enum Type
	{
		None,
		Go,
		Back,
		Take,
		Drop,
		Use,
		Open,
		Eat,
		Look,
		Item,
		Save,
	};
}

UENUM()
namespace EWOZGameItem
{
	enum Type
	{
		None,

		//Score,
		
		GoldenKey,
		SilverKey,
		CopperKey,
		
		GoldenTreasureBox,
		SilverTreasureBox,
		CopperTreasureBox,

		EmptyGoldenTreasureBox,
		EmptySilverTreasureBox,
		EmptyCopperTreasureBox,
		
		MagicCookie,

		TeleportDoor,
		
		MAX UMETA(Hidden),
	};
}

UENUM()
namespace EWOZGameItemType
{
	enum Type
	{
		None,
		Key,
		TreasureBox,
		EmptyTreasureBox,
		Food,
		RoomInteractive
	};
}

UENUM()
namespace EWOZGameRoomDirection
{
	enum Type
	{
		None,
		East,
		South,
		West,
		North
	};
}

USTRUCT(BlueprintType)
struct FWOZGameItemInfo
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EWOZGameItemType::Type> Type = EWOZGameItemType::None;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Quantifier;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> Strings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UTexture> Icon;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Weight = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EWOZCommand::Type> InMapDefaultCommand = EWOZCommand::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EWOZCommand::Type> InBagDefaultCommand = EWOZCommand::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Scale = 1.f;

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Priority = 0;

	bool operator<(const FWOZGameItemInfo& R) const
	{
		return Priority < R.Priority;
	}*/
};

USTRUCT(BlueprintType)
struct FWOZGameRoomInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UTexture> Floor;
};

USTRUCT(BlueprintType)
struct FWOZGameRoomData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FWOZGameRoomInfo Info;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIntPoint Position;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<TEnumAsByte<EWOZGameItem::Type>, FIntPoint> Items;
};

USTRUCT(BlueprintType)
struct FWOZCommandReplyMsg
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Command;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Reply;
};

USTRUCT(BlueprintType)
struct FWOZSaveGameData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform PlayerTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TEnumAsByte<EWOZGameItem::Type>> BagItems;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FWOZGameRoomData> RoomDatas;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FWOZCommandReplyMsg> CommandReplyMsgs;
};

/**
 * 
 */
UCLASS()
class WORLDOFZUUL_API UWOZGameplayData : public UDataAsset
{
	GENERATED_BODY()

public:
	FString GetItemNameString(EWOZGameItem::Type ItemEnum) const;
	FString GetItemDefaultCommandString(EWOZGameItem::Type ItemEnum) const;
	EWOZGameItemType::Type GetItemType(EWOZGameItem::Type ItemEnum) const;
	EWOZGameItem::Type GetItemByString(const FString& Str);
	TMap<EWOZGameItem::Type, int32> GetItemCounterMap(const TArray<TEnumAsByte<EWOZGameItem::Type>>& ItemEnums);
	FString ItemCounterToString(const TMap<EWOZGameItem::Type, int32>& Counter) const;
	FString GetStringCommand(EWOZCommand::Type Command);
	FString GetStringDirection(EWOZGameRoomDirection::Type Direction);

	TArray<TEnumAsByte<EWOZGameItem::Type>> GetTreasureFromBox(EWOZGameItem::Type TreasureBox) const;
	int32 GetScoreFromBox(EWOZGameItem::Type TreasureBox) const;

	bool IsTakeableItem(EWOZGameItem::Type Item) const;
	bool IsOpenableItem(EWOZGameItem::Type Item) const;
	bool IsUseableItem(EWOZGameItem::Type Item) const;
	bool IsEatableItem(EWOZGameItem::Type Item) const;

	
public:
	UPROPERTY(EditAnywhere, Category = "WOZ|Command")
	TMap<FString, TEnumAsByte<EWOZCommand::Type>> StringCommands;
	
	UPROPERTY(EditAnywhere, Category = "WOZ|Command")
	TMap<TEnumAsByte<EWOZCommand::Type>, FText> CommandNames;

	UPROPERTY(EditAnywhere, Category = "WOZ|Player")
	float PlayerSize = 100.f;
	
	UPROPERTY(EditAnywhere, Category = "WOZ|Item")
	float ItemSize = 64.f;
	
	UPROPERTY(EditAnywhere, Category = "WOZ|Item")
	TSubclassOf<AWOZGameItem> ItemClass;
	
	UPROPERTY(EditAnywhere, Category = "WOZ|Item")
	TMap<TEnumAsByte<EWOZGameItem::Type>, FWOZGameItemInfo> Items;
	
	UPROPERTY(EditAnywhere, Category = "WOZ|Room")
	TSubclassOf<AWOZGameRoom> RoomClass;
	
	UPROPERTY(EditAnywhere, Category = "WOZ|Room")
	float RoomSize = 1000.f;

	UPROPERTY(EditAnywhere, Category = "WOZ|Room")
	TArray<FWOZGameRoomInfo> RoomInfos;
	
	UPROPERTY(EditAnywhere, Category = "WOZ|Room")
	TMap<FString, TEnumAsByte<EWOZGameRoomDirection::Type>> StringDirections;

	UPROPERTY(EditAnywhere, Category = "WOZ|Room")
	FIntPoint RoomItemSpawnCountRange = FIntPoint(2, 6);

	UPROPERTY(EditAnywhere, Category = "WOZ|Room")
	TMap<TEnumAsByte<EWOZGameItem::Type>, float> RoomItemSpawnRate;

	UPROPERTY(EditAnywhere, Category = "WOZ|TreasureBox")
	FIntPoint GoldenTreasureBoxScoreRange= FIntPoint(40, 60);
	
	UPROPERTY(EditAnywhere, Category = "WOZ|TreasureBox")
	FIntPoint GoldenTreasureRange = FIntPoint(4, 6);
	
	UPROPERTY(EditAnywhere, Category = "WOZ|TreasureBox")
	TMap<TEnumAsByte<EWOZGameItem::Type>, float> GoldenTreasureRate;

	UPROPERTY(EditAnywhere, Category = "WOZ|TreasureBox")
	FIntPoint SilverTreasureBoxScoreRange= FIntPoint(25, 40);
	
	UPROPERTY(EditAnywhere, Category = "WOZ|TreasureBox")
	FIntPoint SilverTreasureRange = FIntPoint(2, 4);
	
	UPROPERTY(EditAnywhere, Category = "WOZ|TreasureBox")
	TMap<TEnumAsByte<EWOZGameItem::Type>, float> SilverTreasureRate;

	UPROPERTY(EditAnywhere, Category = "WOZ|TreasureBox")
	FIntPoint CopperTreasureBoxScoreRange= FIntPoint(10, 25);
	
	UPROPERTY(EditAnywhere, Category = "WOZ|TreasureBox")
	FIntPoint CopperTreasureRange = FIntPoint(0, 2);
	
	UPROPERTY(EditAnywhere, Category = "WOZ|TreasureBox")
	TMap<TEnumAsByte<EWOZGameItem::Type>, float> CopperTreasureRate;

	UPROPERTY(EditAnywhere, Category = "WOZ|MagicCookie")
	int32 MagicCookieMaxWeightAddCount = 10;
};
