// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/MenuWidget.h"
#include "HttpModule.h"
#include "JsonObjectConverter.h"
#include "Components/Button.h"
#include "Gameplay/WOZGameInstance.h"
#include "Interfaces/IHttpResponse.h"
#include "Kismet/GameplayStatics.h"
#include "Widget/DialogWidget.h"
#include "Widget/GameHistoryWidget.h"

void UMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetIsFocusable(true);
	
	LoadGame();
	LoadHistory();

	Button_SinglePlayer->OnReleased.AddDynamic(this, &UMenuWidget::SinglePlayer);
	Button_GameHistory->OnReleased.AddDynamic(this, &UMenuWidget::GameHistory);
	Button_Logout->OnReleased.AddDynamic(this, &UMenuWidget::Logout);
}

void UMenuWidget::SinglePlayer()
{
	if (!PrevSaveGameDataStr.IsEmpty())
	{
		check(DialogWidgetClass);

		UDialogWidget* DialogWidget = UDialogWidget::DisplayDialog(DialogWidgetClass, this, FText::FromString(TEXT("你有一个正在进行的游戏，是否继续该游戏进程")), true);
		DialogWidget->OnReply.BindUObject(this, &UMenuWidget::OnSinglePlayerDialogReply);
	}
	else
	{
		UGameplayStatics::OpenLevel(this, GameMapName);
	}
}

void UMenuWidget::GameHistory()
{
	check(GameHistoryWidgetClass);

	UGameHistoryWidget* GameHistoryWidget = UGameHistoryWidget::DisplayGameHistoryWidget(GameHistoryWidgetClass, this, GameHistoryDatas);
}

void UMenuWidget::Logout()
{
	UWOZGameInstance* GameInstance = Cast<UWOZGameInstance>(GetGameInstance());
	check(GameInstance);

	GameInstance->Username = FText();
	GameInstance->UserID = 0;
	GameInstance->bIsNewGame = true;
	GameInstance->SinglePlayerSaveGameData = FWOZSaveGameData();
	
	UGameplayStatics::OpenLevel(this, LoginRegisterMapName);
}

void UMenuWidget::SaveClear()
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
	Request->SetURL(SaveClearURL);
	Request->SetVerb("POST");
	
	Request->ProcessRequest();
}

void UMenuWidget::LoadGame()
{
	UWOZGameInstance* GameInstance = Cast<UWOZGameInstance>(GetGameInstance());
	check(GameInstance);
	
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();

	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetNumberField("userId", GameInstance->UserID);

	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	Request->OnProcessRequestComplete().BindUObject(this, &UMenuWidget::OnLoadGameResponseReceived);
	Request->SetHeader("Content-Type", "application/json;charset=UTF-8");
	Request->SetContentAsString(RequestBody);
	Request->SetURL(LoadGameURL);
	Request->SetVerb("POST");
	
	Request->ProcessRequest();
}

void UMenuWidget::LoadHistory()
{
	UWOZGameInstance* GameInstance = Cast<UWOZGameInstance>(GetGameInstance());
	check(GameInstance);
	
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();

	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetNumberField("userId", GameInstance->UserID);

	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	Request->OnProcessRequestComplete().BindUObject(this, &UMenuWidget::OnLoadHistoryResponseReceived);
	Request->SetHeader("Content-Type", "application/json;charset=UTF-8");
	Request->SetContentAsString(RequestBody);
	Request->SetURL(LoadHistoryURL);
	Request->SetVerb("POST");
	
	Request->ProcessRequest();
}

void UMenuWidget::OnLoadGameResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bRequestSuccessful)
{
	if (!bRequestSuccessful || !Response.IsValid() || !EHttpResponseCodes::IsOk(Response->GetResponseCode()))
	{
		return;
	}
	
	FString ResponseString = Response->GetContentAsString();

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseString);
	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		if (JsonObject->GetIntegerField("code") == 1)
		{
			const auto& Obj = JsonObject->GetObjectField("data");
			const FString& Str = Obj->GetStringField("saveGameData");
			PrevSaveGameDataStr = Str;
		}
	}
}

void UMenuWidget::OnLoadHistoryResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bRequestSuccessful)
{
	if (!bRequestSuccessful || !Response.IsValid() || !EHttpResponseCodes::IsOk(Response->GetResponseCode()))
	{
		return;
	}
	
	FString ResponseString = Response->GetContentAsString();

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseString);
	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		if (JsonObject->GetIntegerField("code") == 1)
		{
			const auto& Arr = JsonObject->GetArrayField("data");
			for (const TSharedPtr<FJsonValue>& JsonValue : Arr)
			{
				const auto& Obj = JsonValue->AsObject();
				const FString& Str = Obj->GetStringField("gameHistoryData");
				FWOZGameHistoryData GameHistoryData;
				FJsonObjectConverter::JsonObjectStringToUStruct(Str, &GameHistoryData);
				GameHistoryDatas.Emplace(GameHistoryData);
			}
		}
	}
}

void UMenuWidget::OnSinglePlayerDialogReply(bool bReply)
{
	UWOZGameInstance* GameInstance = Cast<UWOZGameInstance>(GetGameInstance());
	check(GameInstance);

	if (bReply)
	{
		GameInstance->bIsNewGame = false;

		FWOZSaveGameData SaveGameData;
		FJsonObjectConverter::JsonObjectStringToUStruct(PrevSaveGameDataStr, &SaveGameData);
		GameInstance->SinglePlayerSaveGameData = SaveGameData;
	}
	else
	{
		GameInstance->bIsNewGame = true;
		GameInstance->SinglePlayerSaveGameData = FWOZSaveGameData();
	}

	UGameplayStatics::OpenLevel(this, GameMapName);
}


