// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "TimerManager.h"
#include "RankingWidget.generated.h"

#define GET_RANKINGDATA_URL "http://192.168.110.200:3000/user/game/ranking?amountOfViewData="
/**
 * 
 */
UCLASS()
class MEE_HTTP_API URankingWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	FHttpModule* Http;

	FTimerManager TimerManager;

	UFUNCTION()
		void HttpCall_GetRankData();

	// http GET 요청 직후 호출되는 콜백함수
	void OnResponseReceived_GetRankData(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* UserName_1st;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* UserName_2st;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* UserName_3st;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* UserName_4st;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* UserName_5st;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Score_1st;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Score_2st;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Score_3st;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Score_4st;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Score_5st;

	TArray<UTextBlock*> Username;
	TArray<UTextBlock*> Score;
};
