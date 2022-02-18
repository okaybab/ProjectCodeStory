// Fill out your copyright notice in the Description page of Project Settings.


#include "SignupWidget.h"
#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/UMGStyle.h" 
#include "Runtime/UMG/Public/Slate/SObjectWidget.h" 
#include "Runtime/UMG/Public/IUMGModule.h" 
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "UMG/Public/Components/Button.h"

void USignupWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Http = &FHttpModule::Get();

	Button_Signup->OnClicked.AddDynamic(this, &USignupWidget::OnClick_Signup);
	Button_Cancle->OnClicked.AddDynamic(this, &USignupWidget::OnClick_Cancle);
}

void USignupWidget::HttpCall_Signup(FString id, FString username, FString password)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &USignupWidget::OnResponseReceived_Signup);

	FString PostBody = FString::Printf(TEXT("{ \"id\": \"%s\", \"username\" : \"%s\", \"password\" : \"%s\" }"), *id, *username, *password);

	Request->SetURL(SIGNUP_URL);
	Request->SetVerb("POST");
	Request->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
	Request->SetHeader("Content-Type", TEXT("application/json"));
	Request->SetContentAsString(PostBody);
	Request->ProcessRequest();
}

void USignupWidget::OnResponseReceived_Signup(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	//Create a pointer to hold the json serialized data
	TSharedPtr<FJsonObject> JsonObject;

	//Create a reader pointer to read the json data
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

	//Deserialize the json data given Reader and the actual object to deserialize
	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		GEngine->AddOnScreenDebugMessage(-1, 62.0f, FColor::Green, Response->GetContentAsString());
		const TArray<TSharedPtr<FJsonValue>>* Value;
		if (JsonObject->TryGetArrayField(TEXT("message"), Value))
		{
			FString MessageField = (*Value)[0]->AsString();
			GEngine->AddOnScreenDebugMessage(-1, 62.0f, FColor::Green, MessageField);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 62.0f, FColor::Green, TEXT("회원가입 성공!"));
		}

	}
}

void USignupWidget::OnClick_Signup()
{
	HttpCall_Signup(EditableText_id->GetText().ToString(), EditableText_username->GetText().ToString(), EditableText_password->GetText().ToString());
}

void USignupWidget::OnClick_Cancle()
{
	this->SetVisibility(ESlateVisibility::Collapsed);
	EditableText_id->SetText(FText::FromString(TEXT("")));
	EditableText_username->SetText(FText::FromString(TEXT("")));
	EditableText_password->SetText(FText::FromString(TEXT("")));
}
