// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/IHttpRequest.h"
#include "Engine/Texture2DDynamic.h"
#include "TestUploadImage.generated.h"

UCLASS()
class MEE_HTTP_API ATestUploadImage : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATestUploadImage();

	TArray<UTexture2DDynamic*>* pTexture_2D;
	TArray<FString>* URL;
	int32 Count;
	bool Check;

	UFUNCTION(BlueprintCallable)
		void GetTexture(TArray<UTexture2DDynamic*>& Texture);
	UFUNCTION(BlueprintCallable)
		void GetURL(TArray<FString>& URL_String);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;



	void OnRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);

public:
	UFUNCTION(BlueprintCallable)
	bool UploadImage();

	UFUNCTION(BlueprintCallable)
	bool CheckAllDownloaded();

private:
	UFUNCTION(BlueprintCallable)
	void OnDownloadImageSuccess(UTexture2DDynamic* Texture);
	void UploadingSuccess(FString LinkURL);
};
