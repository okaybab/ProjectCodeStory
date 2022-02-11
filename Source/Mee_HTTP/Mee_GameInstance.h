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

public:

	FString Session;
	FString ItemType;
	FString ItemName;

	UFUNCTION(BlueprintCallable)
	void GetUserData(FString& OutItemType, FString& OutItemName);
	
};
