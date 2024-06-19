// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/BagItemWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Widget/OverlayWidget.h"

UBagItemWidget* UBagItemWidget::CreateBagItemWidget(UOverlayWidget* Overlay, TSubclassOf<UBagItemWidget> Class, EWOZGameItem::Type Item, UWOZGameplayData* GameplayData)
{
	check(Overlay && Class && GameplayData);

	UBagItemWidget* BagItemWidget = CreateWidget<UBagItemWidget>(Overlay, Class);
	check(BagItemWidget);
	BagItemWidget->GameplayData = GameplayData;
	BagItemWidget->OverlayWidget = Overlay;
	BagItemWidget->ItemInfo = GameplayData->Items.FindRef(Item);
	FString Str = BagItemWidget->ItemInfo.Name.ToString() + " (" + TEXT("重量: ") + FString::FromInt(BagItemWidget->ItemInfo.Weight) + ")";
	BagItemWidget->TextBlock_Item->SetText(FText::FromString(Str));
	BagItemWidget->TextBlock_Func->SetText(GameplayData->CommandNames.FindRef(GameplayData->Items.FindRef(Item).InBagDefaultCommand));

	return BagItemWidget;
}

void UBagItemWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Button_Func->OnReleased.AddDynamic(this, &UBagItemWidget::OnButtonFuncReleased);
	Button_Drop->OnReleased.AddDynamic(this, &UBagItemWidget::OnButtonDropReleased);
}

void UBagItemWidget::OnButtonFuncReleased()
{
	check(GameplayData && OverlayWidget);
	
	FString Command = GameplayData->GetStringCommand(ItemInfo.InBagDefaultCommand) + " " + ItemInfo.Strings[0];
	OverlayWidget->ExecuteCommand(Command);
}

void UBagItemWidget::OnButtonDropReleased()
{
	check(GameplayData && OverlayWidget);
	
	FString Command = GameplayData->GetStringCommand(EWOZCommand::Drop) + " " + ItemInfo.Strings[0];
	OverlayWidget->ExecuteCommand(Command);
}
