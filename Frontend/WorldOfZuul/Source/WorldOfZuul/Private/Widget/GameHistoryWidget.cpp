// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/GameHistoryWidget.h"
#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "Widget/GameHistoryItemWidget.h"

void UGameHistoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetIsFocusable(true);

	Button_Close->OnReleased.AddDynamic(this, &UGameHistoryWidget::Close);
}

UGameHistoryWidget* UGameHistoryWidget::DisplayGameHistoryWidget(TSubclassOf<UGameHistoryWidget> Class, UObject* Owner, const TArray<FWOZGameHistoryData>& GameHistoryDatas)
{
	if (!Class) return nullptr;
	
	UGameHistoryWidget* GameHistoryWidget = CreateWidget<UGameHistoryWidget>(Cast<UUserWidget>(Owner), Class);
	if (!GameHistoryWidget) return nullptr;

	for (const FWOZGameHistoryData& GameHistoryData : GameHistoryDatas)
	{
		UGameHistoryItemWidget* ItemWidget = UGameHistoryItemWidget::CreateGameHistoryItemWidget(GameHistoryWidget->GameHistoryItemWidgetClass, GameHistoryWidget, GameHistoryData);
		GameHistoryWidget->ScrollBox_History->AddChild(ItemWidget);
	}
	
	GameHistoryWidget->ParentWidget = Cast<UUserWidget>(Owner);
	GameHistoryWidget->AddToViewport(64);
	GameHistoryWidget->SetFocus();
	
	return GameHistoryWidget;
}

void UGameHistoryWidget::Close()
{
	if (ParentWidget)
	{
		ParentWidget->SetFocus();
	}
	
	RemoveFromParent();
}
