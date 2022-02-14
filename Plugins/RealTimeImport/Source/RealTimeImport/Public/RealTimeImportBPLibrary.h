// Copyright 2018-2020 David Romanski(Socke). All Rights Reserved.


#pragma once

#include "RealTimeImport.h"

#ifndef __FileFunctionsRealTimeImport
#define __FileFunctionsRealTimeImport
#include "FileFunctionsRealTimeImport.h"
#endif

#ifndef __RealTimeImportImage
#define __RealTimeImportImage
#include "RealTimeImportImage.h"
#endif

#ifndef __RealTimeImportMesh
#define __RealTimeImportMesh
#include "RealTimeImportMesh.h"
#endif

#ifndef __RealTimeImportSound
#define __RealTimeImportSound
#include "RealTimeImportSound.h"
#endif


#include "RealTimeImportBPLibrary.generated.h"





UCLASS()
class REALTIMEIMPORT_API URealTimeImportBPLibrary : public UObject
{
	GENERATED_UCLASS_BODY()

public:

	//Delegates
	//Files
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FLoadFileEventDelegate, int32, errorCode, FString, eventID, const TArray<uint8>&, bytes, FString, fileName, FString, errorMessage);
	UFUNCTION()
		void LoadFileEventDelegate(const int32 errorCode, const FString eventID, const TArray<uint8>& bytes, const FString fileName, const FString errorMessage);
	UPROPERTY(BlueprintAssignable, Category = "RealTimeImport|File|Events|LoadFile")
		FLoadFileEventDelegate onLoadFileEventDelegate;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FSaveFileEventDelegate, int32, errorCode, FString, eventID, FString, fileName, FString, fullPath, FString, errorMessage);
	UFUNCTION()
		void SaveFileEventDelegate(const int32 errorCode, const FString eventID, const FString fileName, const FString fullPath, const FString errorMessage);
	UPROPERTY(BlueprintAssignable, Category = "RealTimeImport|File|Events|SaveFile")
		FSaveFileEventDelegate onSaveFileEventDelegate;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FLoadAllFilesFromDirectoryEventDelegate, FString, eventID, int32, count, const TArray<FString>&, files, const TArray<FString>&, filePaths);
	UFUNCTION()
		void LoadAllFilesFromDirectoryEventDelegate(const FString eventID, const int32& count, const  TArray<FString>& files, const TArray<FString>& filePaths);
	UPROPERTY(BlueprintAssignable, Category = "RealTimeImport|File|Events|LoadAllFilesFromDirectory")
		FLoadAllFilesFromDirectoryEventDelegate onLoadAllFilesFromDirectoryEventDelegate;



	//WAV
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FCreateSoundWaveFromFileEventDelegate, int32, errorCode, FString, eventID, USoundWaveProcedural*, Sound, FString, fileName, FString, errorMessage);
	UFUNCTION()
		void CreateSoundWaveFromFileEventDelegate(const int32 errorCode, const FString eventID, const USoundWaveProcedural* Sound, const FString fileName, const FString errorMessage);
	UPROPERTY(BlueprintAssignable, Category = "RealTimeImport|Sound|Events|CreateSoundWaveFromFile")
		FCreateSoundWaveFromFileEventDelegate onCreateSoundWaveFromFileEventDelegate;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FCreateSoundWaveFromBytesEventDelegate, int32, errorCode, FString, eventID, USoundWaveProcedural*, Sound, FString, errorMessage);
	UFUNCTION()
		void CreateSoundWaveFromBytesEventDelegate(const int32 errorCode, const FString eventID, const USoundWaveProcedural* Sound, const FString errorMessage);
	UPROPERTY(BlueprintAssignable, Category = "RealTimeImport|Sound|Events|CreateSoundWaveFromBytes")
		FCreateSoundWaveFromBytesEventDelegate onCreateSoundWaveFromBytesEventDelegate;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FPlaySoundEventDelegate, int32, errorCode, FString, fileID, FString, errorMessage, float, durationInSeconds);
	UFUNCTION()
		void PlaySoundEventDelegate(const int32 errorCode, const FString fileID, const FString errorMessage, const float durationInSeconds);
	UPROPERTY(BlueprintAssignable, Category = "RealTimeImport|Sound|Events|Play")
		FPlaySoundEventDelegate onPlaySoundEventDelegate;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FActiveSoundPositionEventDelegate, FString, fileID, float, positionSecondsStart, float, positionSecondsEnd, float, positionPercent);
	UFUNCTION()
		void ActiveSoundPositionEventDelegate(const FString fileID, const float positionSecondsStart, const float positionSecondsEnd, const float positionPercent);
	UPROPERTY(BlueprintAssignable, Category = "RealTimeImport|Sound|Events|Position")
		FActiveSoundPositionEventDelegate onActiveSoundPositionEventDelegate;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FActiveSoundEnvelopeValueEventDelegate, float, averageEnvelopeValue, float, maxEnvelope, int32, numWaveInstances);
	UFUNCTION()
		void ActiveSoundEnvelopeValueEventDelegate(const float averageEnvelopeValue, const float maxEnvelope, const int32 numWaveInstances);
	UPROPERTY(BlueprintAssignable, Category = "RealTimeImport|Sound|Events|EnvelopeValue")
		FActiveSoundEnvelopeValueEventDelegate onActiveSoundEnvelopeValueEventDelegate;
	

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStopSoundEventDelegate, FString, fileID);
	UFUNCTION()
		void StopSoundEventDelegate(const FString fileID);
	UPROPERTY(BlueprintAssignable, Category = "RealTimeImport|Sound|Events|Stop")
		FStopSoundEventDelegate onStopSoundEventDelegate;

	//Images
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FLoadImageFileEventDelegate, int32, errorCode, FString, eventID, UTexture2D*, texture, FString, fileName, FString, errorMessage);
	UFUNCTION()
		void LoadImageFileEventDelegate(const int32 errorCode, const FString eventID, const UTexture2D* texture, const FString fileName, const FString errorMessage);
	UPROPERTY(BlueprintAssignable, Category = "RealTimeImport|Image|Events|LoadImageFile")
		FLoadImageFileEventDelegate onLoadImageFileEventDelegate;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FLoadImageBytesEventDelegate, int32, errorCode, FString, eventID, UTexture2D*, texture, FString, errorMessage);
	UFUNCTION()
		void LoadImageBytesEventDelegate(const int32 errorCode, const FString eventID, const UTexture2D* texture, const FString errorMessage);
	UPROPERTY(BlueprintAssignable, Category = "RealTimeImport|Image|Events|LoadImageBytes")
		FLoadImageBytesEventDelegate onLoadImageBytesEventDelegate;


	static URealTimeImportBPLibrary* RealTimeImportBPLibrary;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "RealTimeImport")
		static URealTimeImportBPLibrary* getRealTimeImportTarget();

	//For an easier readable C++ code some functions were moved to the UFileFunctionsRealTimeImport class. For compatibility reasons, however, the functions will remain accessible via the URealTimeImportBPLibrary.

	UFUNCTION(BlueprintCallable, Category = "RealTimeImport|File")
		static void LoadFile(ERTIDirectoryType directoryType, FString filePath, FString& fileName, TArray<uint8>& bytes, int32& errorCode, FString& errorMessage);

	UFUNCTION(BlueprintCallable, Category = "RealTimeImport|File")
		URealTimeImportBPLibrary* LoadFileAsync(ERTIDirectoryType directoryType, FString filePath, FString eventID);

	UFUNCTION(BlueprintCallable, Category = "RealTimeImport|File")
		static void SaveFile(ERTIDirectoryType directoryType, FString filePath, bool createDirectories, bool overwriteFile, TArray<uint8> bytes, FString& fileName, FString& fullPath, int32& errorCode, FString& errorMessage);

	UFUNCTION(BlueprintCallable, Category = "RealTimeImport|File")
		URealTimeImportBPLibrary* SaveFileAsync(ERTIDirectoryType directoryType, FString filePath, bool createDirectories, bool overwriteFile, TArray<uint8> bytes, FString eventID);

	UFUNCTION(BlueprintCallable, Category = "RealTimeImport|File")
		static void LoadAllFilesFromDirectory(ERTIDirectoryType directoryType, ERTIFileType fileType, FString dirPath, bool recursiveSearch, int32& count, TArray<FString>& files, TArray<FString>& filePaths);

	UFUNCTION(BlueprintCallable, Category = "RealTimeImport|File")
		URealTimeImportBPLibrary* LoadAllFilesFromDirectoryAsync(ERTIDirectoryType directoryType, ERTIFileType fileType, FString dirPath, bool recursiveSearch, FString eventID);


	//For an easier readable C++ code some functions were moved to the URealTimeImportSound class. For compatibility reasons, however, the functions will remain accessible via the URealTimeImportBPLibrary.

	/**
	*Creates a USoundWaveProcedural object reference from a sound file on disk that can be used in further UE4 functions. Not all sound functions work with it. For example the start time must be 0. File remains in RAM and does not have to be reloaded until the sound cache is cleared with the corresponding function. On Android and IOS, playback problems may occur depending on the sound file and smartphone model. In case of problems, please use the Play and Stream functions instead.
	*@param directoryType Absolute path or relative path.
	*@param filePath Path inclusive file.
	*@param maxFileSize In megabytes. 0 = any size is possible. 0 > an error message is triggered if the file is larger.
	*@param soundFileType The type (wav, ogg) of sound file that will be loaded
	*/
	UFUNCTION(BlueprintCallable, Category = "RealTimeImport|Sound")
		void CreateSoundWaveFromFile(ERTIDirectoryType directoryType, FString filePath, float maxFileSize, FString& fileID, ERTISoundFileType soundFileType, USoundWaveProcedural*& soundWave, FString& fileName, int32& errorCode, FString& errorMessage) {
		URealTimeImportSound::getRealTimeImportSound()->CreateSoundWaveFromFile(directoryType, filePath, maxFileSize, fileID, soundFileType, soundWave, fileName, errorCode, errorMessage);
	};


	/**
	*Creates a USoundWaveProcedural object reference from a sound file on disk that can be used in further UE4 functions. Not all sound functions work with it. For example the start time must be 0. File remains in RAM and does not have to be reloaded until the sound cache is cleared with the corresponding function. On Android and IOS, playback problems may occur depending on the sound file and smartphone model. In case of problems, please use the Play and Stream functions instead.
	*@param directoryType Absolute path or relative path.
	*@param filePath Path inclusive file.
	*@param maxFileSize In megabytes. 0 = any size is possible. 0 > an error message is triggered if the file is larger.
	*@param soundFileType The type (wav, ogg) of sound file that will be loaded
	*@param eventID To identify different events, you can enter an ID here which will be output again in the matching event.
	*/
	UFUNCTION(BlueprintCallable, Category = "RealTimeImport|Sound")
		URealTimeImportBPLibrary* CreateSoundWaveFromFileAsync(ERTIDirectoryType directoryType, FString filePath, float maxFileSize, ERTISoundFileType soundFileType, FString eventID) {
		URealTimeImportSound::getRealTimeImportSound()->CreateSoundWaveFromFileAsync(directoryType, filePath, maxFileSize, soundFileType, eventID);
		return RealTimeImportBPLibrary;
	};

	/**
	*Creates a USoundWaveProcedural object reference from a sound file on disk that can be used in further UE4 functions. Not all sound functions work with it. For example the start time must be 0. File remains in RAM and does not have to be reloaded until the sound cache is cleared with the corresponding function. On Android and IOS, playback problems may occur depending on the sound file and smartphone model. In case of problems, please use the Play and Stream functions instead.
	*@param name An individual name to cache the image better in RAM
	*@param bytes Image bytes
	*@param maxFileSize In megabytes. 0 = any size is possible. 0 > an error message is triggered if the file is larger.
	*@param soundFileType The type (wav, ogg) of sound file that will be loaded
	*/
	UFUNCTION(BlueprintCallable, Category = "RealTimeImport|Sound")
		void CreateSoundWaveFromBytes(FString name, TArray<uint8> bytes, float maxFileSize, ERTISoundFileType soundFileType, USoundWaveProcedural*& Sound, int32& errorCode, FString& errorMessage) {
		URealTimeImportSound::getRealTimeImportSound()->CreateSoundWaveFromBytes(name, bytes, maxFileSize, soundFileType, Sound, errorCode, errorMessage);
	};

	/**
	*Creates a USoundWaveProcedural object reference from a sound file on disk that can be used in further UE4 functions. Not all sound functions work with it. For example the start time must be 0. File remains in RAM and does not have to be reloaded until the sound cache is cleared with the corresponding function. On Android and IOS, playback problems may occur depending on the sound file and smartphone model. In case of problems, please use the Play and Stream functions instead.
	*@param name An individual name to cache the image better in RAM
	*@param bytes Image bytes
	*@param maxFileSize In megabytes. 0 = any size is possible. 0 > an error message is triggered if the file is larger.
	*@param soundFileType The type (wav, ogg) of sound file that will be loaded
	*@param eventID To identify different events, you can enter an ID here which will be output again in the matching event.
	*/
	UFUNCTION(BlueprintCallable, Category = "RealTimeImport|Sound")
		URealTimeImportBPLibrary* CreateSoundWaveFromBytesAsync(FString name, TArray<uint8> bytes, float maxFileSize, ERTISoundFileType soundFileType, FString eventID) {
		URealTimeImportSound::getRealTimeImportSound()->CreateSoundWaveFromBytesAsync(name, bytes, maxFileSize, soundFileType, eventID);
		return RealTimeImportBPLibrary;
	};

	/**
	* Removes a Sound file from the cache if you didn't do it on stop. Can only be removed when it is not playing.
	*@param fileID
	*/
	UFUNCTION(BlueprintCallable, Category = "RealTimeImport|Sound")
		void removeSoundFromCache(FString fileID) {
		URealTimeImportSound::getRealTimeImportSound()->removeSoundFromCache(fileID);
	};

	/**
	*Removes all sound files from the cache. Can only be removed when it is not playing.
	*@param fileID
	*/
	UFUNCTION(BlueprintCallable, Category = "RealTimeImport|Sound")
		void emptySoundCache() {
		URealTimeImportSound::getRealTimeImportSound()->emptySoundCache();
	};

	/**
	* Loads a Sound file into RAM and plays it. Offers some functions, like changing the start time, compared to the CreateSound functions. But other functions are missing.
	*@param directoryType Absolute path or relative path.
	*@param filePath Path inclusive file.
	*@param maxFileSize In megabytes. 0 = any size is possible. 0 > an error message is triggered if the file is larger.
	*@param volumeMultiplier Volume.
	*@param playSpeed Playback speed
	*@param startTimeInSeconds From when the Wav should be played. Specified in seconds.
	*@param soundFileType The type (wav, ogg) of sound file that will be loaded
	*/
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, meta = (WorldContext = "WorldContextObject", UnsafeDuringActorConstruction = "true"), Category = "RealTimeImport|Sound")
		void loadAndPlaySoundFileAsync(const UObject* WorldContextObject, ERTIDirectoryType directoryType, FString filePath, float maxFileSize, float volumeMultiplier = 1.0, float playSpeed = 1.0, int32 startTimeInSeconds = 0, bool persistAcrossLevelTransition = true, ERTISoundFileType soundFileType = ERTISoundFileType::E_wav) {
		URealTimeImportSound::getRealTimeImportSound()->loadAndPlaySoundFileAsync(WorldContextObject, directoryType, filePath, maxFileSize, volumeMultiplier, playSpeed, startTimeInSeconds, persistAcrossLevelTransition, soundFileType);
	};

	/**
	* Loads Soud bytes into RAM and plays it. Offers some functions, like changing the start time, compared to the CreateSound functions. But other functions are missing.
	*@param bytes It must be the bytes of a complete Wav with header.
	*@param volumeMultiplier Volume.
	*@param playSpeed Playback speed
	*@param startTimeInSeconds From when the Wav should be played. Specified in seconds.
	*@param soundFileType The type (wav, ogg) of sound file that will be loaded
	*/
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, meta = (WorldContext = "WorldContextObject", UnsafeDuringActorConstruction = "true"), Category = "RealTimeImport|Sound")
		void loadAndPlaySoundBytesAsync(const UObject* WorldContextObject, TArray<uint8> bytes, float volumeMultiplier = 1.0, float playSpeed = 1.0, int32 startTimeInSeconds = 0, bool persistAcrossLevelTransition = true) {
		URealTimeImportSound::getRealTimeImportSound()->loadAndPlaySoundBytesAsync(WorldContextObject, bytes, volumeMultiplier, playSpeed, startTimeInSeconds, persistAcrossLevelTransition);
	};


	/**
	* With this node a Wav is streamed. Only 1 second of sound is read from the hard disk and played. Extremely low RAM usage.
	*@param directoryType Absolute path or relative path.
	*@param filePath Path inclusive file.
	*@param volumeMultiplier Volume.
	*@param startTimeInSeconds From when the Wav should be played. Specified in seconds.
	*/
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, meta = (WorldContext = "WorldContextObject", UnsafeDuringActorConstruction = "true"), Category = "RealTimeImport|Sound")
		void streamWavFile(bool& success, FString& errorMessage, const UObject* WorldContextObject, ERTIDirectoryType directoryType, FString filePath, float volumeMultiplier = 1.0, /*float playSpeed = 1.0,*/ int32 startTimeInSeconds = 0, bool persistAcrossLevelTransition = true) {
		URealTimeImportSound::getRealTimeImportSound()->streamWavFile(success, errorMessage, WorldContextObject, directoryType, filePath, volumeMultiplier, startTimeInSeconds, persistAcrossLevelTransition);
	};

	/**
	* This allows you to jump to a specific position of a wav file that is being streamed.
	*@param fileID This ID is generated when starting playback.
	*/
	UFUNCTION(BlueprintCallable, Category = "RealTimeImport|Sound")
		void seekWavFileStream(FString fileID, float positionInSeconds) {
		URealTimeImportSound::getRealTimeImportSound()->seekWavFileStream(fileID, positionInSeconds);
	};

	/**
	* Pauses a Wav file.
	*@param fileID This ID is generated when starting playback.
	*/
	UFUNCTION(BlueprintCallable, Category = "RealTimeImport|Sound")
		void pauseSound(FString fileID) {
		URealTimeImportSound::getRealTimeImportSound()->pauseSound(fileID);
	};

	/**
	* Resumes a Wav file.
	*@param fileID This ID is generated when starting playback.
	*/
	UFUNCTION(BlueprintCallable, Category = "RealTimeImport|Sound")
		void resumeSound(FString fileID) {
		URealTimeImportSound::getRealTimeImportSound()->resumeSound(fileID);
	};

	/**
	* Stop a Wav file.
	*@param fileID This ID is generated when starting playback.
	*@param Determines whether the wav should remain in RAM if it exists.
	*/
	UFUNCTION(BlueprintCallable, Category = "RealTimeImport|Sound")
		void stopSound(FString fileID, bool removeFromCache) {
		URealTimeImportSound::getRealTimeImportSound()->stopSound(fileID, removeFromCache);
	};

	/**
	* Stop all Wav files.
	*@param Determines whether the wav should remain in RAM if it exists.
	*/
	UFUNCTION(BlueprintCallable, Category = "RealTimeImport|Sound")
		void stopAllSounds(bool removeFromCache) {
		URealTimeImportSound::getRealTimeImportSound()->stopAllSounds(removeFromCache);
	};



	/**
	* The volume can be changed during playback.
	*@param fileID This ID is generated when starting playback.
	*@param volumeMultiplier Volume.
	*/
	UFUNCTION(BlueprintCallable, Category = "RealTimeImport|Sound")
		void changeVolumeSound(FString fileID, float volumeMultiplier = 1.0) {
		URealTimeImportSound::getRealTimeImportSound()->changeVolumeSound(fileID, volumeMultiplier);
	};



	UFUNCTION(BlueprintCallable, Category = "RealTimeImport|Sound")
		bool isSoundActive(FString fileID) {
		return URealTimeImportSound::getRealTimeImportSound()->isSoundActive(fileID);
	};



	UFUNCTION(BlueprintCallable, Category = "RealTimeImport|Sound")
		void getSoundInfo(ERTIDirectoryType directoryType, FString filePath, FString& name, FString& fileNameExtension, int64& fileSize, float& duration,
			int64& numChannels, int32& quality, int64& sampleDataSize, int64& sampleRate) {
		URealTimeImportSound::getRealTimeImportSound()->getSoundInfo(directoryType, filePath, name, fileNameExtension, fileSize, duration,
			numChannels, quality, sampleDataSize, sampleRate);
	};


	//UFUNCTION(BlueprintCallable, Category = "RealTimeImport|Sound")
	//	void changePlaySpeedWavSound(FString fileID, float playSpeed = 1.0);


	//For an easier readable C++ code some functions were moved to the URealTimeImportImage class. For compatibility reasons, however, the functions will remain accessible via the URealTimeImportBPLibrary.

	/**
	*Load .png,.jpg or bmp File. A UTexture2D object is returned. The file stays in RAM and is not loaded again. Beware of the file size of your files!!!
	*@param directoryType Absolute path or relative path.
	*@param filePath Path inclusive file.
	*/
	UFUNCTION(BlueprintCallable, Category = "RealTimeImport|Image")
		void LoadImageFile(ERTIDirectoryType directoryType, FString filePath, UTexture2D*& texture, FString& fileName, int32& errorCode, FString& errorMessage, TextureCompressionSettings compressionSettings = TextureCompressionSettings::TC_Default, bool SRGB = true, bool createMipMaps = false) {
		URealTimeImportImage::getRealTimeImportImage()->LoadImageFile(directoryType, filePath, texture, fileName, errorCode, errorMessage, compressionSettings, SRGB, createMipMaps);
	};

	/**
	*Load asynchronously .png,.jpg or bmp File. A UTexture2D object is returned int the Event. The file stays in RAM and is not loaded again. Beware of the file size of your files!!!
	*@param directoryType Absolute path or relative path.
	*@param filePath Path inclusive file.
	*@param eventID To identify different events, you can enter an ID here which will be output again in the matching event.
	*/
	UFUNCTION(BlueprintCallable, Category = "RealTimeImport|Image")
		URealTimeImportBPLibrary* LoadImageFileAsync(ERTIDirectoryType directoryType, FString filePath, FString eventID, TextureCompressionSettings compressionSettings = TextureCompressionSettings::TC_Default, bool SRGB = true, bool createMipMaps = false) {
		URealTimeImportImage::getRealTimeImportImage()->LoadImageFileAsync(directoryType, filePath, eventID, compressionSettings, SRGB, createMipMaps);
		return RealTimeImportBPLibrary;
	};

	/**
	*Creates an UTexture2D object from a byte array. The file stays in RAM and is not loaded again. Beware of the file size of your files!!!
	*@param name An individual name to cache the image better in RAM
	*@param bytes Image bytes
	*/
	UFUNCTION(BlueprintCallable, Category = "RealTimeImport|Image")
		void LoadImageBytes(FString name, TArray<uint8> bytes, UTexture2D*& texture, int32& errorCode, FString& errorMessage, TextureCompressionSettings compressionSettings = TextureCompressionSettings::TC_Default, bool SRGB = true, bool createMipMaps = false) {
		URealTimeImportImage::getRealTimeImportImage()->LoadImageBytes(name, bytes, texture, errorCode, errorMessage, compressionSettings, SRGB, createMipMaps);
	};

	/**
	*Creates asynchronously an UTexture2D object from a byte array. The file stays in RAM and is not loaded again. Beware of the file size of your files!!!
	*@param name An individual name to cache the image better in RAM
	*@param bytes Image bytes
	*@param eventID To identify different events, you can enter an ID here which will be output again in the matching event.
	*/
	UFUNCTION(BlueprintCallable, Category = "RealTimeImport|Image")
		URealTimeImportBPLibrary* LoadImageBytesAsync(FString name, TArray<uint8> bytes, FString eventID, TextureCompressionSettings compressionSettings = TextureCompressionSettings::TC_Default, bool SRGB = true, bool createMipMaps = false) {
		URealTimeImportImage::getRealTimeImportImage()->LoadImageBytesAsync(name, bytes, eventID, compressionSettings, SRGB, createMipMaps);
		return RealTimeImportBPLibrary;
	};

	/**
	*You can add more images as Mip Maps to a texture with this function. UE4 allows MipMaps only in certain sizes. The texture into which the MipMap is added must also follow these rules. The texture must be 4096x4096 or half that size (2048x2048) or half that size and so on down to 4x4. If you have a texture in 1024x1024 you can only add a MipMap in 512x512. If you want to add another MipMap the image must be 256x256 and so on.
	*@param texture The texture into which the MipMap will be added.
	*@param directoryType Absolute path or relative path.
	*@param filePath Path inclusive file.
	*/
	//is no longer needed
	//UFUNCTION(BlueprintCallable, Category = "RealTimeImport|Image")
	//	void LoadImageAsMipMap(UTexture2D* texture, ERTIDirectoryType directoryType, FString filePath, int32& errorCode, FString& errorMessage) {
	//	URealTimeImportImage::getRealTimeImportImage()->LoadImageAsMipMap(texture, directoryType, filePath, errorCode, errorMessage);
	//};

	UFUNCTION(BlueprintCallable, Category = "RealTimeImport|Image")
		void removeTextureFromCacheAndDestroyTexture(UTexture2D* texture) {
		URealTimeImportImage::getRealTimeImportImage()->removeTextureFromCacheAndDestroyTexture(texture);
	};

	UFUNCTION(BlueprintCallable, Category = "RealTimeImport|Image")
		void emptyTextureCacheAndDestroyTextures() {
		URealTimeImportImage::getRealTimeImportImage()->emptyTextureCacheAndDestroyTextures();
	};

	/**
	*Creates an image file from the TextureRenderTarget2D component and returns the bytes. The bytes can be saved as a file or sent with a different plugin.
	*@param textureRenderTarget2D
	*@param type PNG,JPG and BMP are possible
	*@param quality In percent. 100 = highest quality. 1 = lowest quality. BMP is not compressed. With PNG, UE4 doesn't seem to change much. The biggest difference can be seen with JPG.
	*/
	UFUNCTION(BlueprintCallable, Category = "RealTimeImport|Image")
		static TArray<uint8> textureRenderTarget2D_ToByteArray(UTextureRenderTarget2D* textureRenderTarget2D, ERTIExportImageType type, int32 quality = 100) {
		return URealTimeImportImage::textureRenderTarget2D_ToByteArray(textureRenderTarget2D, type, quality);
	};

	/**
	*Creates an image file from the Texture2D  and returns the bytes. The bytes can be saved as a file or sent with a different plugin.
	*@param texture2D
	*@param type PNG,JPG and BMP are possible
	*@param quality In percent. 100 = highest quality. 1 = lowest quality. BMP is not compressed. With PNG, UE4 doesn't seem to change much. The biggest difference can be seen with JPG.
	*/
	UFUNCTION(BlueprintCallable, Category = "RealTimeImport|Image")
		static TArray<uint8> texture2dToByteArray(UTexture2D* texture2D, ERTIExportImageType type, int32 quality = 100) {
		return URealTimeImportImage::texture2dToByteArray(texture2D, type, quality);
	};



	//UFUNCTION(BlueprintCallable, Category = "RealTimeImport|test")
	//	void testTexture(UTexture2D* texture, UTexture2D* texture2);

	//UFUNCTION(BlueprintCallable, Category = "RealTimeImport|test")
	//	void testMesh(TArray<FVector> vertices, TArray<FVector2D>UV0, TArray<int32> triangles);


	//For an easier readable C++ code some functions were moved to the URealTimeImportMesh class. For compatibility reasons, however, the functions will remain accessible via the URealTimeImportBPLibrary.


	/**
	*Reads a mesh file and creates arrays of vectors, normals, triangles and UVs to create a mesh in UE4.
	*@param fileType Format of the file. Currently only OBJ possible
	*@param directoryType Absolute path or relative path.
	*@param filePath Path inclusive file.
	*/
	UFUNCTION(BlueprintCallable, Category = "RealTimeImport|Mesh")
		void LoadMeshFile(ERTIMeshType fileType,
			ERTIDirectoryType directoryType,
			FString filePath,
			bool& success,
			FString& successMessage,
			TArray<FVector>& vertices,
			TArray<FVector>& normals,
			TArray<FVector2D>& UV0,
			TArray<struct FRTIMeshStruct>& meshStructs,
			bool autoDetectionNormalMap = true,
			bool useSRGB = true,
			bool createMipMaps = false) {

		URealTimeImportMesh::getRealTimeImportMesh()->LoadMeshFile(fileType, directoryType, filePath, success, successMessage, vertices, normals, UV0, meshStructs, autoDetectionNormalMap, useSRGB, createMipMaps);
	};


	UFUNCTION(BlueprintCallable, Category = "RealTimeImport|Mesh")
		static void breakMeshStruct(struct FRTIMeshStruct meshStruct,
			FString& meshName,
			TArray<int32>& triangles,
			struct FRTIMaterialStruct& materialStruct) {
		URealTimeImportMesh::getRealTimeImportMesh()->breakMeshStruct(meshStruct, meshName, triangles, materialStruct);
	};


	UFUNCTION(BlueprintCallable, Category = "RealTimeImport|Mesh")
		static void breakMaterialStruct(struct FRTIMaterialStruct materialData,
			bool& hasTexture,
			FString& textureName,
			TArray<struct FRTITextureStruct>& textures,
			FColor& ambient,
			FColor& diffuse,
			FColor& specular,
			float& specularExponent,
			float& dissolved,
			float& dissolvedInverted) {
		URealTimeImportMesh::getRealTimeImportMesh()->breakMaterialStruct(materialData, hasTexture, textureName, textures,
			ambient, diffuse, specular, specularExponent, dissolved, dissolvedInverted);
	};



	UFUNCTION(BlueprintCallable, Category = "RealTimeImport|Mesh")
		static void breakTextureStruct(struct FRTITextureStruct textureData,
			UTexture2D*& texture,
			ERTITextureType& textureType) {
		URealTimeImportMesh::getRealTimeImportMesh()->breakTextureStruct(textureData, texture, textureType);
	};

	//UFUNCTION(BlueprintCallable, Category = "RealTimeImport|Mesh")
	//	void test(AActor* meshActor);





private:


};