// Copyright 2018-2020 David Romanski(Socke). All Rights Reserved.
#pragma once

#include "Modules/ModuleManager.h"
#include "CoreMinimal.h"
#include "Runtime/Engine/Public/AudioDevice.h"
#include "AudioDecompress.h"
#include "Interfaces/IAudioFormat.h"
#include "Runtime/Engine/Public/VorbisAudioInfo.h"
#include "Runtime/Engine/Public/OpusAudioInfo.h"

#include "ImageUtils.h"
#include "Runtime/Core/Public/PixelFormat.h"
#include "Runtime/ImageWrapper/Public/IImageWrapper.h"
#include "Runtime/ImageWrapper/Public/IImageWrapperModule.h"
#include "Runtime/RenderCore/Public/RenderUtils.h"
#include "Runtime/RenderCore/Public/RenderCore.h"
#include "Runtime/Core/Public/HAL/PlatformFilemanager.h"
#include "Engine/Texture2D.h"
#include "Runtime/Core/Public/Misc/Base64.h"
#include "Runtime/Core/Public/HAL/RunnableThread.h"
#include "Runtime/Core/Public/Misc/FileHelper.h"
#include "Runtime/Core/Public/HAL/FileManager.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Runtime/Core/Public/Async/Async.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "RealTimeImport.generated.h"



UENUM(BlueprintType)
enum class ERTIDirectoryType : uint8
{
	E_gd	UMETA(DisplayName = "Game directory"),
	E_ad 	UMETA(DisplayName = "Absolute directory")
};

UENUM(BlueprintType)
enum class ERTISoundFileType : uint8
{
	E_wav	UMETA(DisplayName = "WAV"),
	E_ogg 	UMETA(DisplayName = "OGG")
};

UENUM(BlueprintType)
enum class ERTIExportImageType : uint8
{
	E_png 	UMETA(DisplayName = "PNG"),
	E_bmp 	UMETA(DisplayName = "BMP"),
	E_jpg 	UMETA(DisplayName = "JPG"),
	E_exr 	UMETA(DisplayName = "EXR")
};

UENUM(BlueprintType)
enum class ERTIMeshType : uint8
{
	E_obj	UMETA(DisplayName = "OBJ"),
	E_fbx	UMETA(DisplayName = "FBX Binary")
};


UENUM(BlueprintType)
enum class ERTITextureType : uint8
{
	E_none	UMETA(DisplayName = "none"),
	E_un	UMETA(DisplayName = "Unknown"),
	E_Kd	UMETA(DisplayName = "Diffuse"),
	E_Ks 	UMETA(DisplayName = "Specular"),
	E_Ns 	UMETA(DisplayName = "SpecularHighlightComponent"),
	E_d		UMETA(DisplayName = "Alpha"),
	E_bump 	UMETA(DisplayName = "BumpOrNormal"),
	E_disp 	UMETA(DisplayName = "Displacement"),
	E_decal	UMETA(DisplayName = "StencilDecal"),
	E_refl 	UMETA(DisplayName = "Reflection"),
	E_Pr 	UMETA(DisplayName = "Roughness"),
	E_Pm 	UMETA(DisplayName = "Metallic"),
	E_Ps 	UMETA(DisplayName = "Sheen"),
	E_Pc 	UMETA(DisplayName = "ClearcoatThickness"),
	E_Pcr 	UMETA(DisplayName = "ClearcoatRoughness"),
	E_Ke 	UMETA(DisplayName = "Emissive"),
	E_aniso	UMETA(DisplayName = "Anisotropy"),
	E_anisor 	UMETA(DisplayName = "AnisotropyRotation"),
	E_norm 	UMETA(DisplayName = "Normal"),
	E_RMA 	UMETA(DisplayName = "Microsoft_RMA")
};


UENUM(BlueprintType)
enum class ERTIFileType : uint8
{
	E_wav	UMETA(DisplayName = "WAV"),
	E_ogg	UMETA(DisplayName = "OGG"),
	E_png 	UMETA(DisplayName = "PNG"),
	E_bmp 	UMETA(DisplayName = "BMP"),
	E_jpg 	UMETA(DisplayName = "JPG"),
	E_exr 	UMETA(DisplayName = "EXR"),
	E_obj 	UMETA(DisplayName = "OBJ"),
	E_fbx 	UMETA(DisplayName = "FBX"),
	E_all 	UMETA(DisplayName = "ALL")
};



//UENUM()
//enum class ERTIFbxIDType : uint8
//{
//	E_none	UMETA(DisplayName = "none"),
//	E_model	UMETA(DisplayName = "model"),
//	E_geometry	UMETA(DisplayName = "geometry"),
//	E_material	UMETA(DisplayName = "material"),
//	E_texture	UMETA(DisplayName = "texture"),
//	E_video	UMETA(DisplayName = "video")
//};




USTRUCT(BlueprintType)
struct FRTITextureStruct
{
	GENERATED_USTRUCT_BODY()
	FString textureName = FString();
	UTexture2D* texture = nullptr;
	ERTITextureType textureType = ERTITextureType::E_none;

	int64 fbxID = 0;
	//TArray<uint8> fbxTextureData;
};

USTRUCT(BlueprintType)
struct FRTIMaterialStruct
{
	GENERATED_USTRUCT_BODY()

	bool isEmpty = true;
	FString textureName = FString();
	TMap<FString, struct FRTITextureStruct> textures;
	TArray<FString> texturesFileNames;
	FColor ambient = FColor();
	FColor diffuse = FColor();
	FColor specular = FColor();
	float specularExponent = 0;
	float dissolved = 0;
	float dissolvedInverted = 0;

	int64 fbxID = 0;
};


USTRUCT(BlueprintType)
struct FRTIMeshStruct
{
	GENERATED_USTRUCT_BODY()

		bool isDefaultStruct = false;

	FString meshName = FString();

	TArray<int32> triangles;

	bool hasMaterialData = false;
	struct FRTIMaterialStruct materialData = FRTIMaterialStruct();
};




struct FRTIWavPlayAsyncStruct
{
	const UObject* WorldContextObject = nullptr;
	ERTIDirectoryType directoryType = ERTIDirectoryType::E_gd;
	FString filePath = FString();
	float maxFileSize = 0;
	UAudioComponent* AudioComponent = nullptr;
	float volumeMultiplier = 1.0;
	float pitchMultiplier = 1.0;
	int32 startTime = 0;
	bool persistAcrossLevelTransition = false;


	FSoundQualityInfo qualityInfo = { 0 };
};


USTRUCT(BlueprintType)
struct FRTISoundDataStruct
{
	GENERATED_USTRUCT_BODY()
		float duration = 0;
	uint64 sampleRate = 0;
	uint64 numChannels = 0;
	int64 headerSize = 0;
	TArray<uint8> soundData;
	USoundWaveProcedural* soundWave = nullptr;
};


#ifndef __RealTimeImportBPLibrary
#define __RealTimeImportBPLibrary
#include "RealTimeImportBPLibrary.h"
#endif


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



class FRealTimeImportModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};