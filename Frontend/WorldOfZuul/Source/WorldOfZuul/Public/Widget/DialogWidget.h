// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DialogWidget.generated.h"

class UButton;
class UTextBlock;

DECLARE_DELEGATE_OneParam(FDialogWidgetSignature, bool);

/**
 * 
 */
UCLASS()
class WORLDOFZUUL_API UDialogWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual bool Initialize() override;
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable, Category = "Dialog")
	static UDialogWidget* DisplayDialog(TSubclassOf<UDialogWidget> Class, UObject* Owner, const FText& Msg, bool bShowButtonNo);

private:
	UFUNCTION()
	void ReplyYes();

	UFUNCTION()
	void ReplyNo();

	void PostReply();

public:
	FDialogWidgetSignature OnReply;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_Message;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Yes;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_No;

	UPROPERTY()
	TObjectPtr<UUserWidget> ParentWidget;
};
