// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/DialogWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

bool UDialogWidget::Initialize()
{
	if (!Super::Initialize()) return false;

	SetIsFocusable(true);
	
	return true;
}

void UDialogWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Button_Yes->OnReleased.AddDynamic(this, &UDialogWidget::ReplyYes);
	Button_No->OnReleased.AddDynamic(this, &UDialogWidget::ReplyNo);
}

UDialogWidget* UDialogWidget::DisplayDialog(TSubclassOf<UDialogWidget> Class, UObject* Owner, const FText& Msg, bool bShowButtonNo)
{
	if (!Class) return nullptr;
	
	UDialogWidget* DialogWidget = CreateWidget<UDialogWidget>(Cast<UUserWidget>(Owner), Class);
	if (!DialogWidget) return nullptr;
	
	if (!bShowButtonNo)
	{
		DialogWidget->Button_No->SetVisibility(ESlateVisibility::Collapsed);
	}

	DialogWidget->ParentWidget = Cast<UUserWidget>(Owner);
	DialogWidget->TextBlock_Message->SetText(Msg);
	DialogWidget->AddToViewport(64);
	DialogWidget->SetFocus();
	
	return DialogWidget;
}

void UDialogWidget::ReplyYes()
{
	if (OnReply.IsBound())
	{
		OnReply.Execute(true);
	}
	PostReply();
}

void UDialogWidget::ReplyNo()
{
	if (OnReply.IsBound())
	{
		OnReply.Execute(false);
	}
	PostReply();
}

void UDialogWidget::PostReply()
{
	if (ParentWidget)
	{
		ParentWidget->SetFocus();
	}
	
	RemoveFromParent();
}