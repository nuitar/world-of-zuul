// Fill out your copyright notice in the Description page of Project Settings.

#include "WorldOfZuul/Public/Widget/OverlayWidget.h"

#include "HttpModule.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/Image.h"
#include "Components/RichTextBlock.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Gameplay/WOZGameInstance.h"
#include "Gameplay/WOZPlayerController.h"
#include "Gameplay/WOZPlayerState.h"
#include "Widget/BagItemWidget.h"

void UOverlayWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UWOZGameInstance* GameInstance = Cast<UWOZGameInstance>(GetGameInstance());
	check(GameInstance);

	TextBlock_GameData_Username->SetText(GameInstance->Username);

	PlayerController = Cast<AWOZPlayerController>(GetOwningPlayer());
	PlayerState = PlayerController->GetPlayerState<AWOZPlayerState>();

	PlayerState->OnScoreUpdated.AddUObject(this, &UOverlayWidget::OnScoreUpdated);
	PlayerState->OnRoomPositionHistoryUpdated.AddUObject(this, &UOverlayWidget::OnRoomPositionHistoryUpdated);
	PlayerState->OnBagItemUpdated.AddUObject(this, &UOverlayWidget::OnBagItemUpdated);
	PlayerState->OnMaxWeightUpdated.AddUObject(this, &UOverlayWidget::OnMaxWeightUpdated);

	EditableTextBox_Command->OnTextCommitted.AddDynamic(this, &UOverlayWidget::OnCommandCommited);
	Button_Execute->OnReleased.AddDynamic(this, &UOverlayWidget::ExecuteCommand);
	Button_Bag_DropAll->OnReleased.AddDynamic(this, &UOverlayWidget::DropAll);

	OnScoreUpdated(PlayerState);
	OnRoomPositionHistoryUpdated(PlayerState);
	OnBagItemUpdated(PlayerState);
	OnMaxWeightUpdated(PlayerState);
}

void UOverlayWidget::SetRenderTarget(UTextureRenderTarget2D* InRenderTarget2D)
{
	RenderTarget2D = InRenderTarget2D;
	FSlateBrush Brush = Image_Viewport->GetBrush();
	
	MaterialInstanceDynamic = UMaterialInstanceDynamic::Create(Cast<UMaterialInterface>(Brush.GetResourceObject()), this);
	MaterialInstanceDynamic->SetTextureParameterValue(FName("Texture"), RenderTarget2D);
	Brush.SetResourceObject(MaterialInstanceDynamic);

	Image_Viewport->SetBrush(Brush);
}

void UOverlayWidget::AddCommandReplyMsg(const FWOZCommandReplyMsg& Msg)
{
	FString Str = RichTextBlock_Process->GetText().ToString();
	if (!Str.IsEmpty())
	{
		Str += "\n> ";
	}
	Str += Msg.Command.ToString() + "\n" + Msg.Reply.ToString();
	RichTextBlock_Process->SetText(FText::FromString(Str));

	if (UScrollBox* ScrollBox = Cast<UScrollBox>(RichTextBlock_Process->GetParent()))
	{
		ScrollBox->ScrollToStart();
		ScrollBox->ScrollToEnd();
	}
}

void UOverlayWidget::RefreshBagItems()
{
	check(BagItemWidgetClass && GameplayData && PlayerState);

	ScrollBox_Bag->ClearChildren();
	const TArray<TEnumAsByte<EWOZGameItem::Type>>& Items = PlayerState->GetBagItems();
	for (EWOZGameItem::Type Item : Items)
	{
		UBagItemWidget* BagItemWidget = UBagItemWidget::CreateBagItemWidget(this, BagItemWidgetClass, Item, GameplayData);
		ScrollBox_Bag->AddChild(BagItemWidget);
	}
}

void UOverlayWidget::OnCommandCommited(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		ExecuteCommand();
	}
}

void UOverlayWidget::ExecuteCommand()
{
	check(PlayerController);

	PlayerController->ExecuteCommand(EditableTextBox_Command->GetText().ToString());
	EditableTextBox_Command->SetText(FText());
}

void UOverlayWidget::ExecuteCommand(const FString& Command)
{
	check(PlayerController);

	PlayerController->ExecuteCommand(Command);
}

void UOverlayWidget::ExecuteCommand_SaveTarget(const FString& Target)
{
	check(PlayerController && GameplayData);
	PlayerController->ExecuteCommand(GameplayData->GetStringCommand(EWOZCommand::Save) + " " + Target);
}

void UOverlayWidget::ExecuteCommand_ItemTarget(TEnumAsByte<EWOZCommand::Type> Command, TEnumAsByte<EWOZGameItem::Type> Target)
{
	check(PlayerController);
	PlayerController->ExecuteCommand_ItemTarget(Command, Target);
}

void UOverlayWidget::ExecuteCommand_DirectionTarget(TEnumAsByte<EWOZCommand::Type> Command, TEnumAsByte<EWOZGameRoomDirection::Type> Target)
{
	check(PlayerController);
	PlayerController->ExecuteCommand_DirectionTarget(Command, Target);
}

void UOverlayWidget::SetGameRemainTime(float RemainTime)
{
	TextBlock_GameData_RemainTime->SetText(FText::FromString(FString::FromInt((int32)(RemainTime + 0.5f))));
	
	if (RemainTime <= 0.01f)
	{
		SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}

void UOverlayWidget::OnScoreUpdated(AWOZPlayerState* PS)
{
	check(PlayerState);
	const FString& Str = FString::FromInt(PlayerState->GetGameScore());
	TextBlock_GameData_Score->SetText(FText::FromString(Str));
}

void UOverlayWidget::OnRoomPositionHistoryUpdated(AWOZPlayerState* PS)
{
	check(PlayerState);

	const FString& Str = "(" + FString::FromInt(PlayerState->GetCurrentRoomPosition().X) + " , " + FString::FromInt(PlayerState->GetCurrentRoomPosition().Y) + ")";
	TextBlock_GameData_RoomPosition->SetText(FText::FromString(Str));
}

void UOverlayWidget::OnBagItemUpdated(AWOZPlayerState* PS)
{
	check(PlayerState && GameplayData);
	
	RefreshBagItems();
	TextBlock_Bag_CurrentWeight->SetText(FText::FromString(FString::FromInt(PlayerState->GetCurrentWeight(GameplayData))));
}

void UOverlayWidget::OnMaxWeightUpdated(AWOZPlayerState* PS)
{
	check(PlayerState);
	TextBlock_Bag_MaxWeight->SetText(FText::FromString(FString::FromInt(PlayerState->GetMaxWeight())));
}

void UOverlayWidget::DropAll()
{
	check(PlayerController && GameplayData);
	
	FString Command;
	
	for (const auto& StringCommand : GameplayData->StringCommands)
	{
		if (StringCommand.Value == EWOZCommand::Drop)
		{
			Command = StringCommand.Key;
			break;
		}
	}

	Command += " all";
	PlayerController->ExecuteCommand(Command);
}
