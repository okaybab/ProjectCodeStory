// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Messenger.h"
#include "Modules/ModuleManager.h"

template<typename T>
class CodeStoryProtocolObserver;
class CodeStoryProtocolStrategy;
class CodeStoryMessengerAPI;

static CodeStoryProtocolStrategy* ProtocolStrategy;
static CodeStoryProtocolObserver<FString> *ProtocolObserver;
static CodeStoryMessengerAPI* CSMessenger;

class FCodeStorySocketClientLibModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
