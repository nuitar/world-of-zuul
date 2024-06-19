// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Gameplay/WOZGameplayData.h"
#include "BagItemWidget.generated.h"

struct FWOZGameItemInfo;
class UOverlayWidget;
class UButton;
class UTextBlock;

/**
 * 
 */
UCLASS()
class WORLDOFZUUL_API UBagItemWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	static UBagItemWidget* CreateBagItemWidget(UOverlayWidget* Overlay, TSubclassOf<UBagItemWidget> Class, EWOZGameItem::Type Item, UWOZGameplayData* GameplayData);

	virtual void NativeConstruct() override;
	
private:
	UFUNCTION()
	void OnButtonFuncReleased();
	
	UFUNCTION()
	void OnButtonDropReleased();
	
protected:
	UPROPERTY()
	TObjectPtr<UWOZGameplayData> GameplayData;
	
	UPROPERTY()
	TObjectPtr<UOverlayWidget> OverlayWidget;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_Item;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Func;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_Func;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Drop;

	FWOZGameItemInfo ItemInfo;
};
