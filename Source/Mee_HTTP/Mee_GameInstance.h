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
	FString UserID;

	int32 Money;
	FString EquipmentBody;
	FString EquipmentHair;
	TArray<FString> Inventory;


	UFUNCTION(BlueprintCallable)
		void GetUserData(FString& OutEquipmentBody, FString& OutEquipmentHair, int32& OutMoney, TArray<FString>& OutInventory);

	UFUNCTION(BlueprintCallable)
		int32 GetMoney();
	UFUNCTION(BlueprintCallable)
		void SetMoney(int32 _Money);
	
};
