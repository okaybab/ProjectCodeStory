// Fill out your copyright notice in the Description page of Project Settings.


#include "LoginWidget.h"
#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/UMGStyle.h" 
#include "Runtime/UMG/Public/Slate/SObjectWidget.h" 
#include "Runtime/UMG/Public/IUMGModule.h" 
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "UMG/Public/Components/Button.h"

void ULoginWidget::HttpCall_Login(FString id, FString password)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &ULoginWidget::OnResponseReceived);

	FString PostBody = FString::Printf(TEXT("{ \"id\": \"%s\", \"password\" : \"%s\" }"), *id, *password);

	Request->SetURL(LOGIN_URL);
	Request->SetVerb("POST");
	Request->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
	Request->SetHeader("Content-Type", TEXT("application/json"));
	Request->SetContentAsString(PostBody);
	Request->ProcessRequest();
}

void ULoginWidget::HttpCall_Signup(FString id, FString username, FString password)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &ULoginWidget::OnResponseReceived);

	FString PostBody = FString::Printf(TEXT("{ \"id\": \"%s\", \"username\" : \"%s\", \"password\" : \"%s\" }"), *id, *username, *password);

	Request->SetURL(SIGNUP_URL);
	Request->SetVerb("POST");
	Request->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
	Request->SetHeader("Content-Type", TEXT("application/json"));
	Request->SetContentAsString(PostBody);
	Request->ProcessRequest();
}

void ULoginWidget::HttpCall_GetUserData()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &ULoginWidget::OnResponseReceived_GetUserData);

	FString RequestURL = GET_USERDATA_URL + GameInstance->UserID;
	Request->SetURL(RequestURL);
	Request->SetVerb("GET");
	Request->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
	Request->SetHeader("Content-Type", TEXT("application/json"));
	Request->ProcessRequest();
}

void ULoginWidget::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	//Create a pointer to hold the json serialized data
	TSharedPtr<FJsonObject> JsonObject;

	//Create a reader pointer to read the json data
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

	//Deserialize the json data given Reader and the actual object to deserialize
	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		const TArray<TSharedPtr<FJsonValue>>* Value;
		if (JsonObject->TryGetArrayField(TEXT("message"), Value))
		{
			FString MessageField = (*Value)[0]->AsString();
			GEngine->AddOnScreenDebugMessage(-1, 62.0f, FColor::Green, MessageField);
		}
		else
		{
			FString Session;
			FString UserID;
			if (JsonObject->TryGetStringField(TEXT("session"), Session))//로그인 성공
			{
				GameInstance->Session = Session;
				GEngine->AddOnScreenDebugMessage(-1, 62.0f, FColor::Green, GameInstance->Session);
				if (JsonObject->TryGetStringField(TEXT("userId"), UserID))//세션 저장 후, 유저 아이디 저장
				{
					GameInstance->UserID = UserID;
					GEngine->AddOnScreenDebugMessage(-1, 62.0f, FColor::Green, GameInstance->UserID);

					HttpCall_GetUserData(); //로그인 성공 시 유저 데이터를 가져온다.
				}
				else//로그인 에러
				{
					GEngine->AddOnScreenDebugMessage(-1, 62.0f, FColor::Green, Response->GetContentAsString());
				}
			}
			else //로그인 에러
			{
				GEngine->AddOnScreenDebugMessage(-1, 62.0f, FColor::Green, Response->GetContentAsString());
			}
		}

	}
}

void ULoginWidget::OnResponseReceived_GetUserData(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	//Create a pointer to hold the json serialized data
	TSharedPtr<FJsonObject> JsonObject;

	//Create a reader pointer to read the json data
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

	//Deserialize the json data given Reader and the actual object to deserialize
	if (FJsonSerializer::Deserialize(Reader, JsonObject))//유저 데이터를 가져오는데 성공
	{
		FString EquipmentBody = JsonObject->GetStringField(TEXT("equipmentBody"));
		FString EquipmentHair = JsonObject->GetStringField(TEXT("equipmentHair"));
		int32 Money = JsonObject->GetIntegerField(TEXT("money"));

		GameInstance->EquipmentBody = EquipmentBody;
		GameInstance->EquipmentHair = EquipmentHair;
		GameInstance->Money = Money;

		const TArray<TSharedPtr<FJsonValue>>* Inventory;
		if (JsonObject->TryGetArrayField(TEXT("inventory"), Inventory))
		{
			for (int i = 0; i < Inventory->Num(); ++i)
			{
				TSharedPtr<FJsonObject> Object = (*Inventory)[i]->AsObject();
				GameInstance->Inventory.Add(Object->GetStringField(TEXT("itemName")));
			}
			UGameplayStatics::OpenLevel(this, "P_Level");//Level 이동
		}
	}
}

void ULoginWidget::OnResponseReceived_GetEquipment(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	//Create a pointer to hold the json serialized data
	TSharedPtr<FJsonObject> JsonObject;

	//Create a reader pointer to read the json data
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

	//Deserialize the json data given Reader and the actual object to deserialize
	if (FJsonSerializer::Deserialize(Reader, JsonObject))//유저 데이터를 가져오는데 성공
	{
		const TArray<TSharedPtr<FJsonValue>>* Value;
		if (JsonObject->TryGetArrayField(TEXT("userItems"), Value))
		{
			TSharedPtr<FJsonObject> Object = (*Value)[0]->AsObject();
			//GameInstance->ItemType = Object->GetStringField(TEXT("itemType"));
			//GEngine->AddOnScreenDebugMessage(-1, 60.0f, FColor::Blue, GameInstance->ItemType);

			//GameInstance->ItemName = Object->GetStringField(TEXT("itemName"));
			//GEngine->AddOnScreenDebugMessage(-1, 60.0f, FColor::Blue, GameInstance->ItemName);

			//UGameplayStatics::OpenLevel(this, "P_Level");//Level 이동
		}
	}
}

//Like 'BeginPlay'
void ULoginWidget::NativeConstruct()
{
	Super::NativeConstruct();

	GameInstance = Cast<UMee_GameInstance>(GetGameInstance());
	Http = &FHttpModule::Get();

	Button_Signup->OnClicked.AddDynamic(this, &ULoginWidget::OnClick_Signup);
	Button_Login->OnClicked.AddDynamic(this, &ULoginWidget::OnClick_Login);

}

void ULoginWidget::OnClick_Signup()
{
	HttpCall_Signup(EditableText_id->GetText().ToString(), EditableText_username->GetText().ToString(), EditableText_password->GetText().ToString());
}

void ULoginWidget::OnClick_Login()
{
	HttpCall_Login(EditableText_id->GetText().ToString(), EditableText_password->GetText().ToString());
}
