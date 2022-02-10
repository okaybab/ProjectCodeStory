// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine.h"
#include "Networking.h"
#include "Sockets.h"
#include "SocketSubsystem.h"

// =====================================================================================================================================
// =====================================================================================================================================
// ========================================================== Struct of Packet =========================================================
// =====================================================================================================================================
// =====================================================================================================================================

static uint32 DEFAULT_BUF_SIZE = 12;

enum ElasticPacketType
{
    STRING = 0,
    INT_32 = 1, // 32비트 (int)
    INT_16 = 2, // 16비트 (short)
    FLOAT = 3
};

struct ElasticPacket
{
    uint16 ChannelId; // 방 id
    uint16 EventId; // 이벤트 id
    uint32 AmountOfData; // 데이터 길이
    uint32 TypeId; // 현재 패킷 구조체에 담긴 Payload 타입
    
    uint8* Payload;
};

// =====================================================================================================================================
// =====================================================================================================================================
// ========================================================== Packet Templates =========================================================
// =====================================================================================================================================
// =====================================================================================================================================

template <typename T>
class CODESTORYSOCKETCLIENTLIB_API BinaryPacketTemplate
{
public:
    virtual uint8* Serialize(T Data) = 0;
    virtual T Deserialize(uint8* Serial);
};

class CODESTORYSOCKETCLIENTLIB_API ErrorTemplate : public BinaryPacketTemplate<int>
{
public:
    uint8* Serialize(int Data);
    int Deserialize(uint8* Serial);
};

class CODESTORYSOCKETCLIENTLIB_API StringPacketTemplate : public BinaryPacketTemplate<FString>
{
public:
    uint8* Serialize(FString Data);
    FString Deserialize(uint8* Serial);
};

// =====================================================================================================================================
// =====================================================================================================================================
// ========================================================== Packet Builder =========================================================
// =====================================================================================================================================
// =====================================================================================================================================

class CODESTORYSOCKETCLIENTLIB_API PacketDescription
{
private:
    uint16 ChannelId = 0;
    uint16 EventId = 0;
    uint32 AmountOfData = 0;
    uint32 TypeId = 0;
    uint8* Payload = 0;
    
public:
    PacketDescription& SetChannelId(uint16 ChannelId);
    PacketDescription& SetEventId(uint16 EventId);
    PacketDescription& SetAmountOfData(uint32 AmountOfData);
    PacketDescription& SetTypeId(uint32 TypeId);
    PacketDescription& SetPayload(uint8* Payload);
    ElasticPacket MakePacket();
    
    static PacketDescription Builder();
};

// =====================================================================================================================================
// =====================================================================================================================================
// ========================================================== PacketUtils ==============================================================
// =====================================================================================================================================
// =====================================================================================================================================

class CODESTORYSOCKETCLIENTLIB_API PacketWrapper
{
public:
    static ElasticPacket CreatePacket();
    static void Fill(ElasticPacket* Packet);
    static void FillDefault(ElasticPacket* Packet, uint16 ChannelId, uint16 EventId, uint32 AmountOfData, uint32 TypeId);
    static uint8* Serialize(ElasticPacket Packet);
    static ElasticPacket DeSerialize(uint8* SerializedPacket);
    static ElasticPacket DeSerializeHeader(uint8* SerializedPacket);
    static void DeSerializePayload(ElasticPacket *Header, uint8* SerializedPacket, uint32 AmountOfData);
};

class CODESTORYSOCKETCLIENTLIB_API DataSerializer
{
public:
    static uint8* IntToBinary(int32 Data);
    static uint8* ShortToBinary(int16 Data);
    static uint8* FloatToBinary(float Data);
    static uint8* StringToBinary(FString Data, int &Size);
    static uint8* StringToUTF8Binary(FString Data, int &Size);
};

class CODESTORYSOCKETCLIENTLIB_API DataDeSerializer
{
public:
    static int32 BinaryToInt(uint8* Data);
    static int16 BinaryToShort(uint8* Data);
    static float BinaryToFloat(uint8* Data);
    static FString BinaryToString(TArray<uint8> Data);
    static FString BinaryUINT8ToString(uint8* Data);
    
    static FString BinaryUINT8ToStringUTF8(uint8* Data);
    
    static int32 BinaryToIntLE(uint8* Data);
    static int16 BinaryToShortLE(uint8* Data);
    static float BinaryToFloatLE(uint8* Data);
};

template<typename T>
inline T BinaryPacketTemplate<T>::Deserialize(uint8* Serial)
{
    return T();
}
