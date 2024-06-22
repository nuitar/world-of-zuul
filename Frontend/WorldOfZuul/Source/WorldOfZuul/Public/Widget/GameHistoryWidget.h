// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameHistoryWidget.generated.h"

struct FWOZGameHistoryData;
class UGameHistoryItemWidget;
class UScrollBox;
class UButton;

/**
 * 
 */
UCLASS()
class WORLDOFZUUL_API UGameHistoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	static UGameHistoryWidget* DisplayGameHistoryWidget(TSubclassOf<UGameHistoryWidget> Class, UObject* Owner, const TArray<FWOZGameHistoryData>& GameHistoryDatas);

private:
	UFUNCTION()
	void Close();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScrollBox> ScrollBox_History;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Close;
	
	UPROPERTY(EditAnywhere, Category = "WOZ")
	TSubclassOf<UGameHistoryItemWidget> GameHistoryItemWidgetClass;

	UPROPERTY()
	TObjectPtr<UUserWidget> ParentWidget;
};
