// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameHistoryItemWidget.generated.h"

class UBorder;
class UButton;
struct FWOZGameHistoryData;
class UTextBlock;

/**
 * 
 */
UCLASS()
class WORLDOFZUUL_API UGameHistoryItemWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	static UGameHistoryItemWidget* CreateGameHistoryItemWidget(TSubclassOf<UGameHistoryItemWidget> Class, UUserWidget* Parent, const FWOZGameHistoryData& GameHistoryData);

public:

private:
	UFUNCTION()
	void Detail();
	
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Detail;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> Border_Detail;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_Date;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_Score;
};
