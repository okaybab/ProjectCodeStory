// Copyright 2018-2020 David Romanski(Socke). All Rights Reserved.

#pragma once

#define STB_IMAGE_IMPLEMENTATION
#define STBI_SUPPORT_ZLIB

#include "RealTimeImport.h"
#include "RealTimeImportBPLibrary.h"
//#include "ThirdParty/stb_image.h"
#include "Misc/MemoryReadStream.h"
#include "Misc/Compression.h"
#include "RealTimeImportMesh.generated.h"

UENUM()
enum class ERTIInformationType : uint8
{

	E_none	UMETA(DisplayName = "none"),
	E_ByPolygonVertex	UMETA(DisplayName = "ByPolygonVertex"), /*(or also ByVertice, as the Blender exporter writes.I think the author is spanish.)*/
	E_ByVertex 	UMETA(DisplayName = "ByVertex"),
	E_ByPolygon	UMETA(DisplayName = "ByPolygon"),
	E_ByEdge 	UMETA(DisplayName = "ByEdge"),
	E_AllSame 	UMETA(DisplayName = "AllSame")
};

UENUM()
enum class ERTIReferenceInformationType : uint8
{
	E_none	UMETA(DisplayName = "none"),
	E_Direct	UMETA(DisplayName = "Direct"),
	E_IndexToDirect	UMETA(DisplayName = "IndexToDirect")
};

USTRUCT()
struct FRTIFBXVideoStruct
{
	GENERATED_USTRUCT_BODY()

	FString filePath = FString();
	int64 fbxID = 0;
	TArray<uint8> fbxTextureData;
};

USTRUCT()
struct FRTIFBXTextureStruct
{
	GENERATED_USTRUCT_BODY()

	ERTITextureType textureType = ERTITextureType::E_none;
	int64 fbxID = 0;
	TMap<int64,struct FRTIFBXVideoStruct*> videoStructs;
};


USTRUCT()
struct FRTIFBXMaterialStruct
{
	GENERATED_USTRUCT_BODY()

	TArray<uint8> ambient;
	TArray<uint8> diffuse;
	TArray<uint8> specular;

	int64 fbxID = 0;
	TMap<int64, struct FRTIFBXTextureStruct*> textureStructs;
};


USTRUCT()
struct FRTIFBXGeometryStruct
{
	GENERATED_USTRUCT_BODY()

	TArray<FVector> vertices;
	TArray<int32> polygonVertexIndexes;
	TArray<FVector> normals;

	TArray<FVector2D> UV0;
	TArray<int32> UV0Indexes;

	TArray<int32> edges;

	int64 fbxID = 0;

	ERTIInformationType normalInformationType;
	ERTIReferenceInformationType normalReferenceInformationType;
	ERTIInformationType UVInformationType;
	ERTIReferenceInformationType UVReferenceInformationType;
};

USTRUCT()
struct FRTIFBXModelStruct
{
	GENERATED_USTRUCT_BODY()

	int64 fbxID = 0;
	TMap<int64, struct FRTIFBXMaterialStruct*> materialStructs;
	struct FRTIFBXGeometryStruct* geometryStruct = nullptr;
};




USTRUCT()
struct FRTIFBXConnections
{
	GENERATED_USTRUCT_BODY()

	TMap<int64, struct FRTIFBXModelStruct*> model;
	TMap<int64, struct FRTIFBXGeometryStruct*> geometry;
	TMap<int64, struct FRTIFBXMaterialStruct*> material;
	TMap<int64, struct FRTIFBXTextureStruct*> texture;
	TMap<int64, struct FRTIFBXVideoStruct*> video;

	//TMap<int64, int64> connectionMap;
	//TMap<int64, TArray<int64>> connectionMapRevers;
};



UCLASS(Blueprintable, BlueprintType)
class URealTimeImportMesh : public UObject
{
	GENERATED_UCLASS_BODY()

public:

	static URealTimeImportMesh* realTimeImportMesh;
	UFUNCTION()
		static URealTimeImportMesh* getRealTimeImportMesh();



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
		bool useSRGB = true, bool createMipMaps = false);


	static void breakMeshStruct(struct FRTIMeshStruct meshStruct,
		FString& meshName,
		TArray<int32>& triangles,
		struct FRTIMaterialStruct& materialStruct);





	static void breakMaterialStruct(struct FRTIMaterialStruct materialData,
		bool& hasTexture,
		FString& textureName,
		TArray<struct FRTITextureStruct>& textures,
		FColor& ambient,
		FColor& diffuse,
		FColor& specular,
		float& specularExponent,
		float& dissolved,
		float& dissolvedInverted);


	static void breakTextureStruct(struct FRTITextureStruct textureData,
		UTexture2D*& texture,
		ERTITextureType& textureType);


private:

	void LoadOBJFile(
		FString dir,
		bool& success,
		FString& successMessage,
		TArray<FVector>& vertices,
		TArray<FVector>& normals,
		TArray<FVector2D>& UV0,
		TArray<struct FRTIMeshStruct>& meshStructs,
		bool autoDetectionNormalMap,
		bool useSRGB,
		bool createMipMaps);

	void LoadFBXFile(
		FString dir,
		bool& success,
		FString& successMessage,
		TArray<FVector>& vertices,
		TArray<FVector>& normals,
		TArray<FVector2D>& UV0,
		TArray<struct FRTIMeshStruct>& meshStructs,
		bool autoDetectionNormalMap,
		bool useSRGB,
		bool createMipMaps);


	int32 parseFbxBinaryData(bool& success, FString& successMessage, TArray<uint8>& fbxFileData, uint32 dataPosition,
		 struct FRTIFBXConnections& fbxConnections , int32 fbxVersion);

	uint32 byteArrayToInt32(TArray<uint8>& data, uint32 start);
	int64 byteArrayToInt64(TArray<uint8>& data, uint32 start);
	TArray<int32> byteArrayToInt32Tarray(TArray<uint8>& data, uint32 start, uint32 size);
	FString byteArrayToFString(TArray<uint8>& data, uint32 start, uint32 size);
	TArray<uint8> cutByteArray(TArray<uint8>& original, uint32 start, uint32 size);
	double parseBytesToDouble(TArray<uint8>& data, uint32 start);
	TArray<uint8> uncompressFbxData(TArray<uint8>& data, uint32 start, uint32 size, uint32 compressedLen);

	ERTIInformationType informationTypeStringToEnum(FString informationType);
	ERTIReferenceInformationType referenceInformationTypeTypeStringToEnum(FString informationType);

	void createMaterialStructFromObj(FString mtlFilePath, TMap<FString, struct FRTIMaterialStruct>& materialStructs,
		bool autoDetectionNormalMap,
		bool useSRGB, bool createMipMaps);
};
