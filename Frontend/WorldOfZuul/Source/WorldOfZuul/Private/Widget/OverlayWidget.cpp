// Fill out your copyright notice in the Description page of Project Settings.

#include "WorldOfZuul/Public/Widget/OverlayWidget.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/RichTextBlock.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Gameplay/WOZPlayerController.h"
#include "Gameplay/WOZPlayerState.h"
#include "Widget/BagItemWidget.h"

void UOverlayWidget::NativeConstruct()
{
	Super::NativeConstruct();

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

void UOverlayWidget::AddCommandReplyMsg(const FWOZCommandReplyMsg& Msg)
{
	FString Str = RichTextBlock_Process->GetText().ToString();
	Str += "\n> " + Msg.Command.ToString() + "\n" + Msg.Reply.ToString();
	RichTextBlock_Process->SetText(FText::FromString(Str));

	if (UScrollBox* ScrollBox = Cast<UScrollBox>(RichTextBlock_Process->GetParent()))
	{
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

void UOverlayWidget::OnScoreUpdated(AWOZPlayerState* PS)
{
	check(PlayerState);
	const FString& Str = FString::FromInt(PlayerState->GetScore());
	TextBlock_GameData_Score->SetText(FText::FromString(Str));
}

void UOverlayWidget::OnRoomPositionHistoryUpdated(AWOZPlayerState* PS)
{
	check(PlayerState);
	const FString& Str = FString::FromInt(PlayerState->GetCurrentRoomPosition().X) + " , " + FString::FromInt(PlayerState->GetCurrentRoomPosition().Y);
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

