// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "WOZGameMode.generated.h"

class UWOZGameplayData;
class AWOZGameRoom;
struct FWOZGameRoomData;

/**
 * 
 */
UCLASS()
class WORLDOFZUUL_API AWOZGameMode : public AGameModeBase
{
	GENERATED_BODY()


public:
	AWOZGameMode();
	
	virtual void Tick(float DeltaSeconds) override;
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	float GetRemainTime() const { return GameRemainTime; }
	
	UFUNCTION(BlueprintCallable)
	void GenerateMap(const FIntPoint& HalfSize);
	void GenerateMap(const TArray<FWOZGameRoomData>& RoomData);

	AWOZGameRoom* GetRoomByPosition(const FIntPoint& Position);
	const TArray<AWOZGameRoom*>& GetAllRooms() const { return Rooms; }
	
protected:

private:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UWOZGameplayData> GameplayData;
	
	UPROPERTY()
	TArray<AWOZGameRoom*> Rooms;

	UPROPERTY()
	float GameRemainTime = 0.f;
};
