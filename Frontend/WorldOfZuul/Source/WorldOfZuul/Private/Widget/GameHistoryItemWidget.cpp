// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/GameHistoryItemWidget.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Gameplay/WOZGameplayData.h"

void UGameHistoryItemWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Button_Detail->OnReleased.AddDynamic(this, &UGameHistoryItemWidget::Detail);
}

UGameHistoryItemWidget* UGameHistoryItemWidget::CreateGameHistoryItemWidget(TSubclassOf<UGameHistoryItemWidget> Class, UUserWidget* Parent, const FWOZGameHistoryData& GameHistoryData)
{
	check(Class);
	
	UGameHistoryItemWidget* GameHistoryItemWidget = CreateWidget<UGameHistoryItemWidget>(Parent, Class);
	check(GameHistoryItemWidget);

	GameHistoryItemWidget->TextBlock_Date->SetText(FText::FromString(GameHistoryData.DateString));
	GameHistoryItemWidget->TextBlock_Score->SetText(FText::FromString(FString::FromInt(GameHistoryData.GameScore)));
	
	return GameHistoryItemWidget;
}

void UGameHistoryItemWidget::Detail()
{
	if (Border_Detail->GetVisibility() != ESlateVisibility::Collapsed)
	{
		Border_Detail->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		Border_Detail->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}
