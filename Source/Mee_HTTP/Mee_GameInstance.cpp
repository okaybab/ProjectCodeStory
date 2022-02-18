// Fill out your copyright notice in the Description page of Project Settings.


#include "Mee_GameInstance.h"

void UMee_GameInstance::GetUserData(FString& OutEquipmentBody, FString& OutEquipmentHair, int32& OutMoney, TArray<FString>& OutInventory)
{
	OutEquipmentBody = EquipmentBody;
	OutEquipmentHair = EquipmentHair;
	OutMoney = Money;
	OutInventory = Inventory;
}

int32 UMee_GameInstance::GetMoney()
{
	return Money;
}

void UMee_GameInstance::SetMoney(int32 _Money)
{
	Money = _Money;
}