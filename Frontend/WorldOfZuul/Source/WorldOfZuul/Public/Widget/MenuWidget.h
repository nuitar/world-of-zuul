// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/IHttpRequest.h"
#include "MenuWidget.generated.h"

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
	
	void SaveClear();	
	void Load();	

	void OnSinglePlayerDialogReply(bool bReply);
	void OnLoadResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bRequestSuccessful);
	
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_SinglePlayer;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Logout;
	
private:
	UPROPERTY(EditAnywhere, Category = "WOZ")
	TSubclassOf<UDialogWidget> DialogWidgetClass;

	UPROPERTY(EditAnywhere, Category = "WOZ")
	FName GameMapName;
	
	UPROPERTY(EditAnywhere, Category = "WOZ|HTTP")
	FString SaveURL;
	
	UPROPERTY(EditAnywhere, Category = "WOZ|HTTP")
	FString LoadURL;

	FString PrevSaveGameDataStr;
};
