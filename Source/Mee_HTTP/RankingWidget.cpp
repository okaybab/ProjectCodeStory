// Fill out your copyright notice in the Description page of Project Settings.


#include "RankingWidget.h"
#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/UMGStyle.h" 
#include "Runtime/UMG/Public/Slate/SObjectWidget.h" 
#include "Runtime/UMG/Public/IUMGModule.h" 
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"

//Like 'BeginPlay'
void URankingWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Http = &FHttpModule::Get();

	Username.Add(UserName_1st);
	Username.Add(UserName_2st);
	Username.Add(UserName_3st);
	Username.Add(UserName_4st);
	Username.Add(UserName_5st);

	Score.Add(Score_1st);
	Score.Add(Score_2st);
	Score.Add(Score_3st);
	Score.Add(Score_4st);
	Score.Add(Score_5st);

	HttpCall_GetRankData();
	FTimerHandle RankingDataTimerHandle;
	TimerManager.SetTimer(RankingDataTimerHandle, this, &URankingWidget::HttpCall_GetRankData, 30.0f, true);
}

void URankingWidget::HttpCall_GetRankData()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &URankingWidget::OnResponseReceived_GetRankData);

	FString amountOfViewData = "5";
	FString RequestURL = GET_RANKINGDATA_URL + amountOfViewData;
	Request->SetURL(RequestURL);
	Request->SetVerb("GET");
	Request->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
	Request->SetHeader("Content-Type", TEXT("application/json"));
	Request->ProcessRequest();
}

void URankingWidget::OnResponseReceived_GetRankData(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	if (FJsonSerializer::Deserialize(Reader, JsonObject))//랭킹 데이터를 가져오는데 성공
	{
		const TArray<TSharedPtr<FJsonValue>>* amountOfViewData;
		if (JsonObject->TryGetArrayField(TEXT("rankings"), amountOfViewData))
		{
			for (int i = 0; i < amountOfViewData->Num(); ++i)
			{
				TSharedPtr<FJsonObject> Object = (*amountOfViewData)[i]->AsObject();
				FString Value = Object->GetStringField(TEXT("userId"));
				Username[i]->SetText(FText::FromString(Value));
				int32 Value2 = Object->GetNumberField(TEXT("score"));
				FString Value3 = FString::FromInt(Value2);
				Score[i]->SetText(FText::FromString(Value3));
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 100.0f, FColor::Red, Response->GetContentAsString());
		}
	}
}
