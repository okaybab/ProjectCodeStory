// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CodeStoryProtocol.h"
#include "ElasticPacket.h"

enum CODESTORYSOCKETCLIENTLIB_API CodeStoryMessengerChannel
{
    CHANNEL_MAIN = 0,
};

enum CODESTORYSOCKETCLIENTLIB_API CodeStoryMessengerEvent
{
    SEND = 0,
    RECEIVE = 1
};

class CODESTORYSOCKETCLIENTLIB_API CodeStoryMessengerAPI
{
public:
    virtual ElasticPacket SendMessage(FString Message) = 0;
    virtual ElasticPacket SendMessageToChannel(CodeStoryMessengerChannel Channel, FString Message) = 0;
};

// =====================================================================================================================================
// =====================================================================================================================================
// ========================================================== Implementations ==========================================================
// =====================================================================================================================================
// =====================================================================================================================================

class CODESTORYSOCKETCLIENTLIB_API CodeStoryMessengerListener : public CodeStoryProtocolListener
{
public:
    void OnReceived(ElasticPacket Packet, void* RecvPayload); // 수신했을 때
    void OnTransmitted(ElasticPacket Packet); // 송신했을 때
    void OnConnected(); // 연결됬을 때

};

class CODESTORYSOCKETCLIENTLIB_API CodeStoryMessenger : public CodeStoryMessengerAPI
{
private:
    CodeStoryProtocolStrategy* ProtocolStrategy;
    StringPacketTemplate PacketTemplate;
    
public:
    CodeStoryMessenger(CodeStoryProtocolStrategy* ProtocolStrategy, StringPacketTemplate PacketTemplate);
    
    ElasticPacket SendMessage(FString Message);
    ElasticPacket SendMessageToChannel(CodeStoryMessengerChannel Channel, FString Message);
};

class CODESTORYSOCKETCLIENTLIB_API CodeStoryMessageDecoder : public CodeStoryProtocolDecoder<FString>
{
private:
    StringPacketTemplate PacketTemplate;

public:
    CodeStoryMessageDecoder(StringPacketTemplate PacketTemplate);
    
    FString Decode(uint8* Payload);
};
