// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WOZGameItem.h"
#include "GameFramework/Actor.h"
#include "WOZGameRoom.generated.h"


class AWOZPlayerController;
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
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	static AWOZGameRoom* CreateRoom(UObject* WorldContext, const FWOZGameRoomInfo& Info, const FIntPoint& Pos, UWOZGameplayData* GameplayData);
	static AWOZGameRoom* CreateRoom(UObject* WorldContext, const FWOZGameRoomData& InRoomData, UWOZGameplayData* GameplayData);

	void SetRoomInfo(const FWOZGameRoomInfo& Info);
	void InitDoors(const TArray<FIntPoint>& MapRoomPos);
	
	void AddNewItem(TEnumAsByte<EWOZGameItem::Type> ItemEnum);
	void AddNewItems(const TArray<TEnumAsByte<EWOZGameItem::Type>>& InItemEnums);
	void RemoveItem(AWOZGameItem* ItemToRemove);
	void RemoveItems(const TArray<AWOZGameItem*>& ItemsToRemove);
	void ReplaceItem(AWOZGameItem* OldItem, EWOZGameItem::Type NewItemEnum);

	const TArray<AWOZGameItem*>& GetAllItems() const { return Items; }
	TArray<EWOZGameItem::Type> GetAllItemEnums() const;
	
	const FIntPoint& GetPosition() const { return Position; }
	const FWOZGameRoomInfo& GetRoomInfo() const { return RoomInfo; }
	FWOZGameRoomData GetRoomData();

protected:

private:
	UFUNCTION()
	void OnDoorEastOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnDoorSouthOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnDoorWestOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnDoorNorthOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void DoorOverlap(AActor* Actor, EWOZGameRoomDirection::Type Direction);

	UFUNCTION()
	void OnRep_RoomInfo();
	
private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> Floor;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> Door_East;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> Door_South;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> Door_West;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> Door_North;
	
	UPROPERTY()
	TObjectPtr<UWOZGameplayData> GameplayData;
	
	FIntPoint Position;
	
	UPROPERTY()
	TArray<AWOZGameItem*> Items;
	
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> MaterialInstanceDynamic;

	UPROPERTY(ReplicatedUsing = "OnRep_RoomInfo")
	FWOZGameRoomInfo RoomInfo;
};
