// Fill out your copyright notice in the Description page of Project Settings.

#include "MyActor.h"

#include "HighResScreenshot.h"
#include "Rendering/Texture2DResource.h"

#include <chrono>
#include <ctime>
#include <sys/utime.h>
#include <iostream>
// Sets default values
AMyActor::AMyActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RefreshRate = 15.0f;
	VideoSize = FVector2D(0, 0);
	ShouldResize = true;
	ResizeDeminsions = FVector2D(320, 240);
	RefreshTimer = 0.0f;
}

// Called when the game starts or when spawned
void AMyActor::BeginPlay()
{
	Super::BeginPlay();
	
	const TSharedRef<CodeStoryVideoStreamReceiver> VideoCallbackImpl = MakeShared<CodeStoryVideoStreamReceiver>(CodeStoryVideoStreamReceiver(MakeShareable(this)));
	WebSocketWrapper = MakeShareable(
		CodeStoryWebRTCFacade::CreateWebSocket(TEXT("ws://45.32.249.81:7777/"), CodeStoryWebSocket::EnumToString(CodeStoryWebSocket::WS))
	);
	WebRTCClient = MakeShared<CodeStoryWebRTCClient, ESPMode::ThreadSafe>(CodeStoryWebRTCFacade::CreateClient(VideoCallbackImpl));
	Bridge = MakeShared<CodeStoryWebRTCBridge, ESPMode::ThreadSafe>(CodeStoryWebRTCFacade::CreateWebRTC(*WebRTCClient.Get(), *WebSocketWrapper.Get()));
	
	WebRTCClient->SetWebRTCBridge(Bridge);
	Bridge.Get()->CreatePeerConnection();
	Bridge.Get()->ConnectToSignalingProxyServer();
	PlaneMeshComponent = this->FindComponentByClass<UStaticMeshComponent>();
}

void AMyActor::OnFrame(rtc::scoped_refptr<webrtc::I420BufferInterface> FrameBuffer, int VideoWidth, int VideoHeight)
{
	if(ShouldResize)
	{
		VideoSize.X = VideoWidth;
		VideoSize.Y = VideoHeight;
		VideoTexture = UTexture2D::CreateTransient(VideoSize.X, VideoSize.Y);
		VideoUpdateTextureRegion = new FUpdateTextureRegion2D(0, 0, 0, 0, VideoSize.X, VideoSize.Y);
		DynamicMat = UMaterialInstanceDynamic::Create(PlaneMeshComponent->GetMaterial(0), nullptr);
		DynamicMat->SetTextureParameterValue(FName("VTexture"), VideoTexture);
		PlaneMeshComponent->SetMaterial(0, DynamicMat);
		InitTexture();
		
		ShouldResize = false;
	}
	
	UpdateFrame(FrameBuffer);
}

void AMyActor::DoProcessing()
{
	
}

void AMyActor::UpdateTexture()
{
}

void AMyActor::UpdateFrame(rtc::scoped_refptr<webrtc::I420BufferInterface> FrameBuffer)
{
	double currentTime = FPlatformTime::Seconds();

	if (currentTime > 1. / FPS)
	{
		prev_timemills = FPlatformTime::Seconds();

		/* i420 to RGB ?????? ??? UE4 ???????????? ????????? ????????? */
		// I420 ????????? ????????????.
		const webrtc::I420BufferInterface* I420Buf = FrameBuffer->ToI420();
		uint8_t* Dest = new uint8_t[FrameBuffer->width() * FrameBuffer->height() * 4];

		// libyuv??? ???????????? yuv420 ????????? RGBA ???????????? ????????????.
		if (libyuv::I420ToRGBA(
			I420Buf->DataY(), I420Buf->StrideY(),
			I420Buf->DataU(), I420Buf->StrideU(),
			I420Buf->DataV(), I420Buf->StrideV(),
			Dest, FrameBuffer->width() * 4, FrameBuffer->width(), FrameBuffer->height()
		) > -1)
		{
			// ????????? ???????????? ?????? ?????? ????????? ?????????...?
			VideoSize.X = FrameBuffer->width();
			VideoSize.Y = FrameBuffer->height();

			// RGB Array ?????????
			TArray<FColor> Data;
			Data.Init(FColor(0, 0, 0, 255), VideoSize.X * VideoSize.Y);

			// ????????? RGBA ????????? TArray??? ????????????.
			for (int y = 0; y < FrameBuffer->height(); y++)
			{
				for (int x = 0; x < FrameBuffer->width(); x++)
				{
					const int pos = x + y * static_cast<int>(VideoSize.X);

					Data[pos].A = Dest[pos * 4];
					Data[pos].R = Dest[pos * 4 + 1];
					Data[pos].G = Dest[pos * 4 + 2];
					Data[pos].B = Dest[pos * 4 + 3];
				}
			}

			// ?????? OnFrame??? ???????????? ???????????? ?????? webrtc ?????? ??????????????? ????????? RenderQueue ???????????? Tick?????? ?????? ??????.
			// ????????? ?????? ?????? ?????? Thread crash??? ??????.
			RenderQueue.Enqueue(Data);
		}

		delete Dest;
	}
}

void AMyActor::UpdateTextureRegions(UTexture2D* Texture, uint8* SrcData)
{
	FTexture2DMipMap& Mip = Texture->PlatformData->Mips[0];
	void* BinaryData = Mip.BulkData.Lock( LOCK_READ_WRITE );
	
	FMemory::Memcpy( BinaryData, SrcData, (VideoSize.X * VideoSize.Y) * 4 );
	Mip.BulkData.Unlock();

	Texture->UpdateResource();
	DynamicMat->SetTextureParameterValue(FName("VTexture"), Texture);
	UE_LOG(LogTemp, Log, TEXT("DP 2"));
}

// Called every frame
void AMyActor::Tick(float DeltaTime)
{
	if(!ShouldResize)
	{
		RefreshTimer += DeltaTime;
		TArray<FColor> Data;
		Data.Init(FColor(255, 0, 0, 255), VideoSize.X * VideoSize.Y);
		UE_LOG(LogTemp, Log, TEXT("DP 1"));


		if(!RenderQueue.IsEmpty() && RenderQueue.Dequeue(Data))
		{
			UpdateTextureRegions(VideoTexture, (uint8*) Data.GetData());
			UE_LOG(LogTemp, Log, TEXT("DP 3"));
		}		
	}
}
