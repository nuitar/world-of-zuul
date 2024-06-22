// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/MenuWidget.h"
#include "HttpModule.h"
#include "JsonObjectConverter.h"
#include "Components/Button.h"
#include "Gameplay/WOZGameInstance.h"
#include "Interfaces/IHttpResponse.h"
#include "Kismet/GameplayStatics.h"
#include "Widget/DialogWidget.h"

void UMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	Load();

	Button_SinglePlayer->OnReleased.AddDynamic(this, &UMenuWidget::SinglePlayer);
}

void UMenuWidget::SinglePlayer()
{
	if (!PrevSaveGameDataStr.IsEmpty())
	{
		check(DialogWidgetClass);

		UDialogWidget* DialogWidget = UDialogWidget::DisplayDialog(DialogWidgetClass, this, FText::FromString(TEXT("你有一个正在进行的游戏，是否继续")), true);
		DialogWidget->OnReply.BindUObject(this, &UMenuWidget::OnSinglePlayerDialogReply);
	}
	else
	{
		UGameplayStatics::OpenLevel(this, GameMapName);
	}
}

void UMenuWidget::SaveClear()
{
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();

	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetStringField("savegamedata", FString());

	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	Request->SetHeader("Content-Type", "application/json;charset=UTF-8");
	Request->SetContentAsString(RequestBody);
	Request->SetURL(SaveURL);
	Request->SetVerb("POST");
	
	Request->ProcessRequest();
}

void UMenuWidget::Load()
{
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	
	Request->OnProcessRequestComplete().BindUObject(this, &UMenuWidget::OnLoadResponseReceived);
	Request->SetURL(LoadURL);
	Request->SetVerb("POST");
	
	Request->ProcessRequest();
}

void UMenuWidget::OnSinglePlayerDialogReply(bool bReply)
{
	if (bReply)
	{
		UWOZGameInstance* GameInstance = Cast<UWOZGameInstance>(GetGameInstance());
		check(GameInstance);

		GameInstance->bIsNewGame = false;

		FWOZSaveGameData SaveGameData;
		FJsonObjectConverter::JsonObjectStringToUStruct(PrevSaveGameDataStr, &SaveGameData);
		GameInstance->SinglePlayerSaveGameData = SaveGameData;
	}

	UGameplayStatics::OpenLevel(this, GameMapName);
}

void UMenuWidget::OnLoadResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bRequestSuccessful)
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
			const FString& Str = JsonObject->GetStringField("savegamedata");
			PrevSaveGameDataStr = Str;
		}
	}
}


