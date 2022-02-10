// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "CoreMinimal.h"
#include "Mee_HTTP.h"
#include "GameFramework/PlayerController.h"
#include "TouchPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class MEE_HTTP_API ATouchPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void PostInitializeComponents() override;
	virtual void OnPossess(APawn* NewPawn) override;

protected:
	virtual void BeginPlay() override;
	
};
