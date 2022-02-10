// Fill out your copyright notice in the Description page of Project Settings.


#include "TouchGameModeBase.h"
#include "TouchCharacter.h"
#include "TouchPlayerController.h"

ATouchGameModeBase::ATouchGameModeBase()
{
	DefaultPawnClass = ATouchCharacter::StaticClass();
	PlayerControllerClass = ATouchPlayerController::StaticClass();
}

void ATouchGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
}
