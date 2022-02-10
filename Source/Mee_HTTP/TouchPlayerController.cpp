// Fill out your copyright notice in the Description page of Project Settings.


#include "TouchPlayerController.h"

void ATouchPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
}

void ATouchPlayerController::OnPossess(APawn* NewPawn)
{
	Super::OnPossess(NewPawn);
	
}

void ATouchPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
}
