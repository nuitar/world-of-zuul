// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WOZGameItem.h"
#include "GameFramework/Actor.h"
#include "WOZGameRoom.generated.h"


class UWOZGameplayData;
class AWOZGameItem;
class UBoxComponent;
struct FWOZGameRoomData;

UCLASS()
class WORLDOFZUUL_API AWOZGameRoom : public AActor
{
	GENERATED_BODY()

public:
	AWOZGameRoom();

	static AWOZGameRoom* CreateRoom(UObject* WorldContext, const FIntPoint& Pos, UWOZGameplayData* GameplayData);
	static AWOZGameRoom* CreateRoom(UObject* WorldContext, FWOZGameRoomData* InRoomData, UWOZGameplayData* GameplayData);

	void AddNewItem(TEnumAsByte<EWOZGameItem::Type> ItemEnum);
	void AddNewItems(const TArray<TEnumAsByte<EWOZGameItem::Type>>& ItemEnums);
	void RemoveItems(const TArray<AWOZGameItem*>& ItemsToRemove);
	void ReplaceItem(AWOZGameItem* OldItem, EWOZGameItem::Type NewItemEnum);

	const TArray<AWOZGameItem*> GetAllItems() const { return Items; }
	
	FWOZGameRoomData GetRoomData();

protected:

private:
	UPROPERTY()
	TObjectPtr<UWOZGameplayData> GameplayData;
	
	FIntPoint Position;
	
	UPROPERTY()
	TArray<AWOZGameItem*> Items;
};
