// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputMappingContext.h"
#include "WOZGameplayData.h"
#include "Interfaces/IHttpRequest.h"
#include "GameFramework/PlayerController.h"
#include "WOZPlayerController.generated.h"

class ASceneCapture2D;
class UWOZGameplayData;
class AWOZPlayerState;
struct FInputActionValue;
class UOverlayWidget;

/**
 * 
 */

UCLASS()
class WORLDOFZUUL_API AWOZPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	UFUNCTION(Server, Reliable)
	void ExecuteCommand(const FString& CommandStr);

	UFUNCTION(Client, Reliable)
	void ReplyCommand(const FWOZCommandReplyMsg& Msg);

	UFUNCTION(BlueprintCallable, Category = "WOZ")
	void ExecuteCommand_ItemTarget(TEnumAsByte<EWOZCommand::Type> Command, TEnumAsByte<EWOZGameItem::Type> Target);

	UFUNCTION(BlueprintCallable, Category = "WOZ")
	void ExecuteCommand_DirectionTarget(TEnumAsByte<EWOZCommand::Type> Command, TEnumAsByte<EWOZGameRoomDirection::Type> Target);

	void OnGameRemainTimeTick(float RemainTime);
	void OnGameEnded();
	void SaveClear();

	UFUNCTION(Client, Reliable)
	void OnGameEnded_Client();
	
protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnRep_PlayerState() override;
	virtual void OnRep_Pawn() override;
	void Move(const FInputActionValue& Value);

	UFUNCTION(Server, Reliable)
	void Interact();

private:
	void SetupInput();

	UFUNCTION()
	void OnPawnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION()
	void OnPawnEndOverlap(AActor* OverlappedActor, AActor* OtherActor);

	FText CommandGo(const FString& Target);
	FText CommandTake(const FString& Target);
	FText CommandOpen(const FString& Target);
	FText CommandBack();
	FText CommandUse(const FString& Target);
	FText CommandDrop(const FString& Target);
	FText CommandEat(const FString& Target);
	FText CommandLook();
	FText CommandItem();
	void CommandSaveGame();
	FText CommandHelp();

	UFUNCTION(Client, Reliable)
	void CommandQuit();

	UFUNCTION(Client, Reliable)
	void CommandSaveSetting();
	
	AWOZGameItem* GetNearestItem(EWOZGameItem::Type ItemEnum, AWOZGameRoom* Room) const;
	UFUNCTION(NetMulticast, Reliable)
	void GotoRoom(AWOZGameRoom* Room);
	FText TeleportRandom();

	void OnSaveResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bRequestSuccessful);

protected:
	UPROPERTY()
	TObjectPtr<ASceneCapture2D> SceneCapture2D;

	UPROPERTY()
	TObjectPtr<AWOZPlayerState> WOZPlayerState;
	
	UPROPERTY()
	TObjectPtr<UOverlayWidget> OverlayWidget;

private:
	UPROPERTY(EditAnywhere, Category = "WOZ|Input")
	TObjectPtr<UInputMappingContext> Context;

	UPROPERTY(EditAnywhere, Category = "WOZ|Input")
	TObjectPtr<UInputAction> MoveAction;
	
	UPROPERTY(EditAnywhere, Category = "WOZ|Input")
	TObjectPtr<UInputAction> InteractAction;
	
	UPROPERTY(EditAnywhere, Category = "WOZ|View")
	TSubclassOf<UOverlayWidget> OverlayWidgetClass;

	UPROPERTY(EditAnywhere, Category = "WOZ")
	TObjectPtr<UWOZGameplayData> GameplayData;
	
	UPROPERTY(EditAnywhere, Category = "WOZ")
	FName MenuMapName;
	
	UPROPERTY(EditAnywhere, Category = "WOZ|HTTP")
	FString SaveGameURL;

	UPROPERTY(EditAnywhere, Category = "WOZ|HTTP")
	FString SaveHistoryURL;
	

	
	UPROPERTY()
	TArray<FWOZCommandReplyMsg> CommandReplyMsgs;
	
	UPROPERTY()
	TObjectPtr<AWOZGameItem> CurrentOverlappingGameItem;
};
