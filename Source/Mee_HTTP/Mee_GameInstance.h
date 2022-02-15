// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Mee_GameInstance.generated.h"

/**
 * 
 */
UCLASS()
class MEE_HTTP_API UMee_GameInstance : public UGameInstance
{
	GENERATED_BODY()

private:
	FString Session;

public:
	FString ItemType;
	FString ItemName;
	int32 Money;

	FString GetSession();

	UFUNCTION(BlueprintCallable)
		void GetUserData(FString& OutItemType, FString& OutItemName);

	UFUNCTION(BlueprintCallable)
		int32 GetMoney();

	UFUNCTION(BlueprintCallable)
		void SetMoney(int32 _Money);
	
};
