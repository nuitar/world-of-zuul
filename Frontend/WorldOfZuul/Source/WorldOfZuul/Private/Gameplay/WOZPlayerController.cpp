// Fill out your copyright notice in the Description page of Project Settings.

#include "Gameplay/WOZPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "HttpModule.h"
#include "InputActionValue.h"
#include "JsonObjectConverter.h"
#include "Blueprint/UserWidget.h"
#include "Components/CapsuleComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/SceneCapture2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Gameplay/WOZGameInstance.h"
#include "Gameplay/WOZGameItem.h"
#include "Gameplay/WOZGameMode.h"
#include "Gameplay/WOZGameplayData.h"
#include "Gameplay/WOZGameRoom.h"
#include "Gameplay/WOZPlayerCharacter.h"
#include "Gameplay/WOZPlayerState.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Kismet/GameplayStatics.h"
#include "Widget/OverlayWidget.h"

void AWOZPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	check(GameplayData);
	
	WOZPlayerState = GetPlayerState<AWOZPlayerState>();

	GetPawn()->OnActorBeginOverlap.AddDynamic(this, &AWOZPlayerController::AWOZPlayerController::OnPawnBeginOverlap);
	GetPawn()->OnActorEndOverlap.AddDynamic(this,  &AWOZPlayerController::AWOZPlayerController::OnPawnEndOverlap);

	if (IsLocalController())
	{
		check(OverlayWidgetClass);
		OverlayWidget = CreateWidget<UOverlayWidget>(this, OverlayWidgetClass);
		OverlayWidget->AddToViewport();
		
		if (ASceneCapture2D* SC2D = Cast<ASceneCapture2D>(UGameplayStatics::GetActorOfClass(this, ASceneCapture2D::StaticClass())))
		{
			UTextureRenderTarget2D* RenderTarget2D = NewObject<UTextureRenderTarget2D>(this);
			RenderTarget2D->InitAutoFormat(2048, 2048);
			SceneCapture2D = SC2D;
			SceneCapture2D->GetCaptureComponent2D()->TextureTarget = RenderTarget2D;
			SetViewTarget(SceneCapture2D);

			OverlayWidget->SetRenderTarget(RenderTarget2D);
		}

	
		SetupInput();
		SetShowMouseCursor(true);
	}
	
	UWOZGameInstance* GameInstance = Cast<UWOZGameInstance>(GetGameInstance());
	check(GameInstance);	
	
	if (GetNetMode() == NM_Standalone)
	{
		if (!GameInstance->bIsNewGame)
		{
			AWOZGameMode* GameMode = Cast<AWOZGameMode>(GetWorld()->GetAuthGameMode());
			check(GameMode);

			CommandReplyMsgs = GameInstance->SinglePlayerSaveGameData.CommandReplyMsgs;
			for (const FWOZCommandReplyMsg& CommandReplyMsg : CommandReplyMsgs)
			{
				OverlayWidget->AddCommandReplyMsg(CommandReplyMsg);
			}
		
			WOZPlayerState->AddBagItems(GameInstance->SinglePlayerSaveGameData.BagItems);
			WOZPlayerState->AddScore(GameInstance->SinglePlayerSaveGameData.GameScore);
			WOZPlayerState->RoomPositionHistory = GameInstance->SinglePlayerSaveGameData.RoomPositionHistory;
			WOZPlayerState->RoomPositionHistory.Pop();
			WOZPlayerState->PushRoomPosition(GameInstance->SinglePlayerSaveGameData.RoomPositionHistory.Last());
			WOZPlayerState->SetMaxWeight(GameInstance->SinglePlayerSaveGameData.MaxWeight);
		
			GotoRoom(GameMode->GetRoomByPosition(WOZPlayerState->GetCurrentRoomPosition()));
			GetPawn()->SetActorTransform(GameInstance->SinglePlayerSaveGameData.PlayerTransform);

			return;
		}
	}
	
	WOZPlayerState->SetMaxWeight(GameplayData->PlayerDefaultMaxWeight);
	FWOZCommandReplyMsg Msg;
	Msg.Command = FText::FromString(TEXT("welcome"));
	FString Str = TEXT("欢迎来到World of Zuul。");
	Str += TEXT("\n输入\"help\"或使用快捷指令查看帮助。");
	Str += TEXT("\n祝您游戏愉快。");
	Msg.Reply = FText::FromString(Str);
	ReplyCommand(Msg);
}

void AWOZPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (!GetPlayerState<AWOZPlayerState>()) return;
	if (OverlayWidget) return;
	
	WOZPlayerState = GetPlayerState<AWOZPlayerState>();
	
	if (IsLocalController())
	{
		check(OverlayWidgetClass);
		OverlayWidget = CreateWidget<UOverlayWidget>(this, OverlayWidgetClass);
		OverlayWidget->AddToViewport();

		if (SceneCapture2D)
		{
			OverlayWidget->SetRenderTarget(SceneCapture2D->GetCaptureComponent2D()->TextureTarget);
		}
	}
}

void AWOZPlayerController::OnRep_Pawn()
{
	Super::OnRep_Pawn();

	if (!GetPawn()) return;
	if (SceneCapture2D) return;
	
	if (ASceneCapture2D* SC2D = Cast<ASceneCapture2D>(UGameplayStatics::GetActorOfClass(this, ASceneCapture2D::StaticClass())))
	{
		UTextureRenderTarget2D *RenderTarget2D = NewObject<UTextureRenderTarget2D>(this);
		RenderTarget2D->InitAutoFormat(2048, 2048);
		SceneCapture2D = SC2D;
		SceneCapture2D->GetCaptureComponent2D()->TextureTarget = RenderTarget2D;
		SetViewTarget(SceneCapture2D);

		if (OverlayWidget)
		{
			OverlayWidget->SetRenderTarget(RenderTarget2D);
		}
	}
	
	SetupInput();
	SetShowMouseCursor(true);
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

		if (GameItem->GetItemEnum() == EWOZGameItem::TeleportDoor)
		{
			ExecuteCommand_ItemTarget(EWOZCommand::Use, EWOZGameItem::TeleportDoor);
		}
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

void AWOZPlayerController::Interact_Implementation()
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

			//刷新碰撞
			if (AWOZPlayerCharacter* PlayerCharacter = Cast<AWOZPlayerCharacter>(GetPawn()))
			{
				PlayerCharacter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				PlayerCharacter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			}
			
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
		Msg.Reply = FText::FromString(TEXT("你输入了一个无效的指令。"));
		CommandReplyMsgs.Emplace(Msg);
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
		
	case EWOZCommand::Save:
		if (Target.ToLower() == FString("game"))
		{
			CommandSaveGame();
			return;
		}
		if (Target.ToLower() == FString("settings"))
		{
			CommandSaveSetting();
			Msg.Reply = FText::FromString(TEXT("游戏设置已保存。"));
		}
		else
		{
			Msg.Reply = FText::FromString(TEXT("你并未指定有效的保存目标。"));
		}
		break;
		
	case EWOZCommand::Help:
		Msg.Reply = CommandHelp();
		break;
		
	case EWOZCommand::Quit:
		CommandQuit();
		UGameplayStatics::OpenLevel(this, MenuMapName);
		return;
		
	default: ;
	}

	CommandReplyMsgs.Emplace(Msg);
	ReplyCommand(Msg);
}

void AWOZPlayerController::ExecuteCommand_ItemTarget(TEnumAsByte<EWOZCommand::Type> Command, TEnumAsByte<EWOZGameItem::Type> Target)
{
	FString CommandString = GameplayData->GetStringCommand(Command);
	
	if (Target != EWOZGameItem::None)
	{
		CommandString += " " + GameplayData->GetItemDefaultCommandString(Target);
	}

	ExecuteCommand(CommandString);
}

void AWOZPlayerController::ExecuteCommand_DirectionTarget(TEnumAsByte<EWOZCommand::Type> Command, TEnumAsByte<EWOZGameRoomDirection::Type> Target)
{
	FString CommandString = GameplayData->GetStringCommand(Command);
	
	if (Target != EWOZGameItem::None)
	{
		for (const auto& StringDirection : GameplayData->StringDirections)
		{
			if (StringDirection.Value == Target)
			{
				CommandString += " " + StringDirection.Key;
				break;
			}
		}
	}
	
	ExecuteCommand(CommandString);
}

void AWOZPlayerController::OnGameRemainTimeTick(float RemainTime)
{
	if (OverlayWidget)
	{
		OverlayWidget->SetGameRemainTime(RemainTime);
	}
}

void AWOZPlayerController::OnGameEnded()
{
	check(WOZPlayerState);
	UWOZGameInstance* GameInstance = Cast<UWOZGameInstance>(GetGameInstance());
	check(GameInstance);

	SaveClear();
	
	//保存对局记录
	FWOZGameHistoryData GameHistoryData;
	GameHistoryData.GameScore = WOZPlayerState->GetGameScore();
	FString GameHistoryDataStr;
	FJsonObjectConverter::UStructToJsonObjectString(GameHistoryData, GameHistoryDataStr);
	
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetNumberField("userId", GameInstance->UserID);
	JsonObject->SetStringField("gameHistoryData", GameHistoryDataStr);

	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
	Request->SetHeader("Content-Type", "application/json;charset=UTF-8");
	Request->SetContentAsString(RequestBody);
	Request->SetURL(SaveHistoryURL);
	Request->SetVerb("POST");
	Request->ProcessRequest();
	
	OnGameEnded_Client();
	
	FWOZCommandReplyMsg Msg;
	Msg.Command = FText::FromString("game ended");
	FString Str = TEXT("游戏结束。\n");
	Str += TEXT("你的最终得分是：\n");
	Str += FString::FromInt(WOZPlayerState->GetGameScore());
	Str += TEXT("。\n你会在5秒后返回菜单。");
	Msg.Reply = FText::FromString(Str);
	ReplyCommand(Msg);
}

void AWOZPlayerController::SaveClear()
{
	UWOZGameInstance* GameInstance = Cast<UWOZGameInstance>(GetGameInstance());
	check(GameInstance);
	
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();

	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetNumberField("userId", GameInstance->UserID);
	JsonObject->SetStringField("savegamedata", FString());

	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	Request->SetHeader("Content-Type", "application/json;charset=UTF-8");
	Request->SetContentAsString(RequestBody);
	Request->SetURL(SaveGameURL);
	Request->SetVerb("POST");
	
	Request->ProcessRequest();
}

void AWOZPlayerController::OnGameEnded_Client_Implementation()
{
	DisableInput(this);
	
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(
		TimerHandle,
		FTimerDelegate::CreateLambda([&]
			{
				if (!this) return;
				UGameplayStatics::OpenLevel(this, MenuMapName);
			}),5.f, false);
}

void AWOZPlayerController::ReplyCommand_Implementation(const FWOZCommandReplyMsg& Msg)
{
	check(OverlayWidget);

	OverlayWidget->AddCommandReplyMsg(Msg);
}

FText AWOZPlayerController::CommandGo(const FString& Target)
{
	check(GameplayData && WOZPlayerState);

	AWOZGameMode* GameMode = Cast<AWOZGameMode>(GetWorld()->GetAuthGameMode());
	check(GameMode);
	
	EWOZGameRoomDirection::Type Direction = GameplayData->StringDirections.FindRef(Target);
	if (Direction == EWOZGameRoomDirection::None)
	{
		if (Target == GameplayData->GetStringCommand(EWOZCommand::Back))
		{
			return CommandBack();
		}
		
		return FText::FromString(TEXT("你并未指定有效的方向。"));
	}

	FIntPoint NewPosition = WOZPlayerState->GetCurrentRoomPosition();
	FVector NextRoomOffset = FVector::ZeroVector;

	switch (Direction)
	{
	case EWOZGameRoomDirection::East:
		NewPosition.Y++;
		NextRoomOffset.Y = -GameplayData->RoomSize / 2.5f;
		break;

	case EWOZGameRoomDirection::West:
		NewPosition.Y--;
		NextRoomOffset.Y = GameplayData->RoomSize / 2.5f;
		break;
		
	case EWOZGameRoomDirection::North:
		NewPosition.X++;
		NextRoomOffset.X = -GameplayData->RoomSize / 2.5f;
		break;
		
	case EWOZGameRoomDirection::South:
		NewPosition.X--;
		NextRoomOffset.X = GameplayData->RoomSize / 2.5f;
		break;
		
	default: ;
	}


	AWOZGameRoom* Room = GameMode->GetRoomByPosition(NewPosition);
	if (!Room)
	{
		return FText::FromString(TEXT("目标方向不存在有效的房间。"));
	}
	
	WOZPlayerState->PushRoomPosition(NewPosition);
	GotoRoom(Room);

	GetPawn()->AddActorWorldOffset(NextRoomOffset);
	
	FString Str = TEXT("你来到了房间");
	Str += "(" + FString::FromInt(Room->GetRoomData().Position.X) + " , " + FString::FromInt(Room->GetRoomData().Position.Y) + ")";
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
				FString Str = TEXT("你无法拾取");
				Str += GameplayData->GetItemNameString(CurrentOverlappingGameItem->GetItemEnum());
				Str += TEXT("。");
				return FText::FromString(Str);
			}
		}
		else
		{
			return FText::FromString(TEXT("你的身边没有可拾取的物品。"));
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
				return FText::FromString(TEXT("房间中没有可拾取的物品。"));
			}
		}
		//拾取房间中的目标物品(最近的一个)
		else
		{
			EWOZGameItem::Type ItemEnum = GameplayData->GetItemByString(Target);
			
			if (!GameplayData->IsTakeableItem(ItemEnum))
			{
				const FString& Name = GameplayData->GetItemNameString(ItemEnum);
				FString Str = TEXT("你无法拾取");
				Str += Name + TEXT("。");
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
				FString Str = TEXT("房间中没有");
				const FString& Name = GameplayData->GetItemNameString(ItemEnum);
				Str += Name + TEXT("。");
				return FText::FromString(Str);
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
		return FText::FromString(TEXT("你无法携带超出最大负重的物品。"));
	}
	
	WOZPlayerState->AddBagItems(ItemEnums);
	Room->RemoveItems(Items);

	const TMap<EWOZGameItem::Type, int32>& Counter = GameplayData->GetItemCounterMap(ItemEnums);
	const FString& RetMsg = TEXT("你拾取了:\n") + GameplayData->ItemCounterToString(Counter) + TEXT("。");
	
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
				FString Str = TEXT("你无法对");
				const FString& Name = GameplayData->GetItemNameString(CurrentOverlappingGameItem->GetItemEnum());
				Str += Name + TEXT("进行开启操作。");
				return FText::FromString(Str);
			}
		}
		else
		{
			return FText::FromString(TEXT("你的身边没有可以进行开启操作的事物。"));
		}
	}
	else
	{
		ItemEnum = GameplayData->GetItemByString(Target);

		if (!GameplayData->IsOpenableItem(ItemEnum))
		{
			FString Str = TEXT("你无法对");
			const FString& Name = GameplayData->GetItemNameString(ItemEnum);
			Str += Name + TEXT("进行开启操作。");
			return FText::FromString(Str);
		}
			
		Item = GetNearestItem(ItemEnum, Room);

		if (!Item)
		{
			FString Str = TEXT("房间中没有");
			const FString& Name = GameplayData->GetItemNameString(ItemEnum);
			Str += Name + TEXT("。");
			return FText::FromString(Str);
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
		FString Str = TEXT("你使用") + GameplayData->GetItemNameString(ItemToUse) + TEXT("开启了") + GameplayData->GetItemNameString(ItemEnum) + TEXT("。");
		Str +=  TEXT("\n你得到了:\n");
		Str += FString::FromInt(Score) + TEXT("个得分");
		if (!CounterStr.IsEmpty())
		{
			Str += "\n" + CounterStr;
		}
		TEXT("。");
		
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
		return FText::FromString(TEXT("你没有用于返回上一个房间的记录。"));
	}

	WOZPlayerState->PopRoomPosition();
	AWOZGameRoom* Room = GameMode->GetRoomByPosition(WOZPlayerState->GetCurrentRoomPosition());
	check(Room);

	GotoRoom(Room);
	
	FString Str = TEXT("你回到了房间");
	Str += "(" + FString::FromInt(Room->GetRoomData().Position.X) + " , " + FString::FromInt(Room->GetRoomData().Position.Y) + ")";
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
		if (CurrentOverlappingGameItem)
		{
			if (GameplayData->IsUseableItem(CurrentOverlappingGameItem->GetItemEnum()))
			{
				ItemEnum = CurrentOverlappingGameItem->GetItemEnum();
			}
			else
			{
				FString Str = TEXT("你无法使用");
				Str += GameplayData->GetItemNameString(CurrentOverlappingGameItem->GetItemEnum());        
				Str += TEXT("。");
				return FText::FromString(Str);
			}
		}
		else
		{
			return FText::FromString(TEXT("你的附近没有能够使用的事物。"));
		}
	}
	else
	{
		ItemEnum = GameplayData->GetItemByString(Target);

		if (!GameplayData->IsUseableItem(ItemEnum))
		{
			FString Str = TEXT("你无法使用");
			Str += GameplayData->GetItemNameString(ItemEnum);        
			Str += TEXT("。");
			return FText::FromString(Str);
		}
	}

	//如果是钥匙，需要进行较为复杂的判断和预处理
	if (GameplayData->GetItemType(ItemEnum) == EWOZGameItemType::Key)
	{
		//判断地图中是否有对应的宝箱
		bool bRoomHasTreasureBox = false;
		EWOZGameItem::Type NeededTreasureBox = EWOZGameItem::None;
		switch (ItemEnum)
		{
		case EWOZGameItem::GoldenKey:
			NeededTreasureBox = EWOZGameItem::GoldenTreasureBox;
			break;
				
		case EWOZGameItem::SilverKey:
			NeededTreasureBox = EWOZGameItem::SilverTreasureBox;
			break;
				
		case EWOZGameItem::CopperKey:
			NeededTreasureBox = EWOZGameItem::CopperTreasureBox;
			break;
				
		default: ;
		}
		
		bRoomHasTreasureBox = Room->GetRoomData().Items.Contains(NeededTreasureBox);
		if (!bRoomHasTreasureBox)
		{
			FString Str = TEXT("房间中没有");
			Str += GameplayData->GetItemNameString(NeededTreasureBox);        
			Str += TEXT("。");
			return FText::FromString(Str);
		}

		//判断是否要从地图中拿取钥匙
		bool bTakeKeyFromRoom = false;
		//未指定目标，则表示使用当前重叠的物品，则需要拿取
		if (Target.IsEmpty())
		{
			bTakeKeyFromRoom = true;
		}
		//指定了目标，如果背包里没有这个物品，则需要拿取
		else
		{
			bTakeKeyFromRoom = !WOZPlayerState->GetBagItems().Contains(ItemEnum);
		}

		//需要从地图中拿，但地图中也没有钥匙，则返回错误
		if (bTakeKeyFromRoom && !Room->GetAllItemEnums().Contains(ItemEnum))
		{
			FString Str = TEXT("房间和你的背包中均没有");
			Str += GameplayData->GetItemNameString(ItemEnum);        
			Str += TEXT("。");
			return FText::FromString(Str);
		}

		//从房间里拿取并放到背包里
		if (bTakeKeyFromRoom)
		{
			WOZPlayerState->AddBagItem(ItemEnum);

			if (AWOZGameItem* Item = GetNearestItem(ItemEnum, Room))
			{
				Room->RemoveItem(Item);
			}
		}
	}
	else if (GameplayData->GetItemType(ItemEnum) == EWOZGameItemType::RoomInteractive)
	{
		if (!Room->GetAllItemEnums().Contains(ItemEnum))
		{
			FString Str = TEXT("房间中没有");
			Str += GameplayData->GetItemNameString(ItemEnum);        
			Str += TEXT("。");
			return FText::FromString(Str);
		}
	}
	
	//如果是钥匙
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

	if (GameplayData->GetItemType(ItemEnum) == EWOZGameItemType::RoomInteractive)
	{
		switch (ItemEnum)
		{
		case EWOZGameItem::TeleportDoor:
			return TeleportRandom();
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
		return FText::FromString(TEXT("你并未指定需要丢弃的物品。"));
	}
	if (Target.ToLower() == "all")
	{
		if (WOZPlayerState->GetBagItems().IsEmpty())
		{
			return FText::FromString(TEXT("你的背包是空的。"));
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
	FString Str = TEXT("你丢弃了:");
	Str += "\n" + CounterStr + TEXT("。");

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
		if (CurrentOverlappingGameItem)
		{
			if (GameplayData->IsEatableItem(CurrentOverlappingGameItem->GetItemEnum()))
			{
				ItemEnum = CurrentOverlappingGameItem->GetItemEnum();
			}
			else
			{
				FString Str = TEXT("你无法食用");
				Str += GameplayData->GetItemNameString(CurrentOverlappingGameItem->GetItemEnum());        
				Str += TEXT("。");
				return FText::FromString(Str);
			}
		}
		else
		{
			return FText::FromString(TEXT("你的附近没有能够食用的事物。"));
		}
	}
	else
	{
		ItemEnum = GameplayData->GetItemByString(Target);
		
		if (!GameplayData->IsEatableItem(ItemEnum))
		{
			FString Str = TEXT("你无法食用");
			Str += GameplayData->GetItemNameString(ItemEnum);        
			Str += TEXT("。");
			return FText::FromString(Str);
		}
	}

	//判断是否要从地图中寻找食物
	bool bTakeFromRoom = false;
	//未指定目标，则表示使用当前重叠的物品，则需要拿取
	if (Target.IsEmpty())
	{
		bTakeFromRoom = true;
	}
	//指定了目标，如果背包里没有这个物品，则需要拿取
	else
	{
		bTakeFromRoom = !WOZPlayerState->GetBagItems().Contains(ItemEnum);
	}

	//需要从地图中拿，但地图中也没有，则返回错误
	if (bTakeFromRoom && !Room->GetAllItemEnums().Contains(ItemEnum))
	{
		FString Str = TEXT("房间和你的背包中均没有");
		Str += GameplayData->GetItemNameString(ItemEnum);        
		Str += TEXT("。");
		return FText::FromString(Str);
	}

	//从房间里拿取并放到背包里
	if (bTakeFromRoom)
	{
		WOZPlayerState->AddBagItem(ItemEnum);

		if (AWOZGameItem* Item = GetNearestItem(ItemEnum, Room))
		{
			Room->RemoveItem(Item);
		}
	}

	WOZPlayerState->RemoveBagItem(ItemEnum);
	
	FString Str = TEXT("你吃掉了") + GameplayData->GetItemNameString(ItemEnum) + TEXT("。");

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

	FString Str = TEXT("你位于");
	
	Str += "(" + FString::FromInt(Room->GetRoomData().Position.X) + " , " + FString::FromInt(Room->GetRoomData().Position.Y) + ")";
	Str += TEXT("的");
	if (!Room->GetRoomInfo().Name.IsEmpty())
	{
		Str += Room->GetRoomInfo().Name.ToString();
		Str += TEXT("。");
	}
	else
	{
		Str += TEXT("房间");
	}

	if (!Room->GetRoomInfo().Description.IsEmpty())
	{
		Str += TEXT("\n") + Room->GetRoomInfo().Description.ToString();
	}
	
	if (ItemEnums.IsEmpty())
	{
		Str += TEXT("\n房间内空无一物。");
	}
	else
	{
		Str += TEXT("\n房间内存在物品:\n");
		Str += CounterStr + TEXT("。");
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
		return FText::FromString(TEXT("你的背包中没有物品。"));
	}
	
	const FString& CounterStr = GameplayData->ItemCounterToString(GameplayData->GetItemCounterMap(ItemEnums));
	FString Str = TEXT("你的背包中存在物品:\n");
	Str += CounterStr + TEXT("。");
	Str += TEXT("\n总重量: ") + FString::FromInt(WOZPlayerState->GetCurrentWeight(GameplayData)) + TEXT("。");

	return FText::FromString(Str);
}

void AWOZPlayerController::CommandSaveGame()
{
	//序列化保存
	check(GameplayData && WOZPlayerState);
	UWOZGameInstance* GameInstance = Cast<UWOZGameInstance>(GetGameInstance());
	check(GameInstance);
	AWOZGameMode* GameMode = Cast<AWOZGameMode>(GetWorld()->GetAuthGameMode());
	check(GameMode);

	const TArray<AWOZGameRoom*>& Rooms = GameMode->GetAllRooms();

	FWOZSaveGameData SaveGameData;
	for (AWOZGameRoom* Room : Rooms)
	{
		SaveGameData.RoomDatas.Emplace(Room->GetRoomData());
	}
	SaveGameData.CommandReplyMsgs = CommandReplyMsgs;
	SaveGameData.PlayerTransform = GetPawn()->GetActorTransform();
	SaveGameData.GameScore = WOZPlayerState->GetGameScore();
	SaveGameData.RoomPositionHistory = WOZPlayerState->GetRoomPositionHistory();
	SaveGameData.BagItems = WOZPlayerState->GetBagItems();
	SaveGameData.MaxWeight = WOZPlayerState->GetMaxWeight();
	SaveGameData.GameRemainTime = GameMode->GetRemainTime();
	
	FString Str;
	FJsonObjectConverter::UStructToJsonObjectString(SaveGameData, Str);

	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();

	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetNumberField("userId", GameInstance->UserID);
	JsonObject->SetStringField("saveGameData", Str);

	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	Request->OnProcessRequestComplete().BindUObject(this, &AWOZPlayerController::OnSaveResponseReceived);
	Request->SetHeader("Content-Type", "application/json;charset=UTF-8");
	Request->SetContentAsString(RequestBody);
	Request->SetURL(SaveGameURL);
	Request->SetVerb("POST");
	
	Request->ProcessRequest();
}

FText AWOZPlayerController::CommandHelp()
{
	FString Str = TEXT("你可以使用W,A,S,D进行移动。");
	Str += TEXT("\n你可以按下\"F\"键与身边物体交互。");
	Str += TEXT("\n你可以使用快捷指令进行游戏。");
	Str += TEXT("\n你所保存的游戏将会被存储在服务器中。");
	Str += TEXT("\n开启宝箱会随机获得分数。");
	Str += TEXT("\n食用物品会获得加成效果。");
	Str += TEXT("\n在游戏倒计时结束前的得分越高，成绩越好。");
	Str += TEXT("\n祝您游戏愉快。");
	
	return FText::FromString(Str);
}

void AWOZPlayerController::CommandQuit_Implementation()
{
	UGameplayStatics::OpenLevel(this, MenuMapName);
}

void AWOZPlayerController::CommandSaveSetting_Implementation()
{
	
}


AWOZGameItem* AWOZPlayerController::GetNearestItem(EWOZGameItem::Type ItemEnum, AWOZGameRoom* Room) const
{
	float MinDis = (float)INT32_MAX;
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

void AWOZPlayerController::GotoRoom_Implementation(AWOZGameRoom* Room)
{
	check(GameplayData);
	
	const FVector& RoomLocation = Room->GetActorLocation();
	const float CaptureZ = GameplayData->RoomSize / 2.f;

	GetPawn()->SetActorLocation(FVector(RoomLocation.X, RoomLocation.Y, 92.0));

	if (IsLocalController())
	{
		SceneCapture2D->SetActorLocation(FVector(RoomLocation.X, RoomLocation.Y, CaptureZ));
	}
}

FText AWOZPlayerController::TeleportRandom()
{
	check(GameplayData && WOZPlayerState);
	
	AWOZGameMode* GameMode = Cast<AWOZGameMode>(GetWorld()->GetAuthGameMode());
	check(GameMode);
	
	const TArray<AWOZGameRoom*> Rooms = GameMode->GetAllRooms();
	const int32 RandIndex = FMath::RandRange(0, Rooms.Num() - 1);

	WOZPlayerState->PushRoomPosition(Rooms[RandIndex]->GetPosition());
	GotoRoom(Rooms[RandIndex]);

	FString Str = TEXT("你被传送门传送到了房间");
	Str += "(" + FString::FromInt(Rooms[RandIndex]->GetPosition().X) + " , " + FString::FromInt(Rooms[RandIndex]->GetPosition().Y) + ")";
	Str += TEXT("。");

	return FText::FromString(Str);
}

void AWOZPlayerController::OnSaveResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bRequestSuccessful)
{
	FWOZCommandReplyMsg Msg;
	Msg.Command = FText::FromString(GameplayData->GetStringCommand(EWOZCommand::Save) + " game");

	if (!bRequestSuccessful || !Response.IsValid() || !EHttpResponseCodes::IsOk(Response->GetResponseCode()))
	{
		FString Str = TEXT("保存游戏的网络请求无效。");
		Msg.Reply = FText::FromString(Str);
		ReplyCommand(Msg);
		return;
	}
	
	FString ResponseString = Response->GetContentAsString();

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseString);
	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		if (JsonObject->GetIntegerField("code") == 1)
		{
			FString Str = TEXT("游戏进度已保存至服务器。");
			Msg.Reply = FText::FromString(Str);
			ReplyCommand(Msg);
		}
		else
		{
			FString Str = TEXT("游戏进度保存失败。");
			Msg.Reply = FText::FromString(Str);
			ReplyCommand(Msg);
		}
	}
}
