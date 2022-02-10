// Copyright Epic Games, Inc. All Rights Reserved.

#include "CodeStorySocketClientLibBPLibrary.h"
#include "CodeStorySocketClientLib.h"

UCodeStorySocketClientLibBPLibrary::UCodeStorySocketClientLibBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

void UCodeStorySocketClientLibBPLibrary::Recv()
{
    ElasticPacket Packet = ProtocolStrategy -> GetCurrentProtocol() -> Recv(1024);

    // 수신한 데이터 다시 디코딩
    CodeStoryProtocolDecoder<FString>* Decoder = ProtocolObserver -> GetDecoder();
    FString Decoded = Decoder -> Decode(Packet.Payload);
    void* Wrapped = &Decoded;

    ProtocolObserver -> SendNotificationReceived(Packet, Wrapped);
    delete Packet.Payload;
}

bool UCodeStorySocketClientLibBPLibrary::ConnectToServer(FString Addr, int32 Port)
{
    bool bCreated = ProtocolStrategy->Start(Addr, Port);

    return bCreated;
}

//void UCodeStorySocketClientLibBPLibrary::Send(FString Message)
//{
//    uint16 Channel = (int16) _Channel;
//    uint16 Event =  (int16) CodeStoryMessengerEvent::SEND;
//    uint32 PayloadDataType = (int32) ElasticPacketType::STRING;
//    uint32 Size = Message.Len() + 1;
//    int32 NewSize;
//    uint8* PayloadBinary = DataSerializer::StringToBinary(Message, NewSize);
//    
//    ElasticPacket Packet;
//    Packet.ChannelId = Channel;
//    Packet.EventId = Event;
//    Packet.AmountOfData = Size;
//    Packet.TypeId = PayloadDataType;
//    Packet.Payload = PayloadBinary;
//    
//    ElasticPacket TransmittedPacket = ProtocolStrategy -> GetCurrentProtocol() -> Send(Packet);
//    
//    ProtocolObserver -> SendNotificationTransimitted(TransmittedPacket);
//}

void UCodeStorySocketClientLibBPLibrary::SendMessageToChannel(int32 _Channel, FString Message)
{
    uint16 Channel = (uint16) _Channel;
    uint16 Event =  (int16) CodeStoryMessengerEvent::SEND;
    uint32 PayloadDataType = (int32) ElasticPacketType::STRING;
    uint32 Size = Message.Len() + 1;
    int32 NewSize;
    uint8* PayloadBinary = DataSerializer::StringToBinary(Message, NewSize);
    
    //GEngine->AddOnScreenDebugMessage(-1, 62.0f, FColor::Green, Message);
    

    ElasticPacket Packet;
    Packet.ChannelId = Channel;
    Packet.EventId = Event;
    Packet.AmountOfData = Size;
    Packet.TypeId = PayloadDataType;
    Packet.Payload = PayloadBinary;
    
    ElasticPacket TransmittedPacket = ProtocolStrategy -> GetCurrentProtocol() -> Send(Packet);
    ProtocolObserver -> SendNotificationTransimitted( TransmittedPacket );
}

void UCodeStorySocketClientLibBPLibrary::SendMessage(FString Message)
{
    UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);
    uint16 Channel = 0u;
    uint16 Event =  (int16) CodeStoryMessengerEvent::SEND;
    uint32 PayloadDataType = (int32) ElasticPacketType::STRING;
    uint32 Size = Message.Len() + 1;
    int32 NewSize;
    uint8* PayloadBinary = DataSerializer::StringToUTF8Binary(Message, NewSize);
    
    ElasticPacket Packet;
    Packet.ChannelId = Channel;
    Packet.EventId = Event;
    Packet.AmountOfData = NewSize;
    Packet.TypeId = PayloadDataType;
    Packet.Payload = PayloadBinary;
    
    for(int i = 0; i < NewSize; i++)
    {
        UE_LOG(LogTemp, Warning, TEXT("pb: %d"), Packet.Payload[i]);
    }

    //ElasticPacket TransmittedPacket = ProtocolStrategy -> GetCurrentProtocol() -> Send(Packet);

    //ProtocolObserver -> SendNotificationTransimitted(TransmittedPacket);
}
