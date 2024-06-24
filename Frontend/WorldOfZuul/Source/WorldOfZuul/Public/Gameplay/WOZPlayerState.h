// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WOZGameplayData.h"
#include "GameFramework/PlayerState.h"
#include "WOZPlayerState.generated.h"

class AWOZPlayerState;

DECLARE_MULTICAST_DELEGATE_OneParam(FWOZPlayerStateSignature, AWOZPlayerState*);

/**
 * 
 */
UCLASS()
class WORLDOFZUUL_API AWOZPlayerState : public APlayerState
{
	GENERATED_BODY()
	friend class AWOZPlayerController;

public:
	AWOZPlayerState();
	static AWOZPlayerState* Get(const UObject* WorldContext);
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void OnRep_Owner() override;
	
public:
	const FText& GetUsername() const { return Username; }
	
	void AddScore(int32 Add);
	void AddBagItem(EWOZGameItem::Type ItemToAdd);
	void AddBagItems(TArray<TEnumAsByte<EWOZGameItem::Type>> ItemsToAdd);
	void RemoveBagItem(EWOZGameItem::Type ItemToRemove);
	void RemoveBagItems(TArray<TEnumAsByte<EWOZGameItem::Type>> ItemsToRemove);
	
	void PushRoomPosition(const FIntPoint& Pos);
	void PopRoomPosition();
	const TArray<FIntPoint>& GetRoomPositionHistory() const { return RoomPositionHistory; }

	int32 GetGameScore() const { return GameScore; }
	const TArray<TEnumAsByte<EWOZGameItem::Type>>& GetBagItems() const { return BagItems; }
	int32 GetCurrentWeight(UWOZGameplayData* GameplayData);
	int32 GetMaxWeight() const { return MaxWeight; }
	void SetMaxWeight(int32 Weight);
	void AddMaxWeight(int32 Add);
	
	const FIntPoint& GetCurrentRoomPosition() const { return RoomPositionHistory.Last(); }

private:
	UFUNCTION(Server, Reliable)
	void SetUserName(const FText& InUsername);
	
	UFUNCTION()
	void OnRep_BagItems();
	
	UFUNCTION()
	void OnRep_MaxWeight();

	UFUNCTION()
	void OnRep_RoomPositionHistory();

	UFUNCTION()
	void OnRep_GameScore();
	
public:
	FWOZPlayerStateSignature OnScoreUpdated;
	FWOZPlayerStateSignature OnBagItemUpdated;
	FWOZPlayerStateSignature OnMaxWeightUpdated;
	FWOZPlayerStateSignature OnRoomPositionHistoryUpdated;
	
protected:
	UPROPERTY(Replicated)
	FText Username;
	
	UPROPERTY(ReplicatedUsing = "OnRep_GameScore")
	int32 GameScore = 0;

	UPROPERTY(ReplicatedUsing = "OnRep_RoomPositionHistory")
	TArray<FIntPoint> RoomPositionHistory = { FIntPoint(0, 0) };

	UPROPERTY(ReplicatedUsing = "OnRep_BagItems")
	TArray<TEnumAsByte<EWOZGameItem::Type>> BagItems;
	
	UPROPERTY(ReplicatedUsing = "OnRep_MaxWeight")
	int32 MaxWeight = 30;	

	TArray<FWOZCommandReplyMsg> CommandProcessMsgs;
};
