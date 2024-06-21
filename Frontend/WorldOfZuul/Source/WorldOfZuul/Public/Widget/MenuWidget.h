// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
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

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Logout;
	
private:
	UPROPERTY(EditAnywhere, Category = "LoginAndRegister")
	TSubclassOf<UDialogWidget> DialogWidgetClass;

};
