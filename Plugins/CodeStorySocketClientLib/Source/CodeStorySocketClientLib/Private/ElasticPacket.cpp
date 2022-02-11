// Fill out your copyright notice in the Description page of Project Settings.


#include "ElasticPacket.h"
// =====================================================================================================================================
// =====================================================================================================================================
// ====================================================== Packet Templates =============================================================
// =====================================================================================================================================
// =====================================================================================================================================

template<typename T>
BinaryPacketTemplate<T> GetPacketSerializationTemplate(ElasticPacketType PacketSerialType)
{
    // NotNull
    BinaryPacketTemplate<T> PacketTemplate = NULL;
    
    switch (PacketSerialType) {
        case ElasticPacketType::STRING:
            PacketTemplate = StringPacketTemplate();
            break;
        default:
            PacketTemplate = ErrorTemplate();
            break;
    }
    
    return PacketTemplate;
}

uint8* StringPacketTemplate::Serialize(FString Data)
{
    int32 Size;
    uint8* Serial = DataSerializer::StringToBinary(Data, Size);
    
    return Serial;
}

FString StringPacketTemplate::Deserialize(uint8* Serial)
{
    FString Data = DataDeSerializer::BinaryUINT8ToStringUTF8(Serial);
    
    return Data;
}

uint8* ErrorTemplate::Serialize(int Data)
{
    return (uint8*) -1;
}

int ErrorTemplate::Deserialize(uint8* Serial)
{
    return -1;
}

// =====================================================================================================================================
// =====================================================================================================================================
// ====================================================== Packet Builder ===============================================================
// =====================================================================================================================================
// =====================================================================================================================================

PacketDescription& PacketDescription::SetChannelId(uint16 _ChannelId)
{
    this->ChannelId = _ChannelId;
    return *this;
}

PacketDescription& PacketDescription::SetEventId(uint16 _EventId)
{
    this->EventId = _EventId;
    return *this;
}

PacketDescription& PacketDescription::SetAmountOfData(uint32 _AmountOfData)
{
    this->AmountOfData = _AmountOfData;
    return *this;
}

PacketDescription& PacketDescription::SetTypeId(uint32 _TypeId)
{
    this->TypeId = _TypeId;
    return *this;
}

PacketDescription& PacketDescription::SetPayload(uint8* _Payload)
{
    this->Payload = _Payload;
    return *this;
}

ElasticPacket PacketDescription::MakePacket()
{
    ElasticPacket Packet;
    
    Packet.ChannelId = this->ChannelId;
    Packet.EventId = this->EventId;
    Packet.AmountOfData = this->AmountOfData;
    Packet.TypeId = this->TypeId;
    Packet.Payload = this->Payload;
    
    return Packet;
}

PacketDescription PacketDescription::Builder()
{
    PacketDescription Description;
    return Description;
}

// =====================================================================================================================================
// =====================================================================================================================================
// ====================================================== DataSeriailizer ==============================================================
// =====================================================================================================================================
// =====================================================================================================================================

uint8* DataSerializer::IntToBinary(int32 Data)
{
    uint8* Binary = new uint8[4];
    
    Binary[0] = Data & 0xff;
    Binary[1] = (Data >> 8) & 0xff;
    Binary[2] = (Data >> 16) & 0xff;
    Binary[3] = (Data >> 24) & 0xff;
    
    return Binary;
}

uint8* DataSerializer::ShortToBinary(int16 Data)
{
    uint8* Binary = new uint8[2];
    
    Binary[0] = Data & 0xff;
    Binary[1] = (Data >> 8) & 0xff;

    return Binary;
}

uint8* DataSerializer::FloatToBinary(float Data)
{
    uint8* Binary = new uint8[4];

    Binary[0] = (*((unsigned long*) & Data));
    Binary[1] = (*((unsigned long*) & Data) >> 8);
    Binary[2] = (*((unsigned long*) & Data) >> 16);
    Binary[3] = (*((unsigned long*) & Data) >> 24);
    
    return Binary;
}

uint8* DataSerializer::StringToBinary(FString Data, int& Size)
{
    TCHAR* SerializedChar = Data.GetCharArray().GetData();
    Size = FCString::Strlen(SerializedChar) + 1;
    uint8* Copy = new uint8[Size];
    
    int i = 0;
    for(i = 0; i < Size; i++)
    {
        Copy[i] = (uint8) SerializedChar[i];
    }
    Copy[i] = '\0';
    
    return Copy;
}

uint8* DataSerializer::StringToUTF8Binary(FString Data, int& Size)
{
    TCHAR* Serialized = Data.GetCharArray().GetData();
    Size = FCString::Strlen(Serialized) * 3 + 1;
    uint8* Copy = new uint8[Size];
    uint8* Dst = (uint8*) TCHAR_TO_UTF8(Serialized);
    
    int i =0;
    for(i = 0; i < Size; i++)
    {
        Copy[i] = (uint8) Dst[i];
        UE_LOG(LogTemp, Log, TEXT("%d"), Copy[i]);
    }
    Copy[i] = '\0';
    
    return Copy;
}


// =====================================================================================================================================
// =====================================================================================================================================
// ====================================================== DataDeSeriailizer ==============================================================
// =====================================================================================================================================
// =====================================================================================================================================

int32 DataDeSerializer::BinaryToInt(uint8* Data)
{
    int32 n;
    
    *((uint8*)(&n) + 3) = Data[0];
    *((uint8*)(&n) + 2) = Data[1];
    *((uint8*)(&n) + 1) = Data[2];
    *((uint8*)(&n) + 0) = Data[3];

    return n;
}

int16 DataDeSerializer::BinaryToShort(uint8* Data)
{
    int16 n = 0;
    
    *((uint8*)(&n) + 1) = Data[0];
    *((uint8*)(&n) + 0) = Data[1];
    
    return n;
}

float DataDeSerializer::BinaryToFloat(uint8* Data)
{
    float v;
    *((uint8*)(&v) + 3) = Data[0];
    *((uint8*)(&v) + 2) = Data[1];
    *((uint8*)(&v) + 1) = Data[2];
    *((uint8*)(&v) + 0) = Data[3];
    
    return v;
}

FString DataDeSerializer::BinaryToString(TArray<uint8> Data)
{
    Data.Add('\0');
    
    UE_LOG(LogTemp, Log, TEXT("Data.GetData():: %d"), *Data.GetData());
    
    return BinaryUINT8ToString(Data.GetData());
}

FString DataDeSerializer::BinaryUINT8ToStringUTF8(uint8* Data)
{
    TCHAR* T = UTF8_TO_TCHAR(Data);
    FString DeserializedBinaryPayload = T;
    UE_LOG(LogTemp, Log, TEXT("ASASDSA:: %s"), *DeserializedBinaryPayload);
    
    return DeserializedBinaryPayload;
}


FString DataDeSerializer::BinaryUINT8ToString(uint8* Data)
{
    return FString(reinterpret_cast<const char*>(Data));
}

int32 DataDeSerializer::BinaryToIntLE(uint8* Data)
{
    int32 n;
    
    *((uint8*)(&n) + 0) = Data[0];
    *((uint8*)(&n) + 1) = Data[1];
    *((uint8*)(&n) + 2) = Data[2];
    *((uint8*)(&n) + 3) = Data[3];

    return n;
}

int16 DataDeSerializer::BinaryToShortLE(uint8* Data)
{
    int16 n = 0;
    
    *((uint8*)(&n) + 0) = Data[0];
    *((uint8*)(&n) + 1) = Data[1];
    
    return n;
}

float DataDeSerializer::BinaryToFloatLE(uint8* Data)
{
    float v;
    
    *((uint8*)(&v) + 0) = Data[0];
    *((uint8*)(&v) + 1) = Data[1];
    *((uint8*)(&v) + 2) = Data[2];
    *((uint8*)(&v) + 3) = Data[3];
    
    return v;
}

// =====================================================================================================================================
// =====================================================================================================================================
// ============================================================== Packet ===============================================================
// =====================================================================================================================================
// =====================================================================================================================================

ElasticPacket PacketWrapper::CreatePacket()
{
    ElasticPacket Packet;
    Fill(&Packet);
    
    return Packet;
}

void PacketWrapper::Fill(ElasticPacket* Packet)
{
    FillDefault(Packet, 10, 10, 110, 1110);
}

void PacketWrapper::FillDefault(ElasticPacket* Packet, uint16 ChannelId, uint16 EventId, uint32 AmountOfData, uint32 TypeId)
{
    if(Packet == NULL)
    {
        return;
    }
    ElasticPacket* PacketPointer = (ElasticPacket*) Packet;
    
    PacketPointer -> ChannelId = ChannelId;
    PacketPointer -> EventId = EventId;
    PacketPointer -> AmountOfData = AmountOfData;
    PacketPointer -> TypeId = TypeId;
}

uint8* PacketWrapper::Serialize(ElasticPacket Packet)
{
//    unsigned char* Bytes = reinterpret_cast<unsigned char*>(Packet);
//    uint8* Header = (uint8*) Bytes;
    uint8* Serial = new uint8[DEFAULT_BUF_SIZE + Packet.AmountOfData];
    uint8* Channel_ = DataSerializer::ShortToBinary(Packet.ChannelId);
    uint8* Event_ = DataSerializer::ShortToBinary(Packet.EventId);
    uint8* Amd_ = DataSerializer::IntToBinary(Packet.AmountOfData);
    uint8* Tid_ = DataSerializer::IntToBinary(Packet.TypeId);
    
    Serial[0] = Channel_[0];
    Serial[1] = Channel_[1];
    
    Serial[2] = Event_[0];
    Serial[3] = Event_[1];
    
    Serial[4] = Amd_[0];
    Serial[5] = Amd_[1];
    Serial[6] = Amd_[2];
    Serial[7] = Amd_[3];
    
    Serial[8] = Tid_[0];
    Serial[9] = Tid_[1];
    Serial[10] = Tid_[2];
    Serial[11] = Tid_[3];

    for(uint32 i = 0; i <Packet.AmountOfData; i++)
    {
        UE_LOG(LogTemp, Log, TEXT("Log Message %d"), Packet.Payload[i]);
    }
//
//    for(int i = 0; i < DEFAULT_BUF_SIZE; i++)
//    {
//        Serial[i] = Header[i];
//    }
    
    for(uint32 i = DEFAULT_BUF_SIZE; i < DEFAULT_BUF_SIZE + Packet.AmountOfData; i++)
    {
        Serial[i] = Packet.Payload[i - DEFAULT_BUF_SIZE];
    }
    
    return Serial;
}

void PacketWrapper::DeSerializePayload(ElasticPacket *Header, uint8* SerializedPacket, uint32 AmountOfData)
{
    uint8* Copy = new uint8[AmountOfData];
    for(uint32 i = 0; i < AmountOfData; i++)
    {
        UE_LOG(LogTemp, Log, TEXT("PacketWrapper::Decode %d %d"), SerializedPacket[i], AmountOfData);
        Copy[i] = SerializedPacket[i];
    }
    
    Header -> Payload = Copy;
}

ElasticPacket PacketWrapper::DeSerializeHeader(uint8* SerializedPacket)
{
    ElasticPacket Packet;
    Packet.ChannelId = ((ElasticPacket*) SerializedPacket)->ChannelId;
    Packet.EventId = ((ElasticPacket*) SerializedPacket)->EventId;
    Packet.AmountOfData = ((ElasticPacket*) SerializedPacket)->AmountOfData;
    Packet.TypeId = ((ElasticPacket*) SerializedPacket)->TypeId;
    
    return Packet;
}

ElasticPacket PacketWrapper::DeSerialize(uint8* SerializedPacket)
{
    ElasticPacket Packet;
    Packet.ChannelId = ((ElasticPacket*) SerializedPacket)->ChannelId;
    Packet.EventId = ((ElasticPacket*) SerializedPacket)->EventId;
    Packet.AmountOfData = ((ElasticPacket*) SerializedPacket)->AmountOfData;
    Packet.TypeId = ((ElasticPacket*) SerializedPacket)->TypeId;
    
    uint8* Copy = new uint8[Packet.AmountOfData];
    for(uint32 i = DEFAULT_BUF_SIZE; i < DEFAULT_BUF_SIZE + Packet.AmountOfData; i++)
    {
        UE_LOG(LogTemp, Log, TEXT("PacketWrapper::Decode %d %d"), SerializedPacket[i], Packet.AmountOfData);
        Copy[i - DEFAULT_BUF_SIZE] = SerializedPacket[i];
    }
    Packet.Payload = Copy;
    
    return Packet;

}
