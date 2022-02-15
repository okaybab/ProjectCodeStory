// Fill out your copyright notice in the Description page of Project Settings.


#include "Mee_GameInstance.h"

void UMee_GameInstance::GetUserData(FString& OutItemType, FString& OutItemName)
{
	OutItemType = ItemType;
	OutItemName = ItemName;
}

FString UMee_GameInstance::GetSession()
{
	return Session;
}

int32 UMee_GameInstance::GetMoney()
{
	return Money;
}

void UMee_GameInstance::SetMoney(int32 _Money)
{
	Money = _Money;
}