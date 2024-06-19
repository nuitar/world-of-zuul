// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputMappingContext.h"
#include "WOZGameplayData.h"
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
	
	
protected:
	virtual void BeginPlay() override;
	virtual void AcknowledgePossession(APawn* P) override;
	void Move(const FInputActionValue& Value);
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

	AWOZGameItem* GetNearestItem(EWOZGameItem::Type ItemEnum, AWOZGameRoom* Room) const;
	void GotoRoom(AWOZGameRoom* Room);

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

	UPROPERTY()
	TObjectPtr<AWOZGameItem> CurrentOverlappingGameItem;
};
