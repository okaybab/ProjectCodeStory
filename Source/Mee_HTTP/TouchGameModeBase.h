// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "CoreMinimal.h"
#include "Mee_HTTP.h"
#include "GameFramework/GameModeBase.h"
#include "TouchGameModeBase.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class MEE_HTTP_API ATouchGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	ATouchGameModeBase();

	virtual void PostLogin(APlayerController* NewPlayer) override;
};
