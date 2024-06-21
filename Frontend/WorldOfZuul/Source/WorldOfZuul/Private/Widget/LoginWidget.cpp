// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/LoginWidget.h"
#include "HttpModule.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Gameplay/WOZGameInstance.h"
#include "Interfaces/IHttpResponse.h"
#include "Kismet/GameplayStatics.h"
#include "Widget/DialogWidget.h"
#include "Widget/RegisterWidget.h"

bool ULoginWidget::Initialize()
{
	if (!Super::Initialize()) return false;
	if (!EditableTextBox_Password) return false;

	SetIsFocusable(true);
	
	EditableTextBox_Password->SetIsPassword(true);
	
	return true;
}

void ULoginWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Button_Login->OnReleased.AddDynamic(this, &ULoginWidget::Login);
	Button_Register->OnReleased.AddDynamic(this, &ULoginWidget::Register);
}

void ULoginWidget::Login()
{
	if (EditableTextBox_Username->GetText().IsEmpty() || EditableTextBox_Password->GetText().IsEmpty())
	{
		UDialogWidget::DisplayDialog(DialogWidgetClass, this, EmptyInputMsg, false);
		return;
	}
	
	SetIsEnabled(false);
	
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();

	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetStringField("username", EditableTextBox_Username->GetText().ToString());
	JsonObject->SetStringField("password", EditableTextBox_Password->GetText().ToString());

	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	Request->OnProcessRequestComplete().BindUObject(this, &ULoginWidget::OnLoginResponseReceived);
	Request->SetHeader("Content-Type", "application/json;charset=UTF-8");
	Request->SetContentAsString(RequestBody);
	Request->SetURL(LoginURL);
	Request->SetVerb("POST");
	
	Request->ProcessRequest();
}

void ULoginWidget::Register()
{
	check(RegisterWidgetClass);

	if (URegisterWidget* RegisterWidget = CreateWidget<URegisterWidget>(this, RegisterWidgetClass))
	{
		RegisterWidget->AddToViewport(1);
		RemoveFromParent();
	}
}

void ULoginWidget::OnLoginResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bRequestSuccessful)
{
	SetIsEnabled(true);
	
	if (!bRequestSuccessful || !Response.IsValid() || !EHttpResponseCodes::IsOk(Response->GetResponseCode()))
	{
		UDialogWidget::DisplayDialog(DialogWidgetClass, this, RequestFailedMsg, false);
		return;
	}
	
	FString ResponseString = Response->GetContentAsString();

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseString);
	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		if (JsonObject->GetIntegerField("code") == 1)
		{
			UWOZGameInstance* GameInstance = UWOZGameInstance::Get(this);
			check(GameInstance);
			GameInstance->Username = EditableTextBox_Username->GetText();

			UE_LOG(LogTemp, Log, TEXT("用户已登录。用户名：%s。"), *EditableTextBox_Username->GetText().ToString());

			//测试，会删
			UGameplayStatics::OpenLevel(this, FName("GameMap"));
		}
		else
		{
			UDialogWidget::DisplayDialog(DialogWidgetClass, this, LoginFailedMsg, false);
		}
	}
}
