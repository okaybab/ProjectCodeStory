// Copyright Epic Games, Inc. All Rights Reserved.

#include "CodeStoryProtocol.h"
#include "CodeStorySocketClientLib.h"
#include "ElasticPacket.h"

#define LOCTEXT_NAMESPACE "FCodeStorySocketClientLibModule"

void FCodeStorySocketClientLibModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
    ProtocolStrategy = new CodeStoryProtocolStrategy;
    
    // Make sure choose the protocol
    
    StringPacketTemplate PacketTemplate = StringPacketTemplate();
    
    ProtocolObserver = new CodeStoryProtocolObserver<FString>(new CodeStoryMessageDecoder(PacketTemplate), new CodeStoryMessengerListener);
    ProtocolStrategy -> SetProtocolStrategy(new CodeStoryTcpProtocol("127.0.0.1", 23330, ProtocolObserver));

    CSMessenger = new CodeStoryMessenger(ProtocolStrategy, PacketTemplate);
}

void FCodeStorySocketClientLibModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
    ProtocolStrategy -> RemoveCurrentProtocol();
    
    delete ProtocolStrategy;
    ProtocolStrategy = nullptr;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCodeStorySocketClientLibModule, CodeStorySocketClientLib)
