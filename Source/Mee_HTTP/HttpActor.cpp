// Fill out your copyright notice in the Description page of Project Settings.

#include "HttpActor.h"

// Sets default values
AHttpActor::AHttpActor(const class FObjectInitializer& ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Http = &FHttpModule::Get();
}


void AHttpActor::HttpCall_Login(FString id, FString password)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &AHttpActor::OnResponseReceived);

	FString PostBody = FString::Printf(TEXT("{ \"id\": \"%s\", \"password\" : \"%s\" }"), *id, *password);

	Request->SetURL(LOGIN_URL);
	Request->SetVerb("POST");
	Request->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
	Request->SetHeader("Content-Type", TEXT("application/json"));
	Request->SetContentAsString(PostBody);
	Request->ProcessRequest();
}

void AHttpActor::HttpCall_Signup(FString id, FString username, FString password)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &AHttpActor::OnResponseReceived);

	FString PostBody = FString::Printf(TEXT("{ \"id\": \"%s\", \"username\" : \"%s\", \"password\" : \"%s\" }"), *id, *username, *password);

	Request->SetURL(SIGNUP_URL);
	Request->SetVerb("POST");
	Request->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
	Request->SetHeader("Content-Type", TEXT("application/json"));
	Request->SetContentAsString(PostBody);
	Request->ProcessRequest();
}

void AHttpActor::HttpCall_BuyItem(FString type, FString itemCode)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &AHttpActor::OnResponseReceived_BuyItem);

	//FString PostBody = FString::Printf(TEXT("{ \"session\" : \"%s\", \"type\": \"%s\", \"itemCode\" : \"%s\" }"), *(MyGameInstance->Session), *type, *itemCode);
	FString PostBody = FString::Printf(TEXT("{ \"userId\" : \"%s\", \"itemName\": \"%s\", \"itemType\" : \"%s\" }"), TEXT("skaeodud0507"), *itemCode, *type);

	Request->SetURL(BUYITEM_URL);
	Request->SetVerb("POST");
	Request->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
	Request->SetHeader("Content-Type", TEXT("application/json"));
	Request->SetContentAsString(PostBody);
	Request->ProcessRequest();
}

void AHttpActor::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
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
			GEngine->AddOnScreenDebugMessage(1, 2.0f, FColor::Green, Response->GetContentAsString());
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
		GEngine->AddOnScreenDebugMessage(-1, 62.0f, FColor::Red, TEXT("Successed"));
		const TArray<TSharedPtr<FJsonValue>>* Value;
		if (JsonObject->TryGetArrayField(TEXT("message"), Value))
		{
			FString MessageField = (*Value)[0]->AsString();
			GEngine->AddOnScreenDebugMessage(-1, 62.0f, FColor::Yellow, MessageField);	
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

