// Fill out your copyright notice in the Description page of Project Settings.

#include "Gameplay/WOZPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Blueprint/UserWidget.h"
#include "Engine/SceneCapture2D.h"
#include "Gameplay/WOZGameItem.h"
#include "Gameplay/WOZGameMode.h"
#include "Gameplay/WOZGameplayData.h"
#include "Gameplay/WOZGameRoom.h"
#include "Gameplay/WOZPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Widget/OverlayWidget.h"

void AWOZPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (SceneCapture2D = Cast<ASceneCapture2D>(UGameplayStatics::GetActorOfClass(this, ASceneCapture2D::StaticClass())))
	{
		SetViewTarget(SceneCapture2D);
	}
}

void AWOZPlayerController::AcknowledgePossession(APawn* P)
{
	Super::AcknowledgePossession(P);

	check(OverlayWidgetClass);

	WOZPlayerState = GetPlayerState<AWOZPlayerState>();
	
	SetupInput();
	SetShowMouseCursor(true);

	GetPawn()->OnActorBeginOverlap.AddDynamic(this, &AWOZPlayerController::AWOZPlayerController::OnPawnBeginOverlap);
	GetPawn()->OnActorEndOverlap.AddDynamic(this,  &AWOZPlayerController::AWOZPlayerController::OnPawnEndOverlap);

	if (IsLocalController())
	{
		check(OverlayWidgetClass);

		OverlayWidget = CreateWidget<UOverlayWidget>(this, OverlayWidgetClass);
		OverlayWidget->AddToViewport();
	}
}

void AWOZPlayerController::SetupInput()
{
	
	UEnhancedInputLocalPlayerSubsystem* Subsystem = GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	Subsystem->AddMappingContext(Context, 0);
	
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AWOZPlayerController::Move);
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &AWOZPlayerController::Move);
	EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &AWOZPlayerController::Interact);
}

void AWOZPlayerController::OnPawnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (AWOZGameItem* GameItem = Cast<AWOZGameItem>(OtherActor))
	{
		CurrentOverlappingGameItem = GameItem;
	}
}

void AWOZPlayerController::OnPawnEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (AWOZGameItem* GameItem = Cast<AWOZGameItem>(OtherActor))
	{
		if (GameItem == CurrentOverlappingGameItem)
		{
			CurrentOverlappingGameItem = nullptr;
		}
	}
}

void AWOZPlayerController::Move(const FInputActionValue& Value)
{
	const FVector& InputAxis = Value.Get<FVector>();
	GetPawn()->AddMovementInput(InputAxis);
}

void AWOZPlayerController::Interact()
{
	check(GameplayData);
	if (!CurrentOverlappingGameItem) return;

	const FWOZGameItemInfo& ItemInfo = GameplayData->Items.FindRef(CurrentOverlappingGameItem->GetItemEnum());
	if (ItemInfo.InMapDefaultCommand == EWOZCommand::None) return;
	
	for (TTuple<FString, TEnumAsByte<EWOZCommand::Type>> StringCommand : GameplayData->StringCommands)
	{
		if (StringCommand.Value == ItemInfo.InMapDefaultCommand)
		{
			ExecuteCommand(FString(StringCommand.Key));
			return;
		}
	}
}

void AWOZPlayerController::ExecuteCommand_Implementation(const FString& CommandStr)
{
	check(GameplayData);
	
	UE_LOG(LogTemp, Log, TEXT("服务器尝试执行命令：%s。"), *CommandStr);

	FWOZCommandReplyMsg Msg;
	Msg.Command = FText::FromString(CommandStr);
	
	FString LeftStr, RightStr;

	if (CommandStr.Contains(" "))
	{
		CommandStr.Split(FString(" "), &LeftStr, &RightStr, ESearchCase::IgnoreCase);
	}
	else
	{
		LeftStr = CommandStr;
	}

	if (!GameplayData->StringCommands.Contains(LeftStr.ToLower()))
	{
		Msg.Reply = GameplayData->CommandInvalidMsg;
		ReplyCommand(Msg);
		return;
	}

	const EWOZCommand::Type Command = GameplayData->StringCommands[LeftStr.ToLower()];
	const FString& Target = RightStr.ToLower();
	switch (Command)
	{
	case EWOZCommand::Go:
		Msg.Reply = CommandGo(Target);
		break;
		
	case EWOZCommand::Take:
		Msg.Reply = CommandTake(Target);
		break;

	case EWOZCommand::Open:
		Msg.Reply = CommandOpen(Target);
		break;

	case EWOZCommand::Back:
		Msg.Reply = CommandBack();
		break;
		
	case EWOZCommand::Use:
		Msg.Reply = CommandUse(Target);
		break;
		
	case EWOZCommand::Drop:
		Msg.Reply = CommandDrop(Target);
		break;
		
	case EWOZCommand::Eat:
		Msg.Reply = CommandEat(Target);
		break;
		
	case EWOZCommand::Look:
		Msg.Reply = CommandLook();
		break;
		
	case EWOZCommand::Item:
		Msg.Reply = CommandItem();
		break;
		
	default: ;
	}

	ReplyCommand(Msg);
}

void AWOZPlayerController::ReplyCommand_Implementation(const FWOZCommandReplyMsg& Msg)
{
	check(OverlayWidget);

	OverlayWidget->AddCommandReplyMsg(Msg);
}

FText AWOZPlayerController::CommandGo(const FString& Target)
{
	check(GameplayData && WOZPlayerState && SceneCapture2D);

	AWOZGameMode* GameMode = Cast<AWOZGameMode>(GetWorld()->GetAuthGameMode());
	check(GameMode);
	
	EWOZGameRoomDirection::Type Direction = GameplayData->StringDirections.FindRef(Target);
	if (Direction == EWOZGameRoomDirection::None)
	{
		return GameplayData->CommandGoNoDirectionMsg;
	}

	FIntPoint NewPosition = WOZPlayerState->GetCurrentRoomPosition();

	switch (Direction)
	{
	case EWOZGameRoomDirection::East:
		NewPosition.Y++;
		break;

	case EWOZGameRoomDirection::West:
		NewPosition.Y--;
		break;
		
	case EWOZGameRoomDirection::North:
		NewPosition.X++;
		break;
		
	case EWOZGameRoomDirection::South:
		NewPosition.X--;
		break;
		
	default: ;
	}


	AWOZGameRoom* Room = GameMode->GetRoomByPosition(NewPosition);
	if (!Room)
	{
		return GameplayData->CommandGoInvalidRoomMsg;
	}
	
	WOZPlayerState->PushRoomPosition(NewPosition);
	const FVector& RoomLocation = Room->GetActorLocation();
	const float PrevCaptureZ = SceneCapture2D->GetActorLocation().Z;

	GetPawn()->SetActorLocation(FVector(RoomLocation.X, RoomLocation.Y, 92.0));
	SceneCapture2D->SetActorLocation(FVector(RoomLocation.X, RoomLocation.Y, PrevCaptureZ));

	FString Str = GameplayData->CommandGoSuccessfulPrefixMsg.ToString();
	Str += FString::FromInt(Room->GetRoomData().Position.X) + " , " + FString::FromInt(Room->GetRoomData().Position.Y);
	Str += TEXT("。");
	return FText::FromString(Str);
}

FText AWOZPlayerController::CommandTake(const FString& Target)
{
	check(GameplayData && WOZPlayerState);

	AWOZGameMode* GameMode = Cast<AWOZGameMode>(GetWorld()->GetAuthGameMode());
	check(GameMode);
	AWOZGameRoom* Room = GameMode->GetRoomByPosition(WOZPlayerState->GetCurrentRoomPosition());
	check(Room);
	
	TArray<AWOZGameItem*> Items;
	TArray<TEnumAsByte<EWOZGameItem::Type>> ItemEnums;
	
	//目标为空，则判断当前重叠物品
	if (Target.IsEmpty())
	{
		if (CurrentOverlappingGameItem)
		{
			if (GameplayData->IsTakeableItem(CurrentOverlappingGameItem->GetItemEnum()))
			{
				Items.Emplace(CurrentOverlappingGameItem);
				ItemEnums.Emplace(CurrentOverlappingGameItem->GetItemEnum());
			}
			else
			{
				FString Str = GameplayData->CommandTakeInvalidTypePrefixMsg.ToString();
				Str += GameplayData->GetItemNameString(CurrentOverlappingGameItem->GetItemEnum());
				Str += TEXT("。");
				return FText::FromString(Str);
			}
		}
		else
		{
			return GameplayData->CommandTakeNearNoItemMsg;
		}
	}
	else
	{
		//拾取房间中所有物品
		if (Target.ToLower() == "all")
		{
			for (AWOZGameItem* Item : Room->GetAllItems())
			{
				if (GameplayData->IsTakeableItem(Item->GetItemEnum()))
				{
					Items.Emplace(Item);
					ItemEnums.Emplace(Item->GetItemEnum());
				}	
			}

			if (Items.IsEmpty())
			{
				return GameplayData->CommandTakeRoomNoItemMsg;
			}
		}
		//拾取房间中的目标物品(最近的一个)
		else
		{
			EWOZGameItem::Type ItemEnum = GameplayData->GetItemByString(Target);
			
			if (!GameplayData->IsTakeableItem(ItemEnum))
			{
				FString Str = GameplayData->CommandTakeInvalidTypePrefixMsg.ToString();
				const FString& Name = GameplayData->GetItemNameString(ItemEnum);
				if (GameplayData->Items.FindRef(ItemEnum).Name.ToString().IsEmpty())
				{
					Str += TEXT("一个不存在的物品");
				}
				else
				{
					Str += Name;
				}
				Str += TEXT("。");
				return FText::FromString(Str);
			}
			
			AWOZGameItem* NearestItem = GetNearestItem(ItemEnum, Room);

			if (NearestItem)
			{
				Items.Emplace(NearestItem);
				ItemEnums.Emplace(NearestItem->GetItemEnum());
			}
			else
			{
				return GameplayData->CommandTakeRoomNoItemMsg;
			}
		}
	}


	////
	int32 WeightSum = 0;
	for (EWOZGameItem::Type Enum : ItemEnums)
	{
		WeightSum += GameplayData->Items.FindRef(Enum).Weight;
	}
	
	if (WOZPlayerState->GetCurrentWeight(GameplayData) + WeightSum > WOZPlayerState->MaxWeight)
	{
		return GameplayData->CommandTakeOutWeightMsg;
	}
	
	WOZPlayerState->AddBagItems(ItemEnums);
	Room->RemoveItems(Items);

	const TMap<EWOZGameItem::Type, int32>& Counter = GameplayData->GetItemCounterMap(ItemEnums);
	const FString& RetMsg = TEXT("你拾取了:\n") + GameplayData->ItemCounterToString(Counter);
	
	return FText::FromString(RetMsg);
}

FText AWOZPlayerController::CommandOpen(const FString& Target)
{
	check(GameplayData && WOZPlayerState);
	AWOZGameMode* GameMode = Cast<AWOZGameMode>(GetWorld()->GetAuthGameMode());
	check(GameMode);
	AWOZGameRoom* Room = GameMode->GetRoomByPosition(WOZPlayerState->GetCurrentRoomPosition());
	check(Room);

	AWOZGameItem* Item = nullptr;
	EWOZGameItem::Type ItemEnum = EWOZGameItem::None;
	
	if (Target.IsEmpty())
	{
		if (CurrentOverlappingGameItem)
		{
			if (GameplayData->IsOpenableItem(CurrentOverlappingGameItem->GetItemEnum()))
			{
				Item = CurrentOverlappingGameItem;
				ItemEnum = CurrentOverlappingGameItem->GetItemEnum();
			}
			else
			{
				return GameplayData->CommandOpenInvalidTypeMsg;
			}
		}
		else
		{
			return GameplayData->CommandOpenNearNoItemMsg;
		}
	}
	else
	{
		ItemEnum = GameplayData->GetItemByString(Target);

		if (!GameplayData->IsOpenableItem(ItemEnum))
		{
			return GameplayData->CommandOpenInvalidTypeMsg;
		}
			
		Item = GetNearestItem(ItemEnum, Room);

		if (!Item)
		{
			return GameplayData->CommandOpenRoomNoItemMsg;
		}
	}

	//如果是宝箱
	if (GameplayData->GetItemType(ItemEnum) == EWOZGameItemType::TreasureBox)
	{
		EWOZGameItem::Type ItemToReplace = EWOZGameItem::None;
		EWOZGameItem::Type ItemToUse = EWOZGameItem::None;
		switch (ItemEnum)
		{
		case EWOZGameItem::GoldenTreasureBox:
			ItemToReplace = EWOZGameItem::EmptyGoldenTreasureBox;
			ItemToUse = EWOZGameItem::GoldenKey;
			break;
		case EWOZGameItem::SilverTreasureBox:
			ItemToReplace = EWOZGameItem::EmptySilverTreasureBox;
			ItemToUse = EWOZGameItem::SilverKey;
			break;
		
		case EWOZGameItem::CopperTreasureBox:
			ItemToReplace = EWOZGameItem::EmptyCopperTreasureBox;
			ItemToUse = EWOZGameItem::CopperKey;
			break;
		
		default: ;
			return FText();
		}

		if (!WOZPlayerState->GetBagItems().Contains(ItemToUse))
		{
			FString Str = TEXT("你需要一支") + GameplayData->GetItemNameString(ItemToUse) + TEXT("来开启") + GameplayData->GetItemNameString(ItemEnum);
			Str += TEXT("。");
			return FText::FromString(Str);
		}

		Room->ReplaceItem(Item, ItemToReplace);

		const float Score = GameplayData->GetScoreFromBox(ItemEnum);
		const TArray<TEnumAsByte<EWOZGameItem::Type>>& Treasure = GameplayData->GetTreasureFromBox(ItemEnum);

		WOZPlayerState->AddScore(Score);
		WOZPlayerState->AddBagItems(Treasure);
		WOZPlayerState->RemoveBagItem(ItemToUse);
		
		const FString& CounterStr = GameplayData->ItemCounterToString(GameplayData->GetItemCounterMap(Treasure));
		FString Str = TEXT("你使用") + GameplayData->GetItemNameString(ItemToUse) + TEXT("开启了") + GameplayData->GetItemNameString(ItemEnum);
		Str +=  TEXT("\n你得到了:\n");
		Str += FString::FromInt(Score) + TEXT("个得分");
		if (!CounterStr.IsEmpty())
		{
			Str += "\n" + CounterStr;
		}
		
		return FText::FromString(Str);
	}


	return FText();
}

FText AWOZPlayerController::CommandBack()
{
	check(GameplayData && WOZPlayerState);
	AWOZGameMode* GameMode = Cast<AWOZGameMode>(GetWorld()->GetAuthGameMode());
	check(GameMode);

	if (WOZPlayerState->GetRoomPositionHistory().Num() == 1)
	{
		return GameplayData->CommandBackFromStartMsg;
	}

	WOZPlayerState->PopRoomPosition();
	AWOZGameRoom* Room = GameMode->GetRoomByPosition(WOZPlayerState->GetCurrentRoomPosition());
	check(Room);

	GotoRoom(Room);
	
	FString Str = GameplayData->CommandBackSuccessfulPrefixMsg.ToString();
	Str += FString::FromInt(Room->GetRoomData().Position.X) + " , " + FString::FromInt(Room->GetRoomData().Position.Y);
	Str += TEXT("。");
	return FText::FromString(Str);
}

FText AWOZPlayerController::CommandUse(const FString& Target)
{
	check(GameplayData && WOZPlayerState);
	AWOZGameMode* GameMode = Cast<AWOZGameMode>(GetWorld()->GetAuthGameMode());
	check(GameMode);
	AWOZGameRoom* Room = GameMode->GetRoomByPosition(WOZPlayerState->GetCurrentRoomPosition());
	check(Room);

	EWOZGameItem::Type ItemEnum;

	if (Target.IsEmpty())
	{
		return GameplayData->CommandUseCommandNoItemMsg;
	}
	else
	{
		ItemEnum = GameplayData->GetItemByString(Target);

		if (!GameplayData->IsUseableItem(ItemEnum))
		{
			return GameplayData->CommandUseInvalidTypeMsg;
		}
		
		if (!WOZPlayerState->GetBagItems().Contains(ItemEnum))
		{
			return GameplayData->CommandUseBagNoItemMsg;
		}
	}

	if (GameplayData->GetItemType(ItemEnum) == EWOZGameItemType::Key)
	{
		switch (ItemEnum)
		{
		case EWOZGameItem::GoldenKey:
			return CommandOpen(GameplayData->GetItemDefaultCommandString(EWOZGameItem::GoldenTreasureBox));
			
		case EWOZGameItem::SilverKey:
			return CommandOpen(GameplayData->GetItemDefaultCommandString(EWOZGameItem::SilverTreasureBox));

		case EWOZGameItem::CopperKey:
			return CommandOpen(GameplayData->GetItemDefaultCommandString(EWOZGameItem::CopperTreasureBox));
			
		default: ;
		}
	}

	return FText();
}

FText AWOZPlayerController::CommandDrop(const FString& Target)
{
	check(GameplayData && WOZPlayerState);
	AWOZGameMode* GameMode = Cast<AWOZGameMode>(GetWorld()->GetAuthGameMode());
	check(GameMode);
	AWOZGameRoom* Room = GameMode->GetRoomByPosition(WOZPlayerState->GetCurrentRoomPosition());
	check(Room);

	TArray<TEnumAsByte<EWOZGameItem::Type>> ItemEnums;

	if (Target.IsEmpty())
	{
		return GameplayData->CommandDropCommandNoItemMsg;
	}
	if (Target.ToLower() == "all")
	{
		if (WOZPlayerState->GetBagItems().IsEmpty())
		{
			return GameplayData->CommandDropBagNoItemMsg;
		}

		ItemEnums = WOZPlayerState->GetBagItems();
	}
	else
	{
		ItemEnums.Emplace(GameplayData->GetItemByString(Target));
	}

	WOZPlayerState->RemoveBagItems(ItemEnums);
	Room->AddNewItems(ItemEnums);

	const FString& CounterStr = GameplayData->ItemCounterToString(GameplayData->GetItemCounterMap(ItemEnums));
	FString Str = GameplayData->CommandDropSuccessfulPrefixMsg.ToString();
	Str += "\n" + CounterStr;

	return FText::FromString(Str);
}

FText AWOZPlayerController::CommandEat(const FString& Target)
{
	check(GameplayData && WOZPlayerState);
	AWOZGameMode* GameMode = Cast<AWOZGameMode>(GetWorld()->GetAuthGameMode());
	check(GameMode);
	AWOZGameRoom* Room = GameMode->GetRoomByPosition(WOZPlayerState->GetCurrentRoomPosition());
	check(Room);

	TEnumAsByte<EWOZGameItem::Type> ItemEnum;

	if (Target.IsEmpty())
	{
		return GameplayData->CommandEatCommandNoItemMsg;
	}
	else
	{
		ItemEnum = GameplayData->GetItemByString(Target);
	}

	if (!GameplayData->IsEatableItem(ItemEnum))
	{
		return GameplayData->CommandEatInvalidTypeMsg;
	}

	if (!WOZPlayerState->GetBagItems().Contains(ItemEnum))
	{
		return GameplayData->CommandEatBagNoItemMsg;
	}

	WOZPlayerState->RemoveBagItem(ItemEnum);
	
	FString Str = GameplayData->CommandEatSuccessfulPrefixMsg.ToString() + GameplayData->GetItemNameString(ItemEnum) + TEXT("。");

	//根据食物的类型决定效果
	switch (ItemEnum)
	{
	case EWOZGameItem::MagicCookie:
		WOZPlayerState->AddMaxWeight(GameplayData->MagicCookieMaxWeightAddCount);
		Str += TEXT("\n你的最大负重上升了") + FString::FromInt(GameplayData->MagicCookieMaxWeightAddCount) + TEXT("。");
		break;
		
	default: ;
	}
	
	return FText::FromString(Str);
}

FText AWOZPlayerController::CommandLook()
{
	check(GameplayData && WOZPlayerState);
	AWOZGameMode* GameMode = Cast<AWOZGameMode>(GetWorld()->GetAuthGameMode());
	check(GameMode);
	AWOZGameRoom* Room = GameMode->GetRoomByPosition(WOZPlayerState->GetCurrentRoomPosition());
	check(Room);

	TArray<TEnumAsByte<EWOZGameItem::Type>> ItemEnums;
	for (AWOZGameItem* Item : Room->GetAllItems())
	{
		ItemEnums.Emplace(Item->GetItemEnum());
	}
	const FString& CounterStr = GameplayData->ItemCounterToString(GameplayData->GetItemCounterMap(ItemEnums));

	FString Str = TEXT("你所在的房间位于:");
	Str += FString::FromInt(Room->GetRoomData().Position.X) + " , " + FString::FromInt(Room->GetRoomData().Position.Y);

	if (ItemEnums.IsEmpty())
	{
		Str += "\n" + GameplayData->CommandLookRoomNoItemMsg.ToString();
	}
	else
	{
		Str += TEXT("\n房间内存在物品:\n");
		Str += CounterStr;
	}

	return FText::FromString(Str);
}

FText AWOZPlayerController::CommandItem()
{
	check(GameplayData && WOZPlayerState);
	AWOZGameMode* GameMode = Cast<AWOZGameMode>(GetWorld()->GetAuthGameMode());
	check(GameMode);
	AWOZGameRoom* Room = GameMode->GetRoomByPosition(WOZPlayerState->GetCurrentRoomPosition());
	check(Room);

	const TArray<TEnumAsByte<EWOZGameItem::Type>>& ItemEnums = WOZPlayerState->GetBagItems();
	
	if (ItemEnums.IsEmpty())
	{
		return GameplayData->CommandItemBagNoItemMsg;
	}
	
	const FString& CounterStr = GameplayData->ItemCounterToString(GameplayData->GetItemCounterMap(ItemEnums));
	FString Str = TEXT("\n背包中的物品:\n");
	Str += CounterStr;
	Str += TEXT("\n总重量: ") + FString::FromInt(WOZPlayerState->GetCurrentWeight(GameplayData)) + TEXT("。");

	return FText::FromString(Str);
}

AWOZGameItem* AWOZPlayerController::GetNearestItem(EWOZGameItem::Type ItemEnum, AWOZGameRoom* Room) const
{
	float MinDis = INT32_MAX;
	AWOZGameItem* NearestItem = nullptr;
	for (AWOZGameItem* _Item_ : Room->GetAllItems())
	{
		const float Dis = _Item_->GetDistanceTo(GetPawn());
		if (_Item_->GetItemEnum() == ItemEnum && Dis < MinDis)
		{
			NearestItem = _Item_;
			MinDis = Dis;
		}
	}

	return NearestItem;
}

void AWOZPlayerController::GotoRoom(AWOZGameRoom* Room)
{
	const FVector& RoomLocation = Room->GetActorLocation();
	const float PrevCaptureZ = SceneCapture2D->GetActorLocation().Z;

	GetPawn()->SetActorLocation(FVector(RoomLocation.X, RoomLocation.Y, 92.0));
	SceneCapture2D->SetActorLocation(FVector(RoomLocation.X, RoomLocation.Y, PrevCaptureZ));
}
