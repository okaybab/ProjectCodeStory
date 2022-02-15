// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/IHttpRequest.h"
#include "TestUploadImage.generated.h"

UCLASS()
class MEE_HTTP_API ATestUploadImage : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATestUploadImage();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	FString p;

	void OnRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);

public:
	UFUNCTION(BlueprintCallable)
	void UploadImage();

	UFUNCTION(BlueprintCallable)
		FString GetString();

	UFUNCTION(BlueprintCallable)
	void InitializeURL();
};
