// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/IHttpRequest.h"
#include "RegisterWidget.generated.h"

class ULoginWidget;
class UEditableTextBox;
class UButton;
class UDialogWidget;

/**
 * 
 */
UCLASS()
class WORLDOFZUUL_API URegisterWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual bool Initialize() override;
	virtual void NativeConstruct() override;

private:
	UFUNCTION()
	void Register();

	UFUNCTION()
	void Login();

	void OnRegisterResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bRequestSuccessful);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> EditableTextBox_Username;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> EditableTextBox_Password;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> EditableTextBox_PasswordConfirm;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Register;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Login;

protected:
	UPROPERTY(EditAnywhere, Category = "LoginAndRegister")
	FString LoginURL;

	UPROPERTY(EditAnywhere, Category = "LoginAndRegister")
	FText EmptyInputMsg;
	
	UPROPERTY(EditAnywhere, Category = "LoginAndRegister")
	FText PasswordConfirmErrorMsg;
	
	UPROPERTY(EditAnywhere, Category = "LoginAndRegister")
	FText RequestFailedMsg;

	UPROPERTY(EditAnywhere, Category = "LoginAndRegister")
	FText RegisterSuccessfulMsg;
	
	UPROPERTY(EditAnywhere, Category = "LoginAndRegister")
	FText RegisterFailedMsg;
	
	UPROPERTY(EditAnywhere, Category = "LoginAndRegister")
	TSubclassOf<UDialogWidget> DialogWidgetClass;

	UPROPERTY(EditAnywhere, Category = "LoginAndRegister")
	TSubclassOf<ULoginWidget> LoginWidgetClass;
};
