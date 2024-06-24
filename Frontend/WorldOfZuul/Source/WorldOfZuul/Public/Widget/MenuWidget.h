// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Gameplay/WOZGameplayData.h"
#include "Interfaces/IHttpRequest.h"
#include "MenuWidget.generated.h"

class UGameHistoryWidget;
class UDialogWidget;
class UButton;

/**
 * 
 */
UCLASS()
class WORLDOFZUUL_API UMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

private:
	UFUNCTION()
	void SinglePlayer();

	UFUNCTION()
	void GameHistory();

	UFUNCTION()
	void Logout();
	
	void SaveClear();	
	void LoadGame();	
	void LoadHistory();	

	void OnLoadGameResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bRequestSuccessful);
	void OnLoadHistoryResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bRequestSuccessful);

	void OnSinglePlayerDialogReply(bool bReply);

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_SinglePlayer;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_GameHistory;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Logout;
	
private:
	UPROPERTY(EditAnywhere, Category = "WOZ")
	TSubclassOf<UDialogWidget> DialogWidgetClass;

	UPROPERTY(EditAnywhere, Category = "WOZ")
	TSubclassOf<UGameHistoryWidget> GameHistoryWidgetClass;

	UPROPERTY(EditAnywhere, Category = "WOZ")
	FName LoginRegisterMapName;
	
	UPROPERTY(EditAnywhere, Category = "WOZ")
	FName GameMapName;
	
	UPROPERTY(EditAnywhere, Category = "WOZ|HTTP")
	FString SaveClearURL;
	
	UPROPERTY(EditAnywhere, Category = "WOZ|HTTP")
	FString LoadGameURL;

	UPROPERTY(EditAnywhere, Category = "WOZ|HTTP")
	FString LoadHistoryURL;
	
	FString PrevSaveGameDataStr;

	TArray<FWOZGameHistoryData> GameHistoryDatas;
};
