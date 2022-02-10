// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "Mee_GameInstance.h"
#include "HttpActor.generated.h"

#define LOGIN_URL "http://192.168.110.200:3000/user/authentication/signin"
#define SIGNUP_URL "http://192.168.110.200:3000/user/authentication/signup"
#define BUYITEM_URL "http://192.168.110.200:3000/shop/buy"

UCLASS()
class MEE_HTTP_API AHttpActor : public AActor
{
	GENERATED_BODY()
	
public:
	FHttpModule* Http;

	UMee_GameInstance* MyGameInstance;

	AHttpActor(const class FObjectInitializer& ObjectInitializer);

	UFUNCTION()
		void HttpCall_Login(FString id, FString password);

	UFUNCTION()
		void HttpCall_Signup(FString id, FString username, FString password);

	UFUNCTION(BlueprintCallable)
		void HttpCall_BuyItem(FString type, FString itemCode);

	// http GET 요청 직후 호출되는 콜백함수
	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void OnResponseReceived_BuyItem(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
