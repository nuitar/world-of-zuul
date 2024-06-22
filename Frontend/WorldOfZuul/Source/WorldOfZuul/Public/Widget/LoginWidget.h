// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/IHttpRequest.h"
#include "LoginWidget.generated.h"

class URegisterWidget;
class UDialogWidget;
class UEditableTextBox;
class UButton;

/**
 * 
 */
UCLASS()
class WORLDOFZUUL_API ULoginWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual bool Initialize() override;
	virtual void NativeConstruct() override;

private:
	UFUNCTION()
	void Login();

	UFUNCTION()
	void Register();

	void OnLoginResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bRequestSuccessful);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> EditableTextBox_Username;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> EditableTextBox_Password;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Login;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Register;

protected:
	UPROPERTY(EditAnywhere, Category = "LoginAndRegister")
	FString LoginURL;
	
	UPROPERTY(EditAnywhere, Category = "LoginAndRegister")
	FText EmptyInputMsg;
	
	UPROPERTY(EditAnywhere, Category = "LoginAndRegister")
	FText RequestFailedMsg;

	UPROPERTY(EditAnywhere, Category = "LoginAndRegister")
	FText LoginFailedMsg;
	
	UPROPERTY(EditAnywhere, Category = "LoginAndRegister")
	TSubclassOf<UDialogWidget> DialogWidgetClass;

	UPROPERTY(EditAnywhere, Category = "LoginAndRegister")
	TSubclassOf<URegisterWidget> RegisterWidgetClass;

	UPROPERTY(EditAnywhere, Category = "LoginAndRegister")
	FName MenuMapName;
};
