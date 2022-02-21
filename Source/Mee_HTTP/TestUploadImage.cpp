// Fill out your copyright notice in the Description page of Project Settings.
#include "TestUploadImage.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include <Developer/DesktopPlatform/Public/DesktopPlatformModule.h>
#include "Runtime/Online/HTTP/Public/HttpModule.h"
#include "Blueprint/AsyncTaskDownloadImage.h"



// Sets default values
ATestUploadImage::ATestUploadImage()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ATestUploadImage::GetTexture(TArray<UTexture2DDynamic*> &Texture)
{
	Texture = *pTexture_2D;
	delete pTexture_2D;
}

void ATestUploadImage::GetURL(TArray<FString>& URL_String)
{
	URL_String = URL;
}

// Called when the game starts or when spawned
void ATestUploadImage::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ATestUploadImage::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATestUploadImage::OnRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	//GEngine->AddOnScreenDebugMessage(-1, 100.0f, FColor::Orange, *Response->GetContentAsString());
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
			UploadingSuccess(LinkURL);
			
		}

		//GEngine->AddOnScreenDebugMessage(-1, 100.0f, FColor::Red, LinkURL);

	}

	//UAsyncTaskDownloadImage* TaskDownloadImage = UAsyncTaskDownloadImage::DownloadImage(LinkURL);
	//if (TaskDownloadImage != nullptr)
	//{
	//	TaskDownloadImage->OnSuccess.AddDynamic(this, &ATestUploadImage::OnDownloadImageSuccess);
	//}


}

void ATestUploadImage::UploadImage()
{
	Check = false;
	pTexture_2D = new TArray<UTexture2DDynamic*>;
	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
	if (!ensure(DesktopPlatform != nullptr)) return;

	FString DefaultFile = TEXT("");
	FString DefaultPath = TEXT("");
	FPaths::GetPath(DefaultPath);

	TArray<FString> OpenFileNames;

	DesktopPlatform->OpenFileDialog(NULL, TEXT("OpenFile"), DefaultPath, DefaultFile, TEXT("Image Files | *.png; *.jpg; *.bmp;"),
		EFileDialogFlags::Multiple, OpenFileNames);

	Count = OpenFileNames.Num();
	//GEngine->AddOnScreenDebugMessage(-1, 100.0f, FColor::Red, FString::FromInt(Count));
	for (int i = 0; i < Count; ++i)
	{ 
		auto HttpRequest = FHttpModule::Get().CreateRequest();

		HttpRequest->OnProcessRequestComplete().BindUObject(this, &ATestUploadImage::OnRequestComplete);

		HttpRequest->SetVerb("POST");
		HttpRequest->SetURL("https://api.imgur.com/3/image");
		HttpRequest->SetHeader("User-Agent", "X-UnrealEngine-Agent");
		HttpRequest->SetHeader("Content-Type", "application/json");
		HttpRequest->SetHeader("Authorization", "Client-ID 47beae464ad1a25");

		TSharedRef<FJsonObject> JsonObject_Writer = MakeShared<FJsonObject>();

		TArray<uint8> Binary;

		FFileHelper::LoadFileToArray(Binary, *OpenFileNames[i]);	

		FString B64 = FBase64::Encode(Binary);
		JsonObject_Writer->SetStringField("image", B64);

		FString Content;
		TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Content);
		if (FJsonSerializer::Serialize(JsonObject_Writer, Writer))
		{
			HttpRequest->SetContentAsString(Content);
		}

		HttpRequest->ProcessRequest();
	}
}

bool ATestUploadImage::CheckAllDownloaded()
{
	return Check;
}

void ATestUploadImage::OnDownloadImageSuccess(UTexture2DDynamic* Texture)
{
	pTexture_2D->Add(Texture);

	if (pTexture_2D->Num() == Count)
	{
		Count = 0;
		Check = true;
	}
}

void ATestUploadImage::UploadingSuccess(FString LinkURL)
{
	URL.Add(LinkURL);
	if (URL.Num() == Count)
	{
		Count = 0;
		Check = true;
	}
}

