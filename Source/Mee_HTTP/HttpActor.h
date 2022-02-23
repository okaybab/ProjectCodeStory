// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "Mee_GameInstance.h"
#include "HttpActor.generated.h"

#define BUYITEM_URL "http://192.168.110.200:3000/shop/buy"
#define EQUIPITEM_URL "http://192.168.110.200:3000/user/item/equipment"
#define MINIGAME_URL "http://192.168.110.200:3000/user/game/"
#define GET_USERDATA_URL "http://192.168.110.200:3000/user/information/"

UCLASS()
class MEE_HTTP_API AHttpActor : public AActor
{
	GENERATED_BODY()
	
public:
	FHttpModule* Http;

	UMee_GameInstance* MyGameInstance;

	AHttpActor(const class FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable)
		void HttpCall_EquipItem(FString HairCode, FString BodyCode);
	UFUNCTION(BlueprintCallable)
		void HttpCall_BuyItem(FString itemCode, FString itemType);
	UFUNCTION(BlueprintCallable)
		void HttpCall_ClearMinigame(int32 Score, int32 Money);
	UFUNCTION()
		void HttpCall_GetUserData();

	// http GET 요청 직후 호출되는 콜백함수
	void OnResponseReceived_EquipItem(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void OnResponseReceived_BuyItem(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void OnResponseReceived_ClearMinigame(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void OnResponseReceived_GetUserData(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
