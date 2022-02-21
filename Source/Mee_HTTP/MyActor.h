// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CodeStoryWebRTCFacade.h"
#include "CodeStoryWebRTCClient.h"
#include "CodeStoryWebRTCBridge.h"
#include "CodeStoryVideoStreamReceiver.h"
#include "Engine/Classes/Components/StaticMeshComponent.h"
#include "third_party/libyuv/include/libyuv.h"
#include "MyActor.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MEE_HTTP_API AMyActor : public AActor, public FCodeStoryVideoStreamReceiverInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	TSharedPtr<CodeStoryWebRTCClient, ESPMode::ThreadSafe> WebRTCClient;
	TSharedPtr<WebSocketWrapper, ESPMode::ThreadSafe> WebSocketWrapper;
	TSharedPtr<CodeStoryWebRTCBridge, ESPMode::ThreadSafe> Bridge;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Webcam)
	bool ShouldResize;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Webcam)
	FVector2D ResizeDeminsions;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Webcam)
	float RefreshRate;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Webcam)
	float RefreshTimer;
	

	void UpdateFrame(rtc::scoped_refptr<webrtc::I420BufferInterface> FrameBuffer);
	
	UFUNCTION(BlueprintCallable, Category = Webcam)
	void DoProcessing();
	
	UFUNCTION(BlueprintCallable, Category = Webcam)
	void UpdateTexture();

	UFUNCTION(BlueprintImplementableEvent)
	void OnUpdatedFrame();

	FVector2D VideoSize;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Webcam)
	UTexture2D* VideoTexture;
	
	void UpdateTextureRegions(UTexture2D* Texture, uint8* SrcData);
	FUpdateTextureRegion2D* VideoUpdateTextureRegion;

	UStaticMeshComponent* PlaneMeshComponent;

	TQueue<TArray<FColor>> RenderQueue;

	double prev_timemills;

	const int FPS = 5;
public:
	virtual void OnFrame(rtc::scoped_refptr<webrtc::I420BufferInterface> FrameBuffer, int VideoWidth, int VideoHeight) override;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent)
	void InitTexture();
};
