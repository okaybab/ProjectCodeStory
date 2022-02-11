// Fill out your copyright notice in the Description page of Project Settings.


#include "CodeStoryProtocol.h"
#include "CodeStorySocketClientLibBPLibrary.h"

// =====================================================================================================================================
// =====================================================================================================================================
// ====================================================== CodeStoryProtocolStrategy ====================================================
// =====================================================================================================================================
// =====================================================================================================================================

void CodeStoryProtocolStrategy::SetProtocolStrategy(CodeStoryProtocol* NewProtocol)
{
    // 현재 등록된 프로토콜을 메모리에서 안전하게 삭제합니다.
    RemoveCurrentProtocol();
    // 새로운 프로토콜을 등록합니다.
    Protocol = NewProtocol;
}

void CodeStoryProtocolStrategy::RemoveCurrentProtocol()
{
    if(Protocol != nullptr)
    {
        delete Protocol;
        Protocol = nullptr;
    }
}

bool CodeStoryProtocolStrategy::Start(const FString& Addr, int32 Port)
{
    // FSocket 객체를 만듭니다.
    // CreateSocket() Parameter
    // 1. Type
    // 2. Description
    // 3. Force UDP
    FSocket* Socket=nullptr;

    Socket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM) -> CreateSocket(NAME_Stream, TEXT("ue4-tcp-client"));
    
    //소켓 버퍼 사이즈 조정
   /* int NewSize;
    Socket->SetSendBufferSize(96, NewSize);
    FString qqq = FString::FromInt(NewSize);
    GEngine->AddOnScreenDebugMessage(-1, 100.0f, FColor::Blue, qqq);*/

    // 현 프로토콜에 생성된 FSocket 객체를 등록합니다.
    Protocol->SetSocket(Socket);
    Protocol->SetAddr(Addr);
    Protocol->SetPort(Port);
    

    // 원격 서버와 연결을 시도합니다.
    FRunnableThread::Create( Protocol, TEXT( "This is my thread example" ) );

    return (Socket!=nullptr)?true:false;//소켓이 만들어지면 true
}

CodeStoryProtocol* CodeStoryProtocolStrategy::GetCurrentProtocol()
{
    return Protocol;
}

// =====================================================================================================================================
// =====================================================================================================================================
// ====================================================== CodeStoryTcpProtocol =========================================================
// =====================================================================================================================================
// =====================================================================================================================================

CodeStoryTcpProtocol::CodeStoryTcpProtocol()
{
}

CodeStoryTcpProtocol::CodeStoryTcpProtocol(FString _Addr, int32 _Port, CodeStoryProtocolObserver<FString>* _Observer) : CodeStoryProtocolCommon(_Addr, _Port, _Observer)
{
}

bool CodeStoryTcpProtocol::Connect()
{
    // 문자열로된 서버 주소를 FIPv4Address로 변환합니다.
    FIPv4Address Ip = ParseStringAddressToIp(Addr);
    //Create a proper FInternetAddr representation : 적절한 주소를 만들어서 반환합니다.
    TSharedRef<FInternetAddr> InternetAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM) -> CreateInternetAddr();
    
    // 원격 서버의 아이피 주소를 등록합니다.
    InternetAddr -> SetIp(Ip.Value);
    // 원격 서버의 포트 번호를 등록합니다.
    InternetAddr -> SetPort(Port);
    
    // 버퍼 사이즈를 정합니다 (TODO: 설정 객체를 나중에 따로 만들 예정)
    int32 NewSize = 1024;
    // 수신 버퍼 크기를 지정합니다 (1024B)
    Socket -> SetReceiveBufferSize(NewSize, NewSize);
    
    // 원격 서버와 소켓 연결을 시도합니다.
    bool bConnected = Socket -> Connect(*InternetAddr);
    
    GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Black, FString::Printf(TEXT("Trying to...: %s"), bConnected ? TEXT("true") : TEXT("false")));
    
    return bConnected;
}

ElasticPacket CodeStoryTcpProtocol::Recv(int32 RecvDataSize)//소켓버퍼의 크기만큼 받음.
{
    // 만약 소켓 객체가 생성되있는 경우와 연결 상태가 SCS_Connected (연결중) 일 경우
    if(Socket != nullptr && Socket->GetConnectionState() == ESocketConnectionState::SCS_Connected)
    {
        // 만약 수신 큐에 있는 데이터 크기가 0 보다 크다면
        if(RecvDataSize > 0)
        {
            // 바이트 버퍼 선언
            TArray<uint8> ReceivedData;
            int32 BytesCnt = 0;

            // 버퍼 초기화 0을 DEFAULT_BUF_SIZE만큼
            ReceivedData.Init(0, DEFAULT_BUF_SIZE);
            
            // 데이터를 읽어와서 버퍼에 담습니다.
            Socket -> Recv(ReceivedData.GetData(), DEFAULT_BUF_SIZE, BytesCnt);

            // 만약 수신한 버퍼 크기가 0보다 크다면
            if(ReceivedData.Num() > 0)
            {
                // 헤더부분부터 파싱
                ElasticPacket Packet = PacketWrapper::DeSerializeHeader(ReceivedData.GetData());
                // 아직 데이터가 남았는지 확인
                uint32 RecvPayloadDataSize;
                Socket->HasPendingData(RecvPayloadDataSize);
                GEngine->AddOnScreenDebugMessage(-1, 100.f,
                                                 FColor::Black,
                                                 FString::Printf(TEXT("Payload size: %d"), Packet.AmountOfData));
                GEngine->AddOnScreenDebugMessage(-1, 100.f,
                                                 FColor::Black,
                                                 FString::Printf(TEXT("Recv payload has pending data size: %d"), RecvDataSize));

                if(RecvPayloadDataSize > 0 )
                {
                    GEngine->AddOnScreenDebugMessage(-1, 100.0f, FColor::Blue, TEXT("RecvPayloadDataSize > 0"));

                    if (RecvPayloadDataSize >= Packet.AmountOfData)
                    {
                        GEngine->AddOnScreenDebugMessage(-1, 100.0f, FColor::Blue, TEXT("RecvPayloadDataSize >= Packet.AmountOfData"));
                    }
                    
                    BytesCnt = 0;
                    TArray<uint8> PayloadData;
                    PayloadData.Init(0, Packet.AmountOfData);
                    
                    // 페이로드 읽어오기.
                    Socket -> Recv(PayloadData.GetData(), Packet.AmountOfData, BytesCnt);

                    PacketWrapper::DeSerializePayload(&Packet, PayloadData.GetData(), Packet.AmountOfData);
                }
                else
                {
                    GEngine->AddOnScreenDebugMessage(-1, 100.0f, FColor::Blue, TEXT("else"));
                    Socket -> Recv(ReceivedData.GetData(), RecvPayloadDataSize, BytesCnt);
                    return { (uint16) 0, (uint16) 0, 0, 0, (uint8*) 0 };
                }
                
                return Packet;
            }
            else
            {
                GEngine->AddOnScreenDebugMessage(-1, 10.f,
                                                 FColor::Black,
                                                 FString::Printf(TEXT("Received Data is empty...")));
            }
        }
    }
    return { (uint16)0, (uint16)0, 0, 0, (uint8*)0 };
}

void CodeStoryTcpProtocol::RecvWaitLoop()
{
    while (bThreadRun)
    {
        uint32 RecvDataSize;
        Socket->HasPendingData(RecvDataSize);
        
        if(RecvDataSize > 0)
        {
            GEngine->AddOnScreenDebugMessage(-1, 100.0f, FColor::Red, TEXT("RecvWatiLoop()"));
            ElasticPacket Packet = Recv(RecvDataSize);
            
            /*if(Packet.ChannelId == 4294967295)
            {
                continue;
            }*/
            
            CodeStoryProtocolDecoder<FString>* Decoder = Observer -> GetDecoder();
            FString Decoded = Decoder -> Decode(Packet.Payload);
            void* Wrapped = &Decoded;

            Observer -> SendNotificationReceived(Packet, Wrapped);
            delete Packet.Payload;
        }
        FPlatformProcess::Sleep(1.0f);
    }
}

uint32 CodeStoryTcpProtocol::Run()
{
    Connect();
    //RecvWaitLoop();
    return bThreadRun;
}

void CodeStoryTcpProtocol::Stop()
{
    bThreadRun = false;
}

void CodeStoryTcpProtocol::Exit()
{

}


ElasticPacket CodeStoryTcpProtocol::Send(ElasticPacket Packet)
{
    GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Black, FString::Printf(TEXT("Sock send sock object is not nullptr...: %s"), Socket != nullptr ? TEXT("true") : TEXT("false")));
    GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Black, FString::Printf(TEXT("Sock connection state is...: %s"), Socket->GetConnectionState() == ESocketConnectionState::SCS_Connected ? TEXT("SCS_Connected ") : TEXT("NOT CONNECTED")));


    if(Socket != nullptr && Socket->GetConnectionState() == ESocketConnectionState::SCS_Connected)
    {
        for(uint32 i = 0; i < Packet.AmountOfData; i++)
        {
            UE_LOG(LogTemp, Log, TEXT("CodeStoryTcpProtocol::Send %d %d"), Packet.Payload[i], Packet.AmountOfData);
        }
        
        uint8* Serial = PacketWrapper::Serialize(Packet);
        int32 Sent = 0;

        Socket -> Send(Serial, DEFAULT_BUF_SIZE + Packet.AmountOfData, Sent);
        Socket->SetSendBufferSize(Packet.AmountOfData, Sent);
        delete Packet.Payload;
        
        return Packet;
    }
    else {
        return { (uint16) 65535, (uint16) 65535, 4294967295, 4294967295, (uint8*) 255 };
    }
}

void CodeStoryTcpProtocol::SetAddr(FString inputAddr)
{
    this->Addr = inputAddr;
}

void CodeStoryTcpProtocol::SetPort(int32 inputPort)
{
    this->Port = inputPort;
}

// =====================================================================================================================================
// =====================================================================================================================================
// ====================================================== CodeStoryProtocolCommon ======================================================
// =====================================================================================================================================
// =====================================================================================================================================

CodeStoryProtocol::CodeStoryProtocol()
{
}

CodeStoryProtocol::CodeStoryProtocol(FString _Addr, int32 _Port, CodeStoryProtocolObserver<FString>* _Observer)
{
    Addr = _Addr;
    Port = _Port;
    Observer = _Observer;
}

CodeStoryProtocolCommon::CodeStoryProtocolCommon()
{
}

CodeStoryProtocolCommon::CodeStoryProtocolCommon(FString _Addr, int32 _Port, CodeStoryProtocolObserver<FString>* _Observer) : CodeStoryProtocol(_Addr, _Port, _Observer)
{
    
}

void CodeStoryProtocolCommon::SetSocket(FSocket* socket)
{
    this->Socket = socket;
}

FIPv4Address CodeStoryProtocolCommon::ParseStringAddressToIp(const FString& addr)
{
    FIPv4Address Ip;
    FIPv4Address::Parse(addr, Ip);
    
    return Ip;
}

// =====================================================================================================================================
// =====================================================================================================================================
// ====================================================== CodeStoryObserver ============================================================
// =====================================================================================================================================
// =====================================================================================================================================

template <typename T>
CodeStoryProtocolObserver<T>::CodeStoryProtocolObserver(CodeStoryProtocolDecoder<T> *_Decoder, CodeStoryProtocolListener *_Listener)
{
    Decoder = _Decoder;
    Listener = _Listener;
}

template <typename T>
CodeStoryProtocolObserver<T>::~CodeStoryProtocolObserver()
{
    delete Decoder;
    delete Listener;
    
    Decoder = nullptr;
    Listener = nullptr;
}

template <typename T>
void CodeStoryProtocolObserver<T>::SendNotificationReceived(ElasticPacket Packet, void* RecvPayload)
{
    Listener -> OnReceived(Packet, RecvPayload);
}

template <typename T>
void CodeStoryProtocolObserver<T>::SendNotificationTransimitted(ElasticPacket Packet)
{
    Listener -> OnTransmitted(Packet);
}

template <typename T>
CodeStoryProtocolDecoder<T>* CodeStoryProtocolObserver<T>::GetDecoder()
{
    return Decoder;
}
