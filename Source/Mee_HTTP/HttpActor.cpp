// Fill out your copyright notice in the Description page of Project Settings.

#include "HttpActor.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AHttpActor::AHttpActor(const class FObjectInitializer& ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Http = &FHttpModule::Get();
}

void AHttpActor::HttpCall_EquipItem(FString HairCode, FString BodyCode)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &AHttpActor::OnResponseReceived_EquipItem);

	FString PostBody = FString::Printf(TEXT("{ \"username\" : \"%s\", \"itemHair\": \"%s\", \"itemBody\" : \"%s\" }"), *(MyGameInstance->UserID), *HairCode, *BodyCode);

	Request->SetURL(EQUIPITEM_URL);
	Request->SetVerb("POST");
	Request->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
	Request->SetHeader("Content-Type", TEXT("application/json"));
	Request->SetContentAsString(PostBody);
	Request->ProcessRequest();
}

void AHttpActor::HttpCall_BuyItem(FString itemCode, FString itemType)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &AHttpActor::OnResponseReceived_BuyItem);

	FString PostBody = FString::Printf(TEXT("{ \"userId\" : \"%s\", \"itemName\": \"%s\", \"itemType\" : \"%s\" }"), *(MyGameInstance->UserID), *itemCode, *itemType);

	Request->SetURL(BUYITEM_URL);
	Request->SetVerb("POST");
	Request->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
	Request->SetHeader("Content-Type", TEXT("application/json"));
	Request->SetContentAsString(PostBody);
	Request->ProcessRequest();
}

void AHttpActor::HttpCall_ClearMinigame(int32 Score, int32 Money)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &AHttpActor::OnResponseReceived_ClearMinigame);//의미 없이 콜백받기.

	FString PostBody = FString::Printf(TEXT("{ \"userId\" : \"%s\", \"score\": \"%d\", \"money\" : \"%d\" }"), *(MyGameInstance->UserID), Score, Money);

	Request->SetURL(MINIGAME_URL);
	Request->SetVerb("POST");
	Request->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
	Request->SetHeader("Content-Type", TEXT("application/json"));
	Request->SetContentAsString(PostBody);
	Request->ProcessRequest();
}

void AHttpActor::HttpCall_GetUserData()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &AHttpActor::OnResponseReceived_GetUserData);

	FString RequestURL = GET_USERDATA_URL + MyGameInstance->UserID;
	Request->SetURL(RequestURL);
	Request->SetVerb("GET");
	Request->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
	Request->SetHeader("Content-Type", TEXT("application/json"));
	Request->ProcessRequest();
}

void AHttpActor::OnResponseReceived_EquipItem(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	//Create a pointer to hold the json serialized data
	TSharedPtr<FJsonObject> JsonObject;

	//Create a reader pointer to read the json data
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

	//Deserialize the json data given Reader and the actual object to deserialize
	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		double Check;
		if (JsonObject->TryGetNumberField(TEXT("ok"), Check))
		{
			GEngine->AddOnScreenDebugMessage(1, 2.0f, FColor::Green, TEXT("200"));
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(1, 2.0f, FColor::Red, Response->GetContentAsString());
		}
	}
}

void AHttpActor::OnResponseReceived_BuyItem(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
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
			GEngine->AddOnScreenDebugMessage(1, 2.0f, FColor::Green, MessageField);
		}
		else
		{
			double money;
			if (JsonObject->TryGetNumberField(TEXT("money"), money)) {
				MyGameInstance->Money = money;
				GEngine->AddOnScreenDebugMessage(1, 2.0f, FColor::Green, FString::FromInt(MyGameInstance->Money));
			}
			else {
				GEngine->AddOnScreenDebugMessage(1, 2.0f, FColor::Green, Response->GetContentAsString());
			}
		}
	}
}

void AHttpActor::OnResponseReceived_ClearMinigame(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		//파싱할 데이터 없음.
		HttpCall_GetUserData();
		GEngine->AddOnScreenDebugMessage(-1, 100.0f, FColor::Blue, TEXT("GetUserData()"));
	}
}

void AHttpActor::OnResponseReceived_GetUserData(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
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

		MyGameInstance->EquipmentBody = EquipmentBody;
		MyGameInstance->EquipmentHair = EquipmentHair;
		MyGameInstance->Money = Money;

		const TArray<TSharedPtr<FJsonValue>>* Inventory;
		if (JsonObject->TryGetArrayField(TEXT("inventory"), Inventory))
		{
			for (int i = 0; i < Inventory->Num(); ++i)
			{
				TSharedPtr<FJsonObject> Object = (*Inventory)[i]->AsObject();
				MyGameInstance->Inventory.Add(Object->GetStringField(TEXT("itemName")));
			}
			UGameplayStatics::OpenLevel(this, "127.0.0.1");//Level 이동
		}
	}
}


// Called when the game starts or when spawned
void AHttpActor::BeginPlay()
{
	MyGameInstance = Cast<UMee_GameInstance>(GetGameInstance());
	Super::BeginPlay();
}

// Called every frame
void AHttpActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

