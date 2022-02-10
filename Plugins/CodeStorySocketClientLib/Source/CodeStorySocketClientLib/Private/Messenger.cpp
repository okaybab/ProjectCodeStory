// Fill out your copyright notice in the Description page of Project Settings.

#include "Messenger.h"

// =====================================================================================================================================
// =====================================================================================================================================
// ====================================================== CodeStory Listener ===========================================================
// =====================================================================================================================================
// =====================================================================================================================================
void CodeStoryMessengerListener::OnReceived(ElasticPacket Packet, void* RecvPayload)
{
    FString* RecvData = static_cast<FString*>(RecvPayload);
    
    GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Black, FString::Printf(TEXT("Called OnReceived")));
    GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Black, FString::Printf(TEXT("Packet.ChannelId : %d"), Packet.ChannelId));
    GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Black, FString::Printf(TEXT("Packet.EventId : %d"), Packet.EventId));
    GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Black, FString::Printf(TEXT("Packet.AmountOfData : %d"), Packet.AmountOfData));
    GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Black, FString::Printf(TEXT("Packet.TypeId : %d"), Packet.TypeId));
    GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Black, *RecvData);


    // TODO: 메세지를 수신했을 때
}

void CodeStoryMessengerListener::OnTransmitted(ElasticPacket Packet)
{
    GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Black, FString::Printf(TEXT("Called OnTransmitted")));
}

void CodeStoryMessengerListener::OnConnected()
{
    // TODO: 서버와 연결됬을 때
}


// =====================================================================================================================================
// =====================================================================================================================================
// ====================================================== CodeStory Chat Module ========================================================
// =====================================================================================================================================
// =====================================================================================================================================

CodeStoryMessenger::CodeStoryMessenger(CodeStoryProtocolStrategy* _ProtocolStrategy, StringPacketTemplate _StringPacketTemplate)
{
    ProtocolStrategy = _ProtocolStrategy;
    PacketTemplate = _StringPacketTemplate;
}

ElasticPacket CodeStoryMessenger::CodeStoryMessenger::SendMessage(FString Message)
{
    return SendMessageToChannel(CodeStoryMessengerChannel::CHANNEL_MAIN, Message);
}

ElasticPacket CodeStoryMessenger::SendMessageToChannel(CodeStoryMessengerChannel _Channel, FString Message)
{
    uint16 Channel = (int16) _Channel;
    uint16 Event =  (int16) CodeStoryMessengerEvent::SEND;
    uint32 PayloadDataType = (int32) ElasticPacketType::STRING;
    uint32 Size = Message.Len() + 1;
    int32 NewSize;
    uint8* PayloadBinary = DataSerializer::StringToBinary(Message, NewSize);
    
    ElasticPacket Packet;
    Packet.ChannelId = Channel;
    Packet.EventId = Event;
    Packet.AmountOfData = Size;
    Packet.TypeId = PayloadDataType;
    Packet.Payload = PayloadBinary;
    
    return ProtocolStrategy -> GetCurrentProtocol() -> Send(Packet);
}

// =====================================================================================================================================
// =====================================================================================================================================
// ====================================================== Codestory Chat Receive Data Decoder ==========================================
// =====================================================================================================================================
// =====================================================================================================================================

CodeStoryMessageDecoder::CodeStoryMessageDecoder(StringPacketTemplate _StringPacketTemplate)
{
    PacketTemplate = _StringPacketTemplate;
}


FString CodeStoryMessageDecoder::Decode(uint8* Payload)
{
    FString Data = PacketTemplate.Deserialize(Payload);
    UE_LOG(LogTemp, Log, TEXT("%s"), *Data);
    
    return Data;
}


