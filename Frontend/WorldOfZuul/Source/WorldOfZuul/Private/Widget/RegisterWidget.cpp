// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/RegisterWidget.h"
#include "HttpModule.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Interfaces/IHttpResponse.h"
#include "Widget/DialogWidget.h"
#include "Widget/LoginWidget.h"

bool URegisterWidget::Initialize()
{
	if (!Super::Initialize()) return false;
	if (!EditableTextBox_Password || !EditableTextBox_PasswordConfirm) return false;
	
	SetIsFocusable(true);
	
	EditableTextBox_Password->SetIsPassword(true);
	EditableTextBox_PasswordConfirm->SetIsPassword(true);
	
	return true;
}

void URegisterWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Button_Register->OnReleased.AddDynamic(this, &URegisterWidget::Register);
	Button_Login->OnReleased.AddDynamic(this, &URegisterWidget::Login);
}

void URegisterWidget::Register()
{
	if (EditableTextBox_Username->GetText().IsEmpty() || EditableTextBox_Password->GetText().IsEmpty())
	{
		UDialogWidget::DisplayDialog(DialogWidgetClass, this, EmptyInputMsg, false);
		return;
	}
	
	if (!EditableTextBox_Password->GetText().EqualTo(EditableTextBox_PasswordConfirm->GetText()))
	{
		UDialogWidget::DisplayDialog(DialogWidgetClass, this, PasswordConfirmErrorMsg, false);
		return;
	}
	
	SetIsEnabled(true);
	
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();

	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetStringField("username", EditableTextBox_Username->GetText().ToString());
	JsonObject->SetStringField("password", EditableTextBox_Password->GetText().ToString());

	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	Request->OnProcessRequestComplete().BindUObject(this, &URegisterWidget::OnRegisterResponseReceived);
	Request->SetHeader("Content-Type", "application/json");
	Request->SetContentAsString(RequestBody);
	Request->SetURL(LoginURL);
	Request->SetVerb("POST");
	
	Request->ProcessRequest();
}

void URegisterWidget::Login()
{
	check(LoginWidgetClass);

	if (ULoginWidget* LoginWidget = CreateWidget<ULoginWidget>(this, LoginWidgetClass))
	{
		LoginWidget->AddToViewport(1);
		RemoveFromParent();
	}
}

void URegisterWidget::OnRegisterResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bRequestSuccessful)
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
		if (JsonObject->GetBoolField("success"))
		{
			UDialogWidget::DisplayDialog(DialogWidgetClass, this, RegisterSuccessfulMsg, false);

			UE_LOG(LogTemp, Log, TEXT("成功注册用户。用户名：%s。"), *EditableTextBox_Username->GetText().ToString());
		}
		else
		{
			UDialogWidget::DisplayDialog(DialogWidgetClass, this, RegisterFailedMsg, false);
		}
	}
}
