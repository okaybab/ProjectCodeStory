// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LoginWidget.h"
#include "EntryPlayerController.h"
#include "Mee_HTTPGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class MEE_HTTP_API AMee_HTTPGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMee_HTTPGameModeBase();

protected:
    virtual void BeginPlay() override;

private:
	TSubclassOf<ULoginWidget> LoginWidgetClass;
	class ULoginWidget* LoginWidget;
	
};
