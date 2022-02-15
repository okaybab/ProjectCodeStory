// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine.h"
#include "Networking.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "CoreMinimal.h"
#include "ElasticPacket.h"
#include "HAL/Runnable.h"

template<typename T>
class CodeStoryProtocolObserver;

class CODESTORYSOCKETCLIENTLIB_API CodeStoryProtocol : public FRunnable
{
protected:
    FString Addr;
    int32 Port;
    FSocket* Socket;
    bool bThreadRun = true;
    CodeStoryProtocolObserver<FString>* Observer;
    
public:
    CodeStoryProtocol();
    CodeStoryProtocol(FString Addr, int32 Port, CodeStoryProtocolObserver<FString>* Observer);
    virtual uint32 Run() override = 0;
    virtual void Stop() override = 0;
    virtual void Exit() override = 0;

    virtual bool Connect() = 0;
    virtual ElasticPacket Recv(int32 RecvDataSize) = 0;
    virtual void RecvWaitLoop() = 0;
    virtual ElasticPacket Send(ElasticPacket Packet) = 0;

    virtual void SetSocket(FSocket* Socket) = 0;
    virtual void SetAddr(FString inputAddr) = 0;
    virtual void SetPort(int32 inputPort) = 0;

    virtual FSocket* Get_Socket() = 0;
    
    virtual FIPv4Address ParseStringAddressToIp(const FString& Addr) = 0;
};

class CODESTORYSOCKETCLIENTLIB_API CodeStoryProtocolCommon : public CodeStoryProtocol
{
public:
    CodeStoryProtocolCommon();
    CodeStoryProtocolCommon(FString Addr, int32 Port, CodeStoryProtocolObserver<FString>* Observer);

    virtual uint32 Run() override = 0;
    virtual void Stop() override = 0;
    virtual void Exit() override = 0;
    
    virtual bool Connect() = 0;
    virtual ElasticPacket Recv(int32 RecvDataSize) = 0;
    virtual void RecvWaitLoop() = 0;
    virtual void SetAddr(FString inputAddr) = 0;
    virtual void SetPort(int32 inputPort) = 0;

    virtual FSocket* Get_Socket() = 0;

    virtual ElasticPacket Send(ElasticPacket Packet) = 0;

    void SetSocket(FSocket* Socket);
    
    FIPv4Address ParseStringAddressToIp(const FString& Addr);
};

class CODESTORYSOCKETCLIENTLIB_API CodeStoryTcpProtocol : public CodeStoryProtocolCommon
{
public:
    CodeStoryTcpProtocol();
    CodeStoryTcpProtocol(FString Addr, int32 Port, CodeStoryProtocolObserver<FString>* Observer);
    
    bool Connect();
    ElasticPacket Recv(int32 RecvDataSize);
    void RecvWaitLoop();
    ElasticPacket Send(ElasticPacket Packet);
    void SetAddr(FString inputAddr);
    void SetPort(int32 inputPort);

    FSocket* Get_Socket();
    
    uint32 Run();
    void Stop();
    void Exit();
};

class CODESTORYSOCKETCLIENTLIB_API CodeStoryProtocolStrategy
{
private:
    CodeStoryProtocol* Protocol = nullptr;
    
public:
    void SetProtocolStrategy(CodeStoryProtocol* Protocol);
    
    bool Start(const FString& Addr, int32 Port);
    void RemoveCurrentProtocol();
    CodeStoryProtocol* GetCurrentProtocol();

};

// =====================================================================================================================================
// =====================================================================================================================================
// ========================================================== Socket Listeners =========================================================
// =====================================================================================================================================
// =====================================================================================================================================

template <typename T>
class CODESTORYSOCKETCLIENTLIB_API CodeStoryProtocolDecoder
{
public:
    virtual T Decode(uint8* Payload) = 0;
};

class CODESTORYSOCKETCLIENTLIB_API CodeStoryProtocolListener
{
public:
    virtual void OnReceived(ElasticPacket Packet, void* RecvPayload) = 0; // 수신했을 때
    virtual void OnTransmitted(ElasticPacket Packet) = 0; // 송신했을 때
    virtual void OnConnected() = 0; // 연결됐을 때
};

template <typename T>
class CODESTORYSOCKETCLIENTLIB_API CodeStoryProtocolObserver
{
private:
    CodeStoryProtocolDecoder<T> *Decoder;
    CodeStoryProtocolListener *Listener;
    
public:
    CodeStoryProtocolObserver(CodeStoryProtocolDecoder<T> *_Decoder, CodeStoryProtocolListener *_Listener);
    ~CodeStoryProtocolObserver();
    
    CodeStoryProtocolDecoder<T>* GetDecoder();
    
    void SendNotificationReceived(ElasticPacket Packet, void* RecvPayload);
    void SendNotificationTransimitted(ElasticPacket Packet);
};
