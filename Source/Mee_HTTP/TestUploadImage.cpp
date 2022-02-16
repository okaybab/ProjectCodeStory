// Fill out your copyright notice in the Description page of Project Settings.
#include "TestUploadImage.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include <Developer/DesktopPlatform/Public/DesktopPlatformModule.h>
#include "Runtime/Online/HTTP/Public/HttpModule.h"

// Sets default values
ATestUploadImage::ATestUploadImage()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATestUploadImage::BeginPlay()
{
	Super::BeginPlay();
	p = "INPUTURL";
	
}

// Called every frame
void ATestUploadImage::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATestUploadImage::OnRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	GEngine->AddOnScreenDebugMessage(-1, 100.0f, FColor::Orange, *Response->GetContentAsString());
	UE_LOG(LogTemp, Warning, TEXT("%s"), *Response->GetContentAsString());

	FString LinkURL;
	//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
	//Create a pointer to hold the json serialized data

	TSharedPtr<FJsonObject> JsonObject_Reader = MakeShareable(new FJsonObject());
	//Create a reader pointer to read the json data
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	//Deserialize the json data given Reader and the actual object to deserialize
	if (FJsonSerializer::Deserialize(Reader, JsonObject_Reader))
	{
		const TSharedPtr<FJsonObject> SubJsonObject = JsonObject_Reader->GetObjectField("data");
		if (SubJsonObject.IsValid())
		{
			LinkURL = SubJsonObject->GetStringField("link");
		}

		GEngine->AddOnScreenDebugMessage(-1, 100.0f, FColor::Red, LinkURL);

		p = LinkURL;
	}
}

void ATestUploadImage::UploadImage()
{
	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
	if (!ensure(DesktopPlatform != nullptr)) return;

	FString DefaultFile = TEXT("");
	FString DefaultPath = TEXT("");
	FPaths::GetPath(DefaultPath);

	TArray<FString> OpenFileNames;

	DesktopPlatform->OpenFileDialog(NULL, TEXT("OpenFile"), DefaultPath, DefaultFile, TEXT("Image Files | *.png; *.jpg; *.bmp;"),
		EFileDialogFlags::None, OpenFileNames);

	auto HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb("POST");
	HttpRequest->SetURL("https://api.imgur.com/3/image");

	HttpRequest->SetHeader("User-Agent", "X-UnrealEngine-Agent");
	HttpRequest->SetHeader("Content-Type", "application/json");
	HttpRequest->SetHeader("Authorization", "Client-ID 23a29002d89ffae");

	TSharedRef<FJsonObject> JsonObject_Writer = MakeShared<FJsonObject>();

	TArray<uint8> Binary;

	FFileHelper::LoadFileToArray(Binary, *OpenFileNames[0]);

	FString B64 = FBase64::Encode(Binary);
	JsonObject_Writer->SetStringField("image", B64);

	FString Content;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Content);

	if (FJsonSerializer::Serialize(JsonObject_Writer, Writer))
	{
		HttpRequest->SetContentAsString(Content);
	}

	HttpRequest->OnProcessRequestComplete().BindUObject(this, &ATestUploadImage::OnRequestComplete);
	HttpRequest->ProcessRequest();
}

FString ATestUploadImage::GetString()
{
	return p;
}

void ATestUploadImage::InitializeURL()
{
	p = "INPUTURL";

}

