// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Gameplay/WOZGameplayData.h"
#include "OverlayWidget.generated.h"

struct FWOZCommandReplyMsg;
class UTextBlock;
class UBagItemWidget;
class UScrollBox;
class UWOZGameplayData;
class AWOZPlayerController;
class UImage;
class UButton;
class UEditableTextBox;
class AWOZPlayerState;
class URichTextBlock;

/**
 * 
 */
UCLASS()
class WORLDOFZUUL_API UOverlayWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	void SetRenderTarget(UTextureRenderTarget2D* InRenderTarget2D);

	void AddCommandReplyMsg(const FWOZCommandReplyMsg& Msg);
	void RefreshBagItems();
	
	void ExecuteCommand(const FString& Command);

	UFUNCTION(BlueprintCallable, Category = "WOZ")
	void ExecuteCommand_SaveTarget(const FString& Target);

	UFUNCTION(BlueprintCallable, Category = "WOZ")
	void ExecuteCommand_ItemTarget(TEnumAsByte<EWOZCommand::Type> Command, TEnumAsByte<EWOZGameItem::Type> Target);

	UFUNCTION(BlueprintCallable, Category = "WOZ")
	void ExecuteCommand_DirectionTarget(TEnumAsByte<EWOZCommand::Type> Command, TEnumAsByte<EWOZGameRoomDirection::Type> Target);

	void SetGameRemainTime(float RemainTime);
	
private:
	UFUNCTION()
	void OnCommandCommited(const FText& Text, ETextCommit::Type CommitMethod);

	UFUNCTION()
	void ExecuteCommand();
	
	void OnScoreUpdated(AWOZPlayerState* PS);
	void OnBagItemUpdated(AWOZPlayerState* PS);
	void OnRoomPositionHistoryUpdated(AWOZPlayerState* PS);
	void OnMaxWeightUpdated(AWOZPlayerState* PS);

	UFUNCTION()
	void DropAll();

protected:
	UPROPERTY()
	TObjectPtr<AWOZPlayerController> PlayerController;

	UPROPERTY()
	TObjectPtr<AWOZPlayerState> PlayerState;
	
private:
	UPROPERTY(EditAnywhere, Category = "WOZ")
	TObjectPtr<UWOZGameplayData> GameplayData;

	UPROPERTY(EditAnywhere, Category = "WOZ")
	TSubclassOf<UBagItemWidget> BagItemWidgetClass;

	
	UPROPERTY()
	TObjectPtr<UTextureRenderTarget2D> RenderTarget2D;

	UPROPERTY()
	UMaterialInstanceDynamic* MaterialInstanceDynamic;
	
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_Viewport;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<URichTextBlock> RichTextBlock_Process;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> EditableTextBox_Command;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Execute;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScrollBox> ScrollBox_Bag;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_GameData_Username;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_GameData_Score;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_GameData_RoomPosition;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_GameData_RemainTime;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_Bag_CurrentWeight;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_Bag_MaxWeight;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Bag_DropAll;
};
