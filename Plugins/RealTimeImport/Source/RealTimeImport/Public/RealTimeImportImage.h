// Copyright 2018-2020 David Romanski(Socke). All Rights Reserved.

#pragma once

#include "RealTimeImport.h"
#include "RealTimeImportBPLibrary.h"
#include "RealTimeImportImage.generated.h"

class FLoadImageFileThread;
class FLoadImageBytesThread;




UCLASS(Blueprintable, BlueprintType)
class URealTimeImportImage : public UObject
{
	GENERATED_UCLASS_BODY()

public:

	static URealTimeImportImage* realTimeImportImage;
	UFUNCTION()
		static URealTimeImportImage* getRealTimeImportImage();


	void LoadImageFile(ERTIDirectoryType directoryType, FString filePath, UTexture2D*& texture, FString& fileName, int32& errorCode, FString& errorMessage, TextureCompressionSettings compressionSettings = TextureCompressionSettings::TC_Default, bool SRGB = true, bool createMipMaps = false);
	void LoadImageFileAsync(ERTIDirectoryType directoryType, FString filePath, FString eventID, TextureCompressionSettings compressionSettings = TextureCompressionSettings::TC_Default, bool SRGB = true, bool createMipMaps = false);
	void LoadImageBytesAsync(FString name, TArray<uint8> bytes, FString eventID, TextureCompressionSettings compressionSettings = TextureCompressionSettings::TC_Default, bool SRGB = true, bool createMipMaps = false);
	void LoadImageBytes(FString name, TArray<uint8> bytes, UTexture2D*& texture, int32& errorCode, FString& errorMessage, TextureCompressionSettings compressionSettings = TextureCompressionSettings::TC_Default, bool SRGB = true, bool createMipMaps = false);


	//void LoadImageAsMipMap(UTexture2D* texture, ERTIDirectoryType directoryType, FString filePath, int32& errorCode, FString& errorMessage);
	void removeTextureFromCacheAndDestroyTexture(UTexture2D* texture);
	void emptyTextureCacheAndDestroyTextures();

	static TArray<uint8> textureRenderTarget2D_ToByteArray(UTextureRenderTarget2D* textureRenderTarget2D, ERTIExportImageType type, int32 quality = 100);
	static TArray<uint8> texture2dToByteArray(UTexture2D* texture2D, ERTIExportImageType type, int32 quality = 100);


	void triggerLoadImageFileEventDelegate(int32 errorCode, FString eventID, UTexture2D* texture, FString fileName, FString errorMessage);
	void triggerLoadImageBytesEventDelegate(int32 errorCode, FString eventID, UTexture2D* texture, FString errorMessage);

private:

	TMap<FString, UTexture2D*> textureCache;

	class FLoadImageFileThread* loadImageFileThread = nullptr;
	class FLoadImageBytesThread* loadImageBytesThread = nullptr;

	UTexture2D* createTexture2D(TArray<uint8>& bytes, FString name, int32& errorCode, FString& errorMessage, TextureCompressionSettings compressionSettings, bool SRGB, bool createMipMaps = false);

	TArray<uint8> resizeRawImageData(TArray<uint8> sourceData, int32 sourceX, int32 sourceY, int32 targetX, int32 targetY, EPixelFormat pixelformat);
	static bool getRawImageData(UTextureRenderTarget2D* TexRT, TArray64<uint8>& RawData);
	static void rawImageDataToBMPData(TArray<uint8>& bmpData, int32 SourceWidth, int32 SourceHeight, const FColor* Data, int64 size);

	void addMipMap(UTexture2D* texture, TArray<uint8> decompressedImageData, int32 width, int32 height, int32& errorCode, FString& errorMessage);
};


/******************* Asyc Threads *************************/
class FLoadImageFileThread : public FRunnable {

public:

	FLoadImageFileThread(URealTimeImportImage* imageLibP, ERTIDirectoryType directoryTypeP, FString filePathP, FString eventIDP, TextureCompressionSettings compressionSettingsP, bool SRGB_P, bool createMipMapsP) :
		imageLib(imageLibP),
		directoryType(directoryTypeP),
		filePath(filePathP),
		eventID(eventIDP),
		compressionSettings(compressionSettingsP),
		SRGB(SRGB_P),
		createMipMaps(createMipMapsP) {
		FString threadName = "FLoadImageFileThread_" + FGuid::NewGuid().ToString();
		thread = FRunnableThread::Create(this, *threadName, 0, EThreadPriority::TPri_Normal);
	}

	virtual uint32 Run() override {

		while (true) {
			UTexture2D* texture;
			FString fileName;
			int32 errorCode;
			FString errorMessage;
			imageLib->LoadImageFile(directoryType, filePath, texture, fileName, errorCode, errorMessage, compressionSettings, SRGB, createMipMaps);

			imageLib->triggerLoadImageFileEventDelegate(errorCode, eventID, texture, fileName, errorMessage);

			texture = nullptr;

			pauseThread(true);
			//workaround. suspend do not work on all platforms. lets sleep
			while (paused) {
				FPlatformProcess::Sleep(0.01);
			}
		}
		return 0;
	}

	void restartThread(URealTimeImportImage* imageLibP, ERTIDirectoryType directoryTypeP, FString filePathP, FString eventIDP, TextureCompressionSettings compressionSettingsP, bool SRGB_P, bool createMipMapsP) {
		imageLib = imageLibP;
		directoryType = directoryTypeP;
		filePath = filePathP;
		eventID = eventIDP;
		compressionSettings = compressionSettingsP;
		SRGB = SRGB_P;
		createMipMaps = createMipMapsP;


		pauseThread(false);
	}

	void pauseThread(bool pause) {
		paused = pause;
		if (thread != nullptr)
			thread->Suspend(pause);
	}


protected:
	URealTimeImportImage* imageLib = nullptr;
	ERTIDirectoryType directoryType;
	FString filePath;
	FString eventID;
	TextureCompressionSettings compressionSettings = TextureCompressionSettings::TC_Default;
	bool SRGB = true;
	bool createMipMaps = false;

	FRunnableThread* thread = nullptr;
	bool paused = true;
};


class FLoadImageBytesThread : public FRunnable {

public:

	FLoadImageBytesThread(URealTimeImportImage* imageLibP, FString nameP, TArray<uint8> bytesP, FString eventIDP, TextureCompressionSettings compressionSettingsP, bool SRGB_P, bool createMipMapsP) :
		imageLib(imageLibP),
		name(nameP),
		bytes(bytesP),
		eventID(eventIDP),
		compressionSettings(compressionSettingsP),
		SRGB(SRGB_P),
		createMipMaps(createMipMapsP) {
		FString threadName = "FLoadImageBytesThread_" + FGuid::NewGuid().ToString();
		thread = FRunnableThread::Create(this, *threadName, 0, EThreadPriority::TPri_Normal);
	}

	virtual uint32 Run() override {

		while (true) {
			UTexture2D* texture;
			int32 errorCode;
			FString errorMessage;
			imageLib->LoadImageBytes(name, bytes, texture, errorCode, errorMessage, compressionSettings, SRGB, createMipMaps);

			imageLib->triggerLoadImageBytesEventDelegate(errorCode, eventID, texture, errorMessage);

			texture = nullptr;
			bytes.Empty();

			pauseThread(true);
			//workaround. suspend do not work on all platforms. lets sleep
			while (paused) {
				FPlatformProcess::Sleep(0.01);
			}
		}

		return 0;
	}

	void restartThread(URealTimeImportImage* imageLibP, FString nameP, TArray<uint8> bytesP, FString eventIDP, TextureCompressionSettings compressionSettingsP, bool SRGB_P, bool createMipMapsP) {
		imageLib = imageLibP;
		name = nameP;
		bytes = bytesP;
		eventID = eventIDP;
		eventID = eventIDP;
		compressionSettings = compressionSettingsP;
		SRGB = SRGB_P;
		createMipMaps = createMipMapsP;

		pauseThread(false);
	}

	void pauseThread(bool pause) {
		paused = pause;
		if (thread != nullptr)
			thread->Suspend(pause);
	}

protected:
	URealTimeImportImage* imageLib = nullptr;
	FString name;
	TArray<uint8> bytes;
	FString eventID;
	TextureCompressionSettings compressionSettings = TextureCompressionSettings::TC_Default;
	bool SRGB = true;
	bool createMipMaps = false;

	FRunnableThread* thread = nullptr;
	bool paused = true;
};