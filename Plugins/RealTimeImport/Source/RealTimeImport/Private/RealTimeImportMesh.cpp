// Copyright 2018-2020 David Romanski(Socke). All Rights Reserved.

#include "RealTimeImportMesh.h"


URealTimeImportMesh* URealTimeImportMesh::realTimeImportMesh;
URealTimeImportMesh::URealTimeImportMesh(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) {
	realTimeImportMesh = this;
}


URealTimeImportMesh* URealTimeImportMesh::getRealTimeImportMesh() {
	return realTimeImportMesh;
}



void URealTimeImportMesh::LoadMeshFile(ERTIMeshType fileType, ERTIDirectoryType directoryType, FString filePath, bool& success, FString& successMessage,
	TArray<FVector>& vertices,
	TArray<FVector>& normals,
	TArray<FVector2D>& UV0,
	TArray<struct FRTIMeshStruct>& meshStructs,
	bool autoDetectionNormalMap,
	bool useSRGB,
	bool createMipMaps) {

	FString dir;
	if (directoryType == ERTIDirectoryType::E_ad) {
		dir = FPaths::ConvertRelativePathToFull(filePath);
	}
	else {
		FString ProjectDir = FPaths::ProjectDir();
		dir = FPaths::ConvertRelativePathToFull(ProjectDir + filePath);
	}

	switch (fileType)
	{
	case ERTIMeshType::E_obj:

		LoadOBJFile(
			dir,
			success,
			successMessage,
			vertices,
			normals,
			UV0,
			meshStructs,
			autoDetectionNormalMap,
			useSRGB,
			createMipMaps);

		break;
	case ERTIMeshType::E_fbx:

		LoadFBXFile(
			dir,
			success,
			successMessage,
			vertices,
			normals,
			UV0,
			meshStructs,
			autoDetectionNormalMap,
			useSRGB,
			createMipMaps);

		break;
	}
}


void URealTimeImportMesh::LoadOBJFile(
	FString dir,
	bool& success,
	FString& successMessage,
	TArray<FVector>& vertices,
	TArray<FVector>& normals,
	TArray<FVector2D>& UV0,
	TArray<struct FRTIMeshStruct>& meshStructs,
	bool autoDetectionNormalMap,
	bool useSRGB,
	bool createMipMaps) {



	FString stringFileData;
	if (!FFileHelper::LoadFileToString(stringFileData, *dir)) {
		UE_LOG(LogTemp, Error, TEXT("RealTimeImportPlugin: Can't load file: %s"), *dir);
		success = false;
		successMessage = "Can't load file: " + dir;
		return;
	}

	TArray<FString> lines;
	int32 lineCount = stringFileData.ParseIntoArray(lines, TEXT("\n"), true);
	stringFileData.Empty();

	if (lineCount < 1) {
		UE_LOG(LogTemp, Error, TEXT("RealTimeImportPlugin: corrupted file (1): %s"), *dir);
		success = false;
		successMessage = "Corrupted file (1): " + dir;
		return;
	}



	struct FRTIMaterialStruct lastMaterialData;

	TMap<FString, struct FRTIMaterialStruct> materialStructsTemp;


	//we need a default meshstruct
	FRTIMeshStruct defaultMesh;
	defaultMesh.meshName = "RealTimeImport_No_Name";
	defaultMesh.isDefaultStruct = true;



	meshStructs.Add(defaultMesh);

	int faceMethod = -1;


	TArray<FVector> verticesLines;
	TArray<FVector2D> UV0Lines;
	TArray<FVector> normalsLines;

	TArray<FString> lineElements;
	TArray<FString> subElements;
	TArray<int32> tempVerticesArrayPosition;


	for (int32 i = 0; i < lineCount; i++) {
		FString line = lines[i].TrimStartAndEnd();
		if (line.StartsWith("#")) {
			continue;
		}



		//UE_LOG(LogTemp, Display, TEXT("RealTimeImportPlugin:_%s_"), *line);

		//objects
		if (line.StartsWith("o ")) {
			lineElements.Empty();
			int32 elementCount = line.ParseIntoArray(lineElements, TEXT(" "), true);
			if (elementCount >= 2) {
				//use the default mesh
				if (meshStructs.Last().isDefaultStruct) {
					meshStructs.Last().meshName = line.Replace(TEXT("o "), TEXT(""));
					meshStructs.Last().isDefaultStruct = false;
					/*	if (lastMaterialData.isEmpty == false) {
							meshStructs.Last().hasMaterialData = true;
							meshStructs.Last().materialData = lastMaterialData;
						}*/

				}
				else {
					FRTIMeshStruct meshStruct;
					meshStruct.meshName = lineElements[1];
					/*if (lastMaterialData.isEmpty == false) {
						meshStruct.hasMaterialData = true;
						meshStruct.materialData = lastMaterialData;
					}*/
					meshStructs.Add(meshStruct);
				}
				faceMethod = -1;
			}
			continue;
		}

		if (line.StartsWith("g ")) {
			lineElements.Empty();
			int32 elementCount = line.ParseIntoArray(lineElements, TEXT(" "), true);
			if (elementCount >= 2) {
				//use the default mesh
				if (meshStructs.Last().isDefaultStruct) {
					meshStructs.Last().meshName = line.Replace(TEXT("g "), TEXT(""));
					meshStructs.Last().isDefaultStruct = false;
					if (lastMaterialData.isEmpty == false) {
						meshStructs.Last().hasMaterialData = true;
						meshStructs.Last().materialData = lastMaterialData;
					}
				}
				else {
					FRTIMeshStruct meshStruct;
					meshStruct.meshName = lineElements[1];
					if (lastMaterialData.isEmpty == false) {
						meshStruct.hasMaterialData = true;
						meshStruct.materialData = lastMaterialData;
					}
					meshStructs.Add(meshStruct);
				}
				faceMethod = -1;
			}
			continue;
		}

		//mtl files
		if (line.StartsWith("mtllib ")) {
			lineElements.Empty();
			int32 elementCount = line.ParseIntoArray(lineElements, TEXT(" "), true);
			if (elementCount == 2) {
				createMaterialStructFromObj(dir.Replace(*FPaths::GetCleanFilename(dir), *lineElements[1]), materialStructsTemp, autoDetectionNormalMap, useSRGB, createMipMaps);
			}
			continue;
		}

		//vertices
		if (line.StartsWith("v ")) {
			lineElements.Empty();
			int32 elementCount = line.ParseIntoArray(lineElements, TEXT(" "), true);
			if (elementCount == 4) {
				verticesLines.Add(FVector(FCString::Atof(*lineElements[1]), FCString::Atof(*lineElements[2]), FCString::Atof(*lineElements[3])));
			}
			else {
				UE_LOG(LogTemp, Warning, TEXT("RealTimeImportPlugin:(%i) Vertices line corrupted: %s"), (i + 1), *line);
			}
			continue;
		}

		//normals
		if (line.StartsWith("vn ")) {
			lineElements.Empty();
			int32 elementCount = line.ParseIntoArray(lineElements, TEXT(" "), true);
			if (elementCount == 4) {
				normalsLines.Add(FVector(FCString::Atof(*lineElements[1]), FCString::Atof(*lineElements[2]), FCString::Atof(*lineElements[3])));
			}
			else {
				UE_LOG(LogTemp, Warning, TEXT("RealTimeImportPlugin:(%i) Normals line corrupted: %s"), (i + 1), *line);
			}
			continue;
		}

		//UV0
		if (line.StartsWith("vt ")) {
			lineElements.Empty();
			int32 elementCount = line.ParseIntoArray(lineElements, TEXT(" "), true);
			if (elementCount >= 3) {
				float y = FCString::Atof(*lineElements[2]);
				if (y > 0) {
					UV0Lines.Add(FVector2D(FCString::Atof(*lineElements[1]), 1 - y));
				}
				else {
					UV0Lines.Add(FVector2D(FCString::Atof(*lineElements[1]), -1 - y));
				}
			}
			else {
				UE_LOG(LogTemp, Warning, TEXT("RealTimeImportPlugin:(%i) UV line corrupted: %s"), (i + 1), *line);
			}
			continue;
		}


		//Material/Texture name for this group
		if (line.StartsWith("usemtl ")) {
			lineElements.Empty();
			int32 elementCount = line.ParseIntoArray(lineElements, TEXT(" "), true);
			if (elementCount >= 2) {
				if (materialStructsTemp.Find(lineElements[1]) != nullptr) {
					lastMaterialData = meshStructs.Last().materialData = *materialStructsTemp.Find(lineElements[1]);
				}
			}
			continue;
		}

		//faces, create the mesh structure
		if (line.StartsWith("f ")) {
			lineElements.Empty();
			int32 elementCount = line.ParseIntoArray(lineElements, TEXT(" "), true);

			//set face method
			if (faceMethod == -1 && elementCount > 3) {

				//UE_LOG(LogTemp, Warning, TEXT("RealTimeImportPlugin: Method search: %s"), *lineElements[1]);

				//method 1 = f v1 v2 v3 ....
				if (lineElements[1].Contains("/") == false) {
					faceMethod = 1;
				}
				else {
					//method 4 = f v1//vn1 v2//vn2 v3//vn3 ...
					if (lineElements[1].Contains("//")) {
						faceMethod = 4;
					}
					else {
						//method 2 = f v1/vt1 v2/vt2 v3/vt3 ...
						TArray<FString> tmp;
						if (lineElements[1].ParseIntoArray(tmp, TEXT("/"), true) == 2) {
							faceMethod = 2;
						}
						//method 3 = v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3 ...
						else {
							faceMethod = 3;
						}
					}
				}
				//UE_LOG(LogTemp, Warning, TEXT("RealTimeImportPlugin: Method: %i"), faceMethod);
			}

			//create
			if (elementCount > 3) {

				//struct FRTIMeshStruct& tmpMeshStruct = meshStructs.Last();
				tempVerticesArrayPosition.Empty();


				int32 subElementVal = -1;

				for (int32 j = 1; j < lineElements.Num(); j++) {
					subElements.Empty();

					switch (faceMethod)
					{
					case 1:
						subElementVal = FCString::Atoi(*lineElements[j]) - 1;
						if (subElementVal < 0) {
							//For negative values, the lines must be read backwards. If there are 100 lines and the value -100 appears here, line 1 is meant.
							subElementVal = (verticesLines.Num()) - (subElementVal * -1) + 1;
						}

						if (verticesLines.Num() > subElementVal) {
							vertices.Add(verticesLines[subElementVal]);
							tempVerticesArrayPosition.Add(vertices.Num() - 1);
						}
						break;
					case 2:
						elementCount = lineElements[j].ParseIntoArray(subElements, TEXT("/"), true);
						if (elementCount == 2) {
							subElementVal = FCString::Atoi(*subElements[0]) - 1;
							if (subElementVal < 0) {
								subElementVal = (verticesLines.Num()) - (subElementVal * -1) + 1;
							}

							if (verticesLines.Num() > subElementVal) {
								vertices.Add(verticesLines[subElementVal]);

								subElementVal = FCString::Atoi(*subElements[1]) - 1;
								if (subElementVal < 0) {
									subElementVal = (verticesLines.Num()) - (subElementVal * -1) + 1;
								}

								if (UV0Lines.Num() > subElementVal)
									UV0.Add(UV0Lines[subElementVal]);
								tempVerticesArrayPosition.Add(vertices.Num() - 1);
							}
						}
						break;
					case 3:
						elementCount = lineElements[j].ParseIntoArray(subElements, TEXT("/"), true);
						if (elementCount == 3) {
							subElementVal = FCString::Atoi(*subElements[0]) - 1;
							if (subElementVal < 0) {
								subElementVal = (verticesLines.Num()) - (subElementVal * -1) + 1;
							}

							if (verticesLines.Num() > subElementVal) {
								vertices.Add(verticesLines[subElementVal]);

								subElementVal = FCString::Atoi(*subElements[1]) - 1;
								if (subElementVal < 0) {
									subElementVal = (verticesLines.Num()) - (subElementVal * -1) + 1;
								}

								if (UV0Lines.Num() > subElementVal)
									UV0.Add(UV0Lines[subElementVal]);

								subElementVal = FCString::Atoi(*subElements[2]) - 1;
								if (subElementVal < 0) {
									subElementVal = (verticesLines.Num()) - (subElementVal * -1) + 1;
								}

								if (normalsLines.Num() > subElementVal)
									normals.Add(normalsLines[subElementVal]);
								tempVerticesArrayPosition.Add(vertices.Num() - 1);
							}
						}
						break;
					case 4:
						elementCount = lineElements[j].ParseIntoArray(subElements, TEXT("//"), true);
						if (elementCount == 2) {
							subElementVal = FCString::Atoi(*subElements[0]) - 1;
							if (subElementVal < 0) {
								subElementVal = (verticesLines.Num()) - (subElementVal * -1) + 1;
							}

							if (verticesLines.Num() > subElementVal) {
								vertices.Add(verticesLines[subElementVal]);
								subElementVal = FCString::Atoi(*subElements[1]) - 1;
								if (subElementVal < 0) {
									subElementVal = (verticesLines.Num()) - (subElementVal * -1) + 1;
								}

								if (normalsLines.Num() > subElementVal)
									normals.Add(normalsLines[subElementVal]);
								tempVerticesArrayPosition.Add(vertices.Num() - 1);
							}
						}
						break;
					}
				}



				struct FRTIMeshStruct& tmpMeshStruct = meshStructs.Last();

				//Create polygons in the UE4 way
				for (int32 k = 1; k < tempVerticesArrayPosition.Num() - 1; k++) {
					tmpMeshStruct.triangles.Add(tempVerticesArrayPosition[k + 1]);
					tmpMeshStruct.triangles.Add(tempVerticesArrayPosition[k]);
					tmpMeshStruct.triangles.Add(tempVerticesArrayPosition[0]);
				}
				tempVerticesArrayPosition.Empty();



			}
			else {
				UE_LOG(LogTemp, Warning, TEXT("RealTimeImportPlugin:(%i) Cannot process this line. Only polygons and quads are possible. (Method %i): %s"), (i + 1), faceMethod, *line);
			}
		}
	}


	if (vertices.Num() < 3) {
		UE_LOG(LogTemp, Error, TEXT("RealTimeImportPlugin: Less than 3 vertices found in file. Cancel: %s"), *dir);
		success = false;
		successMessage = "Less than 3 vertices found in file. Cancel: " + dir;
		return;
	}

	//if (triangles.Num() < 3) {
	//	UE_LOG(LogTemp, Error, TEXT("RealTimeImportPlugin: No triangles found in file. Cancel: %s"), *dir);
	//	return;
	//}

	//for (int32 i = 0; i < UV0.Num(); i++) {
	//	UE_LOG(LogTemp, Error, TEXT("%f|%f|%f  %f|%f"), vertices[i].X, vertices[i].Y, vertices[i].Z, UV0[i].X, UV0[i].Y);
	//}

	if (normals.Num() == 0) {
		UE_LOG(LogTemp, Warning, TEXT("RealTimeImportPlugin: No normals found in this file. Normals will be automatically created. This can lead to problems with the lighting.: %s"), *dir);
	}

	UE_LOG(LogTemp, Display, TEXT("RealTimeImportPlugin: Vertices: %i UV0: %i Normals: %i "), vertices.Num(), UV0.Num(), normals.Num());

	success = true;
	successMessage = "Vertices: " + FString::FromInt(vertices.Num()) + " UV0: " + FString::FromInt(UV0.Num()) + " Normals: " + FString::FromInt(normals.Num());

	lineElements.Empty();
	subElements.Empty();
	tempVerticesArrayPosition.Empty();
}

void URealTimeImportMesh::LoadFBXFile(FString dir, bool& success, FString& successMessage, TArray<FVector>& vertices, TArray<FVector>& normals,
	TArray<FVector2D>& UV0, TArray<struct FRTIMeshStruct>& meshStructs, bool autoDetectionNormalMap, bool useSRGB, bool createMipMaps){
	
	TArray<uint8> fbxFileData;
	if (!FFileHelper::LoadFileToArray(fbxFileData, *dir)) {
		UE_LOG(LogTemp, Error, TEXT("RealTimeImportPlugin: Can't load file: %s"), *dir);
		success = false;
		successMessage = "Can't load file: " + dir;
		return;
	}

	if (fbxFileData.Num() < 1024) {
		UE_LOG(LogTemp, Error, TEXT("RealTimeImportPlugin: FBX file corrupted (0). %s"), *dir);
		success = false;
		successMessage = "FBX file corrupted (0): " + dir;
		return;
	}




	//https://code.blender.org/2013/08/fbx-binary-file-format-specification/
	//https://banexdevblog.wordpress.com/2014/06/23/a-quick-tutorial-about-the-fbx-ascii-format/

	//fbx type 
	FString fbxType = byteArrayToFString(fbxFileData, 0, 20);

	if (fbxType.Equals("Kaydara FBX Binary  ") == false) {
		UE_LOG(LogTemp, Warning, TEXT("RealTimeImportPlugin: Only FBX files in binary format are supported. %s"), *dir);
		success = false;
		successMessage = "Only FBX files in binary format are supported: " + dir;
		return;
	}



	//fbx version
	int32 fbxVersion = byteArrayToInt32(fbxFileData, 23);

	if (fbxVersion != 7700 && fbxVersion != 7600 && fbxVersion != 7500 && fbxVersion != 7400 /*&& fbxVersion != 7300 && fbxVersion != 7200 && fbxVersion != 7100*/) {
		UE_LOG(LogTemp, Warning, TEXT("RealTimeImportPlugin: Only FBX files with version 7400, 7700 (7.4, 7.7) are supported. Your FBX version: %i %s"), fbxVersion, *dir);
		success = false;
		successMessage = "Only FBX files with version 7400, 7700 (7.4, 7.7) are supported. Your FBX version: " + FString::FromInt(fbxVersion);
		return;
	}


	UE_LOG(LogTemp, Display, TEXT("RealTimeImportPlugin: FBX (binary) Version: %i"), fbxVersion);

	struct FRTIFBXConnections fbxConnections;
	/*TArray<struct FRTIMeshStruct*> meshStructsRefs;*/
	uint32 dataPosition = 27;

	uint32 endData = fbxFileData.Num() - 4;
	do {
		dataPosition = parseFbxBinaryData(success, successMessage, fbxFileData, dataPosition, fbxConnections, fbxVersion);
	} while (dataPosition && success && dataPosition < endData);

	if (!success) {
		return;
	}


	//sort 
	for (auto& element : fbxConnections.model) {
		struct FRTIFBXModelStruct* modelStruct = element.Value;
		struct FRTIFBXGeometryStruct* geometryStruct = modelStruct->geometryStruct;
		if (geometryStruct == nullptr) {
			continue;
		}
		struct FRTIMeshStruct meshStruct;

		//normals
		switch (geometryStruct->normalInformationType)
		{
		case ERTIInformationType::E_ByPolygonVertex:
			normals.Append(geometryStruct->normals);
			break;
		case ERTIInformationType::E_ByVertex:
			for (int32 j = 0; j < geometryStruct->polygonVertexIndexes.Num(); j++) {
				if (geometryStruct->polygonVertexIndexes[j] < 0) {
					normals.Add(geometryStruct->normals[~geometryStruct->polygonVertexIndexes[j]]);
				}
				else {
					normals.Add(geometryStruct->normals[geometryStruct->polygonVertexIndexes[j]]);
				}
			}
			break;
		case ERTIInformationType::E_AllSame:
			UE_LOG(LogTemp, Warning, TEXT("RealTimeImportPlugin: The FBX information type 'AllSame' is not implemented. Normals are probably incorrect."));
			normals.Append(geometryStruct->normals);
			break;
		case ERTIInformationType::E_ByEdge:
			UE_LOG(LogTemp, Warning, TEXT("RealTimeImportPlugin: The FBX information type 'ByEdge' is not implemented. Normals are probably incorrect."));
			normals.Append(geometryStruct->normals);
			break;
		case ERTIInformationType::E_ByPolygon:
			UE_LOG(LogTemp, Warning, TEXT("RealTimeImportPlugin: The FBX information type 'ByPolygon' is not implemented. Normals are probably incorrect."));
			normals.Append(geometryStruct->normals);
			break;
		}

		//UVs
		switch (geometryStruct->UVReferenceInformationType)
		{
		case ERTIReferenceInformationType::E_Direct:
			UV0.Append(geometryStruct->UV0);
			break;
		case ERTIReferenceInformationType::E_IndexToDirect:
			for (int32 j = 0; j < geometryStruct->UV0Indexes.Num(); j++) {
				UV0.Add(geometryStruct->UV0[geometryStruct->UV0Indexes[j]]);
			}
			break;
		}

		//vertices
		TArray<uint32> tempVerticesArrayPosition;
		for (int32 j = 0; j < geometryStruct->polygonVertexIndexes.Num(); j++) {


			if (geometryStruct->polygonVertexIndexes[j] < 0) {
				vertices.Add(geometryStruct->vertices[~geometryStruct->polygonVertexIndexes[j]]);

				//tempVerticesArrayPosition.Add(~geometryStruct->polygonVertexIndexes[j]);
				tempVerticesArrayPosition.Add(vertices.Num() - 1);

				for (int32 k = 1; k < tempVerticesArrayPosition.Num() - 1; k++) {
					meshStruct.triangles.Add(tempVerticesArrayPosition[k + 1]);
					meshStruct.triangles.Add(tempVerticesArrayPosition[k]);
					meshStruct.triangles.Add(tempVerticesArrayPosition[0]);
				}
				tempVerticesArrayPosition.Empty();
			}
			else {

				//tempVerticesArrayPosition.Add(geometryStruct->polygonVertexIndexes[j]);

				vertices.Add(geometryStruct->vertices[geometryStruct->polygonVertexIndexes[j]]);
				tempVerticesArrayPosition.Add(vertices.Num() - 1);
			}
		}

		//textures
		//for (int32 i = 0; i < modelStruct->materialStructs.Num(); i++) {
		for (auto& materialElement : modelStruct->materialStructs) {
			if (modelStruct->geometryStruct == nullptr) {
				continue;
			}
			
			struct FRTIFBXMaterialStruct* materialStruct = materialElement.Value; //modelStruct->materialStructs[i];
			struct FRTIMaterialStruct meshMaterialStruct;

			if (materialStruct->diffuse.Num() == 3) {
				meshMaterialStruct.diffuse = FColor(materialStruct->diffuse[0], materialStruct->diffuse[1], materialStruct->diffuse[2]);
				meshStruct.materialData = meshMaterialStruct;
			}
			if (materialStruct->ambient.Num() == 3) {
				meshMaterialStruct.ambient = FColor(materialStruct->ambient[0], materialStruct->ambient[1], materialStruct->ambient[2]);
				meshStruct.materialData = meshMaterialStruct;
			}
			if (materialStruct->specular.Num() == 3) {
				meshMaterialStruct.specular = FColor(materialStruct->specular[0], materialStruct->specular[1], materialStruct->specular[2]);
				meshStruct.materialData = meshMaterialStruct;
			}

			//for (int32 j = 0; j < materialStruct->textureStructs.Num(); j++) {
			for (auto& textureElement : materialStruct->textureStructs) {
				struct FRTIFBXTextureStruct* textureStruct = textureElement.Value;//materialStruct->textureStructs[j];

				//for (int32 k = 0; k < textureStruct->videoStructs.Num(); k++) {
				for (auto& videoElement : textureStruct->videoStructs) {
					struct FRTIFBXVideoStruct* videoStruct = videoElement.Value;//textureStruct->videoStructs[k];
					if (videoStruct->fbxTextureData.Num() == 0) {
						UE_LOG(LogTemp, Warning, TEXT("RealTimeImportPlugin FBX: Empty Texturestruct: %s"),*videoStruct->filePath);
						continue;
					}
					struct FRTITextureStruct meshTextureStruct;
		
					meshTextureStruct.textureName = FPaths::GetCleanFilename(videoStruct->filePath);
					meshTextureStruct.textureType = textureStruct->textureType;
					

					TextureCompressionSettings compressionSettings = TextureCompressionSettings::TC_Default;
					if (textureStruct->textureType == ERTITextureType::E_bump) {
						compressionSettings = TextureCompressionSettings::TC_Normalmap;
					}


					/*FString dir = "D:/0Testtexturen/"+FGuid::NewGuid().ToString() + meshTextureStruct.textureName;
					FFileHelper::SaveArrayToFile(videoStruct->fbxTextureData,*dir);*/

					int32 errorCode = 0;
					FString errorMessage = FString();
					URealTimeImportImage::getRealTimeImportImage()->LoadImageBytes(meshTextureStruct.textureName, videoStruct->fbxTextureData, meshTextureStruct.texture, errorCode, errorMessage, compressionSettings, useSRGB, createMipMaps);
					


					if (errorCode != 0) {
						UE_LOG(LogTemp, Warning, TEXT("RealTimeImportPlugin FBX: Error %i on loading the texture: %s"), errorCode, *errorMessage);
					}
					else {
						if (meshStruct.hasMaterialData == false) {
							meshMaterialStruct.isEmpty = false;
							meshStruct.materialData = meshMaterialStruct;
							meshStruct.hasMaterialData = true;
						}

						meshStruct.materialData.textures.Add(meshTextureStruct.textureName + "_" + FString::FromInt(textureStruct->fbxID), meshTextureStruct);
					}
				}
			}
		}



		meshStructs.Add(meshStruct);
	}

	//clean
	fbxFileData.Empty();

	for (auto& element : fbxConnections.model) {
		element.Value->geometryStruct = nullptr;
		element.Value->materialStructs.Empty();
	}
	fbxConnections.model.Empty();

	for (auto& element : fbxConnections.geometry) {
		element.Value->normals.Empty();
		element.Value->polygonVertexIndexes.Empty();
		element.Value->UV0.Empty();
		element.Value->UV0Indexes.Empty();
		element.Value->vertices.Empty();
	}
	fbxConnections.geometry.Empty();

	for (auto& element : fbxConnections.material) {
		element.Value->textureStructs.Empty();
	}
	fbxConnections.material.Empty();


	for (auto& element : fbxConnections.texture) {
		element.Value->videoStructs.Empty();
	}
	fbxConnections.texture.Empty();

	for (auto& element : fbxConnections.video) {
		element.Value->fbxTextureData.Empty();
	}
	fbxConnections.video.Empty();



	if (vertices.Num() < 3) {
		UE_LOG(LogTemp, Error, TEXT("RealTimeImportPlugin: Less than 3 vertices found in file. Cancel: %s"), *dir);
		success = false;
		successMessage = "Less than 3 vertices found in file. Cancel: " + dir;
		return;
	}


	UE_LOG(LogTemp, Display, TEXT("RealTimeImportPlugin: Vertices: %i UV0: %i Normals: %i "), vertices.Num(), UV0.Num(), normals.Num());

	success = true;
	successMessage = "Vertices: " + FString::FromInt(vertices.Num()) + " UV0: " + FString::FromInt(UV0.Num()) + " Normals: " + FString::FromInt(normals.Num());

	//lineElements.Empty();
	//subElements.Empty();
	//tempVerticesArrayPosition.Empty();
}




int32 URealTimeImportMesh::parseFbxBinaryData(bool& success, FString& successMessage, TArray<uint8>& fbxFileData, uint32 dataPosition,
	 struct FRTIFBXConnections& fbxConnections, int32 fbxVersion){

	uint32 endOffset = byteArrayToInt32(fbxFileData, dataPosition);
	//UE_LOG(LogTemp, Error, TEXT("endOffset %i "), endOffset);

	FString name = FString();
	FString lastName = FString();
	TArray<FString> lastStringProperty;


	struct FRTIFBXGeometryStruct* geometryStruct = nullptr;
	struct FRTIFBXModelStruct* modelStruct = nullptr;
	struct FRTIFBXMaterialStruct* materialStruct = nullptr;
	struct FRTIFBXTextureStruct* textureStruct = nullptr;
	struct FRTIFBXVideoStruct* videoStruct = nullptr;

	//0 = normals
	//1 = uvs
	//2 = material
	//3 = connections
	int32 mode = -1;

	int64 lastFbxID = 0;

	int64 fbxIDKey = -1;
	int64 fbxIDVal = -1;

	int32 aditionalPosition_A = 4;
	int32 aditionalPosition_B = 12;
	int32 aditionalPosition_C = 13;

	if (fbxVersion == 7700 || fbxVersion == 7600 || fbxVersion == 7500) {
		aditionalPosition_A = 8;
		aditionalPosition_B = 24;
		aditionalPosition_C = 25;
	}


	//UE_LOG(LogTemp, Warning, TEXT("endOffset: %i"), endOffset);
	while ((dataPosition+ aditionalPosition_C) < endOffset) {
		//UE_LOG(LogTemp, Warning, TEXT("%i < %i"), dataPosition, endOffset);

		//ERTIFbxParseMode mode = ERTIFbxParseMode::E_None;

		uint32 numProperties = byteArrayToInt32(fbxFileData, dataPosition + aditionalPosition_A);
		//uint32 PropertyListLength = *(uint32 *)(s+8);
		uint8 nameLength = byteArrayToInt32(fbxFileData, dataPosition + aditionalPosition_B);


		

		//UE_LOG(LogTemp, Warning, TEXT("numProperties: %i"), numProperties);
		//UE_LOG(LogTemp, Warning, TEXT("nameLength: %i"), nameLength);

		//if (!numProperties && !nameLength) {
		//	dataPosition = endOffset;
		//	break;
		//}

		if (nameLength) {
			lastName = name;
			name = byteArrayToFString(fbxFileData, dataPosition + aditionalPosition_C, nameLength);
			//UE_LOG(LogTemp, Warning, TEXT("name: %s"), *name);

			//if (name.Equals("Geometry")) {
			//	FRTIMeshStruct meshstruct;
			//	meshStructs.Add(meshstruct);
			//}
			if (name.Equals("LayerElementNormal")) {
				mode = 0;
			}
			if (name.Equals("LayerElementUV")) {
				mode = 1;
			}
			if (name.Equals("LayerElementMaterial")) {
				mode = 2;
			}
			if (name.Equals("Connections")) {
				mode = 3;
			}

		}
		else {
			name = "";
		}

		
		dataPosition += aditionalPosition_C + nameLength;


		for (uint32 i = 0; i < numProperties; ++i) {
			uint8 type = byteArrayToInt32(fbxFileData, dataPosition++);
			//UE_LOG(LogTemp, Warning, TEXT("type: %i %c"), type, type);

			switch (type) {
			case 'Y': {
				/*p->Type = FBX_PROPERTY_S16;
				p->Size = 2;
				p->Data = s;*/
				dataPosition += 2;
				break;
			}
			case 'C': {
				/*p->Type = FBX_PROPERTY_BOOL;
				p->Size = 1;
				p->Data = s;*/
				++dataPosition;
				break;
			}
			case 'I': {
				/*p->Type = FBX_PROPERTY_S32;
				p->Size = 4;
				p->Data = s;*/
				dataPosition += 4;
				break;
			}
			case 'F': {
				/*p->Type = FBX_PROPERTY_F32;
				p->Size = 4;
				p->Data = s;*/
				dataPosition += 4;
				break;
			}
			case 'D': {
				/*p->Type = FBX_PROPERTY_F64;
				p->Size = 8;
				p->Data = s;*/

				if (lastStringProperty.Num() > 3 && materialStruct != nullptr) {
					int32 lastP = lastStringProperty.Num()-1;
					if (lastStringProperty[lastP].Equals("A")) {
						if (lastStringProperty[lastP-1].Equals("Color")) {
							if (lastStringProperty[lastP - 2].Equals("DiffuseColor")) {
								materialStruct->diffuse.Add((uint8)((float)parseBytesToDouble(fbxFileData, dataPosition) * 255));
							}
							if (lastStringProperty[lastP - 2].Equals("AmbientColor")) {
								materialStruct->ambient.Add((uint8)((float)parseBytesToDouble(fbxFileData, dataPosition) * 255));
							}
							if (lastStringProperty[lastP - 2].Equals("SpecularColor")) {
								materialStruct->specular.Add((uint8)((float)parseBytesToDouble(fbxFileData, dataPosition) * 255));
							}
						}
					}
				}


				dataPosition += 8;
				break;
			}
			case 'L': {
				/*p->Type = FBX_PROPERTY_S64;
				p->Size = 8;
				p->Data = s;*/


				

				//connections
				if (mode == 3) {
					if (fbxIDKey == -1) {
						fbxIDKey = byteArrayToInt64(fbxFileData, dataPosition);
					}
					else {
						if (fbxIDVal == -1) {
							fbxIDVal = byteArrayToInt64(fbxFileData, dataPosition);

							if (fbxConnections.video.Find(fbxIDKey) != nullptr) {
								videoStruct = *fbxConnections.video.Find(fbxIDKey);
								if (fbxConnections.texture.Find(fbxIDVal) != nullptr) {
									textureStruct = *fbxConnections.texture.Find(fbxIDVal);
									textureStruct->videoStructs.Add(videoStruct->fbxID,videoStruct);
								}
							}
							else {
								if (fbxConnections.texture.Find(fbxIDKey) != nullptr) {
									textureStruct = *fbxConnections.texture.Find(fbxIDKey);
									if (fbxConnections.material.Find(fbxIDVal) != nullptr) {
										materialStruct = *fbxConnections.material.Find(fbxIDVal);
										materialStruct->textureStructs.Add(textureStruct->fbxID,textureStruct);
									}
								}
								else {
									if (fbxConnections.material.Find(fbxIDKey) != nullptr) {
										materialStruct = *fbxConnections.material.Find(fbxIDKey);
										if (fbxConnections.model.Find(fbxIDVal) != nullptr) {
											modelStruct = *fbxConnections.model.Find(fbxIDVal);
											modelStruct->materialStructs.Add(materialStruct->fbxID,materialStruct);
										}
									}
									else {
										if (fbxConnections.geometry.Find(fbxIDKey) != nullptr) {
											geometryStruct = *fbxConnections.geometry.Find(fbxIDKey);
											if (fbxConnections.model.Find(fbxIDVal) != nullptr) {
												modelStruct = *fbxConnections.model.Find(fbxIDVal);
												modelStruct->geometryStruct = geometryStruct;
											}
										}
									}
								}
							}

							//fbxConnections.connectionMap.Add(fbxIDKey, fbxIDVal);
							//
							//if (fbxConnections.connectionMapRevers.Find(fbxIDVal) != nullptr) {
							//	(*fbxConnections.connectionMapRevers.Find(fbxIDVal)).Add(fbxIDKey);
							//}
							//else {
							//	TArray<int64> a;
							//	a.Add(fbxIDKey);
							//	fbxConnections.connectionMapRevers.Add(fbxIDVal, a);
							//}
							//UE_LOG(LogTemp, Error, TEXT(" Connection: Key:%lld Val:%lld"), fbxIDKey, fbxIDVal);
							fbxIDKey = -1;
							fbxIDVal = -1;
						}
					}
				}
				else {
					lastFbxID = byteArrayToInt64(fbxFileData, dataPosition);

					if (name.Equals("Geometry")) {
						geometryStruct = new FRTIFBXGeometryStruct();
						geometryStruct->fbxID = lastFbxID;
						//meshStructs.Add(meshStruct);
						fbxConnections.geometry.Add(lastFbxID, geometryStruct);
						//UE_LOG(LogTemp, Error, TEXT("FBX_PROPERTY_S64 Geometry :%lld "), lastFbxID);
					}
					if (name.Equals("Model")) {
						modelStruct = new FRTIFBXModelStruct();
						modelStruct->fbxID = lastFbxID;
						fbxConnections.model.Add(lastFbxID, modelStruct);
						//UE_LOG(LogTemp, Error, TEXT("FBX_PROPERTY_S64 Model :%lld "), lastFbxID);
					}
					if (name.Equals("Material")) {
						materialStruct = new FRTIFBXMaterialStruct();
						materialStruct->fbxID = lastFbxID;
						fbxConnections.material.Add(lastFbxID, materialStruct);
						//UE_LOG(LogTemp, Error, TEXT("FBX_PROPERTY_S64 Material :%lld "), lastFbxID);
					}
					if (name.Equals("Texture")) {
						textureStruct = new FRTIFBXTextureStruct();
						textureStruct->fbxID = lastFbxID;
						fbxConnections.texture.Add(lastFbxID, textureStruct);
						//UE_LOG(LogTemp, Error, TEXT("FBX_PROPERTY_S64 Texture :%lld "), lastFbxID);
					}
					if (name.Equals("Video")) {
						videoStruct = new FRTIFBXVideoStruct();
						videoStruct->fbxID = lastFbxID;
						fbxConnections.video.Add(lastFbxID, videoStruct);
						//UE_LOG(LogTemp, Error, TEXT("FBX_PROPERTY_S64 Video :%lld "), lastFbxID);
					}
				}





				dataPosition += 8;
				break;
			}
			case 'f': {
				/*uint32 ArrayLen = *(uint32*)s;
				uint32 Encoding = *(uint32*)(s + 4);
				uint32 CompressedLen = *(uint32*)(s + 8);*/

				uint32 ArrayLen = byteArrayToInt32(fbxFileData, dataPosition);
				uint32 Encoding = byteArrayToInt32(fbxFileData, dataPosition + 4);
				uint32 CompressedLen = byteArrayToInt32(fbxFileData, dataPosition + 8);

				if (Encoding) {
	/*				p->Type = FBX_PROPERTY_F32_ARRAY;
					p->Size = ArrayLen * 4;
					p->CompressedSize = CompressedLen;
					p->Compressed = true;
					p->Data = s + 12;*/
					dataPosition += 12 + CompressedLen;
				}
				else {
					//p->Type = FBX_PROPERTY_F32_ARRAY;
					//p->Size = ArrayLen * 4;
					//p->Data = s + 12;
					dataPosition += 12 + (ArrayLen * 4);
				}
				break;
			}
			case 'd': {
				/*uint32 ArrayLen = *(uint32*)s;
				uint32 Encoding = *(uint32*)(s + 4);
				uint32 CompressedLen = *(uint32*)(s + 8);*/

				uint32 arrayLen = byteArrayToInt32(fbxFileData, dataPosition);
				uint32 encoding = byteArrayToInt32(fbxFileData, dataPosition + 4);
				uint32 compressedLen = byteArrayToInt32(fbxFileData, dataPosition + 8);

				if (encoding && geometryStruct != nullptr) {
					if (geometryStruct != nullptr) {
						if (name.Equals("Vertices")) {
							//UE_LOG(LogTemp, Error, TEXT(" vertices (encoded): %i %i"), arrayLen * 8, compressedLen);
							//p->Type = FBX_PROPERTY_F64_ARRAY;
							//p->Size = arrayLen * 8;
							//p->CompressedSize = CompressedLen;
							//p->Compressed = true;
							//p->Data = s + 12;				


							TArray<uint8> uncompressedContent = uncompressFbxData(fbxFileData, dataPosition + 12, arrayLen * 8, compressedLen);

							uint32 j = 0;
							while (j < arrayLen) {
								FVector vec = FVector();
								vec.X = (float)parseBytesToDouble(uncompressedContent, (j * 8));
								j++;
								vec.Y = (float)parseBytesToDouble(uncompressedContent, (j * 8));
								j++;
								vec.Z = (float)parseBytesToDouble(uncompressedContent, (j * 8));
								j++;
								geometryStruct->vertices.Add(vec);
							}
						}
						if (name.Equals("Normals")) {
							//UE_LOG(LogTemp, Error, TEXT(" normals (encoded): %i %i"), arrayLen * 8, compressedLen);

							TArray<uint8> uncompressedContent = uncompressFbxData(fbxFileData, dataPosition + 12, arrayLen * 8, compressedLen);

							uint32 j = 0;
							while (j < arrayLen) {
								FVector vec = FVector();
								vec.X = (float)parseBytesToDouble(uncompressedContent, (j * 8));
								j++;
								vec.Y = (float)parseBytesToDouble(uncompressedContent, (j * 8));
								j++;
								vec.Z = (float)parseBytesToDouble(uncompressedContent, (j * 8));
								j++;
								geometryStruct->normals.Add(vec);
							}
						}
						//only one UVLayer supported for now
						if (name.Equals("UV") && geometryStruct->UV0.Num() == 0) {
							//UE_LOG(LogTemp, Error, TEXT(" UV (encoded): %i %i"), arrayLen * 8, compressedLen);

							TArray<uint8> uncompressedContent = uncompressFbxData(fbxFileData, dataPosition + 12, arrayLen * 8, compressedLen);

							uint32 j = 0;
							while (j < arrayLen) {

								FVector2D vec = FVector2D();
								vec.X = (float)parseBytesToDouble(uncompressedContent, (j * 8));
								j++;
								vec.Y = (float)parseBytesToDouble(uncompressedContent, (j * 8));
								if (vec.Y > 0) {
									vec.Y = 1 - vec.Y;
								}
								else {
									vec.Y = -1 - vec.Y;
								}
								j++;
								geometryStruct->UV0.Add(vec);
							}
						}
					}

					dataPosition += 12 + compressedLen;
				}
				else {
					if (geometryStruct != nullptr) {
						if (name.Equals("Vertices")) {
							//UE_LOG(LogTemp, Error, TEXT(" vertices (plain): %i"), arrayLen * 8);

							uint32 j = 0;
							while (j < arrayLen) {
								FVector vec = FVector();
								vec.X = (float)parseBytesToDouble(fbxFileData, dataPosition + 12 + (j * 8));
								j++;
								vec.Y = (float)parseBytesToDouble(fbxFileData, dataPosition + 12 + (j * 8));
								j++;
								vec.Z = (float)parseBytesToDouble(fbxFileData, dataPosition + 12 + (j * 8));
								j++;
								geometryStruct->vertices.Add(vec);
							}
						}
						if (name.Equals("Normals")) {
							//UE_LOG(LogTemp, Error, TEXT(" normals (plain): %i"), arrayLen * 8);

							uint32 j = 0;
							while (j < arrayLen) {
								FVector vec = FVector();
								vec.X = (float)parseBytesToDouble(fbxFileData, dataPosition + 12 + (j * 8));
								j++;
								vec.Y = (float)parseBytesToDouble(fbxFileData, dataPosition + 12 + (j * 8));
								j++;
								vec.Z = (float)parseBytesToDouble(fbxFileData, dataPosition + 12 + (j * 8));
								j++;
								geometryStruct->normals.Add(vec);
							}
						}
						//only one UVLayer supported for now
						if (name.Equals("UV") && geometryStruct->UV0.Num() == 0) {
							//UE_LOG(LogTemp, Error, TEXT(" UV (plain): %i"), arrayLen * 8);

							uint32 j = 0;
							while (j < arrayLen) {
								FVector2D vec = FVector2D();
								vec.X = (float)parseBytesToDouble(fbxFileData, dataPosition + 12 + (j * 8));
								j++;
								vec.Y = (float)parseBytesToDouble(fbxFileData, dataPosition + 12 + (j * 8));
								if (vec.Y > 0) {
									vec.Y = 1 - vec.Y;
								}
								else {
									vec.Y = -1 - vec.Y;
								}

								j++;
								geometryStruct->UV0.Add(vec);
							}
						}
					}
					//p->Type = FBX_PROPERTY_F64_ARRAY;
					//p->Size = arrayLen * 8;
					//p->Data = s + 12;
					dataPosition += 12 + (arrayLen * 8);
				}

				break;
			}
			case 'i': {
					
				uint32 arrayLen = byteArrayToInt32(fbxFileData, dataPosition);
				uint32 encoding = byteArrayToInt32(fbxFileData, dataPosition + 4);
				uint32 compressedLen = byteArrayToInt32(fbxFileData, dataPosition + 8);

				if (encoding) {
					if (geometryStruct != nullptr) {
						if (name.Equals("PolygonVertexIndex")) {
							//UE_LOG(LogTemp, Error, TEXT(" PolygonVertexIndex (encoded): %i %i"), arrayLen * 4, compressedLen);

							TArray<uint8> uncompressedContent = uncompressFbxData(fbxFileData, dataPosition + 12, arrayLen * 4, compressedLen);

							for (uint32 j = 0; j < arrayLen; j++) {
								geometryStruct->polygonVertexIndexes.Add(byteArrayToInt32(uncompressedContent, (j * 4)));
							}
						}
						//only one UVLayer supported for now
						if (name.Equals("UVIndex") && geometryStruct->UV0Indexes.Num() == 0) {
							//UE_LOG(LogTemp, Error, TEXT(" UVIndex (encoded): %i %i"), arrayLen * 4, compressedLen);

							TArray<uint8> uncompressedContent = uncompressFbxData(fbxFileData, dataPosition + 12, arrayLen * 4, compressedLen);

							for (uint32 j = 0; j < arrayLen; j++) {
								geometryStruct->UV0Indexes.Add(byteArrayToInt32(uncompressedContent, (j * 4)));
							}
						}
						//if (name.Equals("Edges") && geometryStruct->UV0Indexes.Num() == 0) {
						//	//UE_LOG(LogTemp, Error, TEXT(" Edges (encoded): %i %i"), arrayLen * 4, compressedLen);

						//	TArray<uint8> uncompressedContent = uncompressFbxData(fbxFileData, dataPosition + 12, arrayLen * 4, compressedLen);

						//	for (uint32 j = 0; j < arrayLen; j++) {
						//		geometryStruct->edges.Add(byteArrayToInt32(uncompressedContent, (j * 4)));
						//	}
						//}

						if (name.Equals("Materials")) {
							//UE_LOG(LogTemp, Error, TEXT(" Materials (encoded): %i %i"), arrayLen * 4, compressedLen);

							//TArray<uint8> uncompressedContent = uncompressFbxData(fbxFileData, dataPosition + 12, arrayLen * 4, compressedLen);

							//for (uint32 j = 0; j < arrayLen; j++) {
							//	UE_LOG(LogTemp, Warning, TEXT("%i"), byteArrayToInt32(uncompressedContent, (j * 4)));
							//}
						}
					}

					dataPosition += 12 + compressedLen;
				}
				else {
					if (geometryStruct != nullptr) {
						if (name.Equals("PolygonVertexIndex")) {
							//UE_LOG(LogTemp, Error, TEXT(" PolygonVertexIndex (plain): %i"), arrayLen);

							for (uint32 j = 0; j < arrayLen; j++) {
								geometryStruct->polygonVertexIndexes.Add(byteArrayToInt32(fbxFileData, dataPosition + 12 + (j * 4)));
							}
						}

						//only one UVLayer supported for now
						if (name.Equals("UVIndex") && geometryStruct->UV0Indexes.Num() == 0) {
							//UE_LOG(LogTemp, Error, TEXT(" UVIndex (plain): %i"), arrayLen);

							for (uint32 j = 0; j < arrayLen; j++) {
								geometryStruct->UV0Indexes.Add(byteArrayToInt32(fbxFileData, dataPosition + 12 + (j * 4)));
							}
						}

						//if (name.Equals("Edges") && geometryStruct->UV0Indexes.Num() == 0) {
						//	//UE_LOG(LogTemp, Error, TEXT(" Edges (plain): %i"), arrayLen);

						//	for (uint32 j = 0; j < arrayLen; j++) {
						//		geometryStruct->edges.Add(byteArrayToInt32(fbxFileData, dataPosition + 12 + (j * 4)));
						//	}
						//}

						if (name.Equals("Materials")) {
							//UE_LOG(LogTemp, Error, TEXT(" Materials (plain): %i"), arrayLen);

							//for (uint32 j = 0; j < arrayLen; j++) {
							//	UE_LOG(LogTemp, Warning, TEXT("%i"), byteArrayToInt32(fbxFileData, dataPosition + 12 + (j * 4)));
							//}
						}
					}

					

					dataPosition += 12 + (arrayLen * 4);
				}

				break;
			}

			case 'l': {
				/*uint32 ArrayLen = *(uint32*)s;
				uint32 Encoding = *(uint32*)(s + 4);
				uint32 CompressedLen = *(uint32*)(s + 8);*/

				uint32 ArrayLen = byteArrayToInt32(fbxFileData, dataPosition);
				uint32 Encoding = byteArrayToInt32(fbxFileData, dataPosition + 4);
				uint32 CompressedLen = byteArrayToInt32(fbxFileData, dataPosition + 8);

				if (Encoding) {
					/*p->Type = FBX_PROPERTY_S64_ARRAY;
					p->Size = ArrayLen * 8;
					p->CompressedSize = CompressedLen;
					p->Compressed = true;
					p->Data = s + 12;*/
					dataPosition += 12 + CompressedLen;
				}
				else {
					/*p->Type = FBX_PROPERTY_S64_ARRAY;
					p->Size = ArrayLen * 8;
					p->Data = s + 12;*/
					dataPosition += 12 + (ArrayLen * 8);
				}
				break;
			}
			case 'b': {
				/*uint32 ArrayLen = *(uint32*)s;
				uint32 Encoding = *(uint32*)(s + 4);
				uint32 CompressedLen = *(uint32*)(s + 8);*/

				uint32 ArrayLen = byteArrayToInt32(fbxFileData, dataPosition);
				uint32 Encoding = byteArrayToInt32(fbxFileData, dataPosition + 4);
				uint32 CompressedLen = byteArrayToInt32(fbxFileData, dataPosition + 8);

				if (Encoding) {
					/*p->Type = FBX_PROPERTY_BOOL_ARRAY;
					p->Size = ArrayLen * 1;
					p->CompressedSize = CompressedLen;
					p->Compressed = true;
					p->Data = s + 12;*/
					dataPosition += 12 + CompressedLen;
				}
				else {
					/*p->Type = FBX_PROPERTY_BOOL_ARRAY;
					p->Size = ArrayLen * 1;
					p->Data = s + 12;*/
					dataPosition += 12 + (ArrayLen * 1);
				}
				break;
			}
			case 'S': {
				/*uint32 Len = *(uint32*)s;*/

				uint32 Len = byteArrayToInt32(fbxFileData, dataPosition);

				if (Len) {
					FString n = byteArrayToFString(fbxFileData, dataPosition + 4, Len);
					lastStringProperty.Add(n);
					//UE_LOG(LogTemp, Error, TEXT("FBX_PROPERTY_STRING: %s"), *n);

					if (mode == 3 ) {
						if (n.Equals("OO") || n.Equals("OP")) {
							fbxIDKey = -1;
							fbxIDVal = -1;
						}
						//if (n.Equals("OP")) {
							if (textureStruct != nullptr) {
								if (n.Equals("DiffuseColor") || n.Equals("Maya|base") || n.Equals("Maya|DiffuseTexture") || n.Equals("Maya|baseColor") ||
									n.Equals("Maya|baseColor|file") || n.Equals("3dsMax|Parameters|base_color_map") || n.Equals("Maya|TEX_color_map|file") || 
									n.Equals("Maya|TEX_color_map")) {
									textureStruct->textureType = ERTITextureType::E_Kd;
								}
								else {
									if (n.Equals("NormalMap") || n.Equals("Bump") || n.Equals("3dsMax|Parameters|bump_map") || n.Equals("Maya|NormalTexture") ||
										n.Equals("Maya|normalCamera") || n.Equals("Maya|normalCamera|file") || n.Equals("Maya|TEX_normal_map") ||
										n.Equals("Maya|TEX_normal_map|file")) {
										textureStruct->textureType = ERTITextureType::E_bump;
									}
									else {
										if (n.Equals("SpecularColor") || n.Equals("Maya|specularColor") || n.Equals("Maya|SpecularTexture") ||
											n.Equals("Maya|SpecularTexture|file") || n.Equals("ShininessExponent") || n.Equals("ReflectionFactor")) {
											textureStruct->textureType = ERTITextureType::E_Ks;
										}
										else {
											if (n.Equals("EmissiveColor") || n.Equals("EmissiveFactor") || n.Equals("Maya|emissionColor") || 
												n.Equals("Maya|emissionColor|file") || n.Equals("3dsMax|Parameters|emission_map") ||
												n.Equals("Maya|TEX_emissive_map") || n.Equals("Maya|TEX_emissive_map|file")  ) {
												textureStruct->textureType = ERTITextureType::E_Ke;
											}
											else {
												if (n.Equals("Maya|metalness") || n.Equals("Maya|metalness|file") || n.Equals("3dsMax|Parameters|metalness_map") || 
													n.Equals("Maya|TEX_metallic_map") || n.Equals("Maya|TEX_metallic_map|file")) {
													textureStruct->textureType = ERTITextureType::E_Pm;
												}
												else {
													if (n.Equals("Maya|specularRoughness") || n.Equals("3dsMax|Parameters|roughness_map") || n.Equals("Maya|TEX_roughness_map") || n.Equals("Maya|TEX_roughness_map|file")) {
														textureStruct->textureType = ERTITextureType::E_Pr;
													}
												}
											}
										}
									}
									
								}
								
							}
						//}
					}


					//if (meshStructs.Num() && meshStructs.Last().meshName.IsEmpty()) {
						/*FString n = byteArrayToFString(fbxFileData, dataPosition + 4, Len);
						UE_LOG(LogTemp, Error, TEXT("FBX_PROPERTY_STRING: %s"), *n);*/
						//meshStructs.Last().meshName = n;
					//}
					if (geometryStruct != nullptr) {
						if (name.Equals("MappingInformationType")) {
							switch (mode) {
							case 0:
								geometryStruct->normalInformationType = informationTypeStringToEnum(n);
								break;
							case 1:
								geometryStruct->UVInformationType = informationTypeStringToEnum(n);
								break;
							}
						}


						if (name.Equals("ReferenceInformationType")) {
							switch (mode) {
							case 0:
								geometryStruct->normalReferenceInformationType = referenceInformationTypeTypeStringToEnum(n);
								break;
							case 1:
								geometryStruct->UVReferenceInformationType = referenceInformationTypeTypeStringToEnum(n);
								break;
							}
						}
					}
					if (videoStruct != nullptr) {
						if (name.ToLower().Equals("filename") || name.Equals("RelativeFilename")) {
							FString filePath = byteArrayToFString(fbxFileData, dataPosition + 4, Len);
							ERTIDirectoryType dirtype;
							if (name.ToLower().Equals("filename")) {
								dirtype = ERTIDirectoryType::E_ad;
							}
							else {
								dirtype = ERTIDirectoryType::E_gd;
							}
							filePath = UFileFunctionsRealTimeImport::getCleanDirectory(dirtype, filePath);
							videoStruct->filePath = filePath;
							if (FPaths::FileExists(filePath)) {
								if (videoStruct->fbxTextureData.Num() == 0) {
									FFileHelper::LoadFileToArray(videoStruct->fbxTextureData, *filePath);
									videoStruct->filePath = filePath;
								}
							}
						}
					}
				}

				/*p->Type = FBX_PROPERTY_STRING;
				p->Size = Len;
				p->Data = s + 4;*/
				dataPosition += 4 + Len;
				break;
			}
			case 'R': {
				/*uint32 Len = *(uint32*)s;*/

				uint32 Len = byteArrayToInt32(fbxFileData, dataPosition);

				//UE_LOG(LogTemp, Error, TEXT("FBX_PROPERTY_RAW: %i"), Len);

				if (Len > 100) {
					//if (fbxConnections.texture.Find(lastFbxID) != nullptr) {
					//	struct FRTITextureStruct* ts = fbxConnections.texture.Find(lastFbxID);
					//	if (ts->fbxTextureData.Num() == 0) {
					//		ts->fbxTextureData.AddUninitialized(Len);
					//		FMemory::Memcpy(ts->fbxTextureData.GetData(), fbxFileData.GetData() + dataPosition + 4, Len);
					//	}					
					//}
					if (videoStruct != nullptr) {
						if (videoStruct->fbxTextureData.Num() == 0) {
							videoStruct->fbxTextureData.AddUninitialized(Len);
							FMemory::Memcpy(videoStruct->fbxTextureData.GetData(), fbxFileData.GetData() + dataPosition + 4, Len);
						}
					}
				}



				//TArray<uint8> rawData;
				//rawData.AddUninitialized(Len);
				//FMemory::Memcpy(rawData.GetData(), fbxFileData.GetData() + dataPosition + 4, Len);
				//FString dir = "D:/"+FGuid::NewGuid().ToString();
				//FFileHelper::SaveArrayToFile(rawData,*dir);
			

				/*p->Type = FBX_PROPERTY_RAW;
				p->Size = Len;
				p->Data = s + 4;*/
				dataPosition += 4 + Len;
				break;
			}
				default: {
					UE_LOG(LogTemp, Error, TEXT("RealTimeImportPlugin: FBX Unknown type in File. Loading is canceled."));
					success = false;
					successMessage = "FBX Unknown type in File. Loading is canceled.";
					return 0;
				}
			}
		}
	}

	success = true;
	return endOffset;
}

uint32 URealTimeImportMesh::byteArrayToInt32(TArray<uint8>& data, uint32 start){
	uint32 val = 0.0;
	FMemory::Memcpy(&val, data.GetData() + start, sizeof(val));

	return val;
}

int64 URealTimeImportMesh::byteArrayToInt64(TArray<uint8>& data, uint32 start) {
	int64 val = 0.0;
	FMemory::Memcpy(&val, data.GetData() + start, sizeof(val));

	return val;
}

TArray<int32> URealTimeImportMesh::byteArrayToInt32Tarray(TArray<uint8>& data, uint32 start, uint32 size){
	TArray<int32> a;
	a.AddUninitialized(size);
	FMemory::Memcpy(a.GetData(), data.GetData() + start, size);
	return a;
}


FString URealTimeImportMesh::byteArrayToFString(TArray<uint8>& data, uint32 start, uint32 size){

	TArray<uint8> fbxCheck;
	fbxCheck.AddUninitialized(size);
	FMemory::Memcpy(fbxCheck.GetData(), data.GetData()+start, size);
	fbxCheck.Add(0x00);// null-terminator

	return FString(UTF8_TO_TCHAR(fbxCheck.GetData()));
}

TArray<uint8> URealTimeImportMesh::cutByteArray(TArray<uint8>& original, uint32 start, uint32 size){
	TArray<uint8> copy;
	copy.AddUninitialized(size);
	FMemory::Memcpy(copy.GetData(), original.GetData() + start, size);
	return copy;
}

double URealTimeImportMesh::parseBytesToDouble(TArray<uint8>& data, uint32 start){

	double val = 0.0;
	FMemory::Memcpy(&val, data.GetData()+start, sizeof(val));

	return val;
}

TArray<uint8> URealTimeImportMesh::uncompressFbxData(TArray<uint8>& data, uint32 start, uint32 size, uint32 compressedLen){
	TArray<uint8> uncompressedContent;
	uncompressedContent.AddUninitialized(size);

	FCompression::UncompressMemoryStream(NAME_Zlib, (void*)uncompressedContent.GetData(), uncompressedContent.Num(),
		(IMemoryReadStream::CreateFromBuffer(data.GetData() + start, compressedLen, false)), 0, compressedLen,
		ECompressionFlags::COMPRESS_ZLIB, 0);

	return uncompressedContent;
}

ERTIInformationType URealTimeImportMesh::informationTypeStringToEnum(FString informationType){
	
	if (informationType.Equals("ByPolygon")) {
		return ERTIInformationType::E_ByPolygon;
	}
	if (informationType.Equals("ByPolygonVertex")) {
		return ERTIInformationType::E_ByPolygonVertex;
	}
	if (informationType.Equals("ByVertex") || informationType.Equals("ByVertice")) {
		return ERTIInformationType::E_ByVertex;
	}
	if (informationType.Equals("ByEdge")) {
		return ERTIInformationType::E_ByEdge;
	}
	if (informationType.Equals("AllSame")) {
		return ERTIInformationType::E_AllSame;
	}


	return ERTIInformationType::E_none;
}

ERTIReferenceInformationType URealTimeImportMesh::referenceInformationTypeTypeStringToEnum(FString informationType){
	if (informationType.Equals("Direct")) {
		return ERTIReferenceInformationType::E_Direct;
	}
	if (informationType.Equals("IndexToDirect")) {
		return ERTIReferenceInformationType::E_IndexToDirect;
	}
	return ERTIReferenceInformationType::E_none;
}

void URealTimeImportMesh::createMaterialStructFromObj(FString mtlFilePath, TMap<FString, struct FRTIMaterialStruct>& materialStructs,
	bool autoDetectionNormalMap,
	bool useSRGB, bool createMipMaps) {

	struct FRTIMaterialStruct materialStruct;

	FString stringFileData;
	if (!FFileHelper::LoadFileToString(stringFileData, *mtlFilePath)) {
		UE_LOG(LogTemp, Error, TEXT("RealTimeImportPlugin: Can't load file: %s"), *mtlFilePath);
		return;
	}

	TArray<FString> lines;
	int32 lineCount = stringFileData.ParseIntoArray(lines, TEXT("\n"), true);
	stringFileData.Empty();

	if (lineCount < 1) {
		UE_LOG(LogTemp, Error, TEXT("RealTimeImportPlugin: corrupted mtl file (2): %s"), *mtlFilePath);
		return;
	}

	TArray<FString> lineElements;

	for (int32 i = 0; i < lineCount; i++) {
		FString line = lines[i].TrimStartAndEnd();
		if (line.StartsWith("#")) {
			continue;
		}



		//UE_LOG(LogTemp, Display, TEXT("RealTimeImportPlugin:_%s_"), *line);

		//new material settings
		if (line.StartsWith("newmtl ")) {

			if (materialStruct.isEmpty == false) {
				materialStructs.Add(materialStruct.textureName, materialStruct);
				materialStruct = FRTIMaterialStruct();
			}

			lineElements.Empty();
			int32 elementCount = line.ParseIntoArray(lineElements, TEXT(" "), true);
			if (elementCount == 2) {
				materialStruct.textureName = lineElements[1];
				materialStruct.isEmpty = false;
			}
			continue;
		}


		if (line.StartsWith("Ns ")) {
			lineElements.Empty();
			int32 elementCount = line.ParseIntoArray(lineElements, TEXT(" "), true);
			if (elementCount == 2) {
				materialStruct.specularExponent = FCString::Atof(*lineElements[1]);
			}
			continue;
		}

		if (line.StartsWith("d ")) {
			lineElements.Empty();
			int32 elementCount = line.ParseIntoArray(lineElements, TEXT(" "), true);
			if (elementCount == 2) {
				materialStruct.dissolved = FCString::Atof(*lineElements[1]);
			}
			continue;
		}

		if (line.StartsWith("Tr ")) {
			lineElements.Empty();
			int32 elementCount = line.ParseIntoArray(lineElements, TEXT(" "), true);
			if (elementCount == 2) {
				materialStruct.dissolvedInverted = FCString::Atof(*lineElements[1]);
			}
			continue;
		}

		if (line.StartsWith("Ka ")) {
			lineElements.Empty();
			int32 elementCount = line.ParseIntoArray(lineElements, TEXT(" "), true);
			if (elementCount == 4) {
				materialStruct.ambient = FColor(FCString::Atof(*lineElements[1]) * 255, FCString::Atof(*lineElements[2]) * 255, FCString::Atof(*lineElements[3]) * 255);
			}
			continue;
		}

		if (line.StartsWith("Kd ")) {
			lineElements.Empty();
			int32 elementCount = line.ParseIntoArray(lineElements, TEXT(" "), true);
			if (elementCount == 4) {
				materialStruct.diffuse = FColor(FCString::Atof(*lineElements[1]) * 255, FCString::Atof(*lineElements[2]) * 255, FCString::Atof(*lineElements[3]) * 255);
			}
			continue;
		}

		if (line.StartsWith("Ks ")) {
			lineElements.Empty();
			int32 elementCount = line.ParseIntoArray(lineElements, TEXT(" "), true);
			if (elementCount == 4) {
				materialStruct.specular = FColor(FCString::Atof(*lineElements[1]) * 255, FCString::Atof(*lineElements[2]) * 255, FCString::Atof(*lineElements[3]) * 255);
			}
			continue;
		}

		if (line.StartsWith("map_") || line.StartsWith("bump") || line.StartsWith("disp")
			|| line.StartsWith("decal") || line.StartsWith("refl") || line.StartsWith("Pc")
			|| line.StartsWith("aniso") || line.StartsWith("norm")) {
			lineElements.Empty();
			int32 elementCount = line.ParseIntoArray(lineElements, TEXT(" "), true);
			if (elementCount >= 2) {
				//map statements are not implemented and are cut out here.
				FString imageFileWithPath = lineElements.Last();

				UTexture2D* texture = nullptr;
				FString fileName = FGuid::NewGuid().ToString();
				int32 errorCode = 0;
				FString errorMessage = FString();

				TextureCompressionSettings compressionSettings = TextureCompressionSettings::TC_Default;
				if (autoDetectionNormalMap && (line.StartsWith("map_Norm") || line.StartsWith("map_norm") || line.StartsWith("norm ") || line.StartsWith("map_Bump ") || line.StartsWith("map_bump ") || line.StartsWith("bump "))) {
					compressionSettings = TextureCompressionSettings::TC_Normalmap;
				}


				URealTimeImportImage::getRealTimeImportImage()->LoadImageFile(ERTIDirectoryType::E_ad,
						mtlFilePath.Replace(*FPaths::GetCleanFilename(mtlFilePath), *imageFileWithPath),
						texture, fileName, errorCode, errorMessage, compressionSettings, useSRGB, createMipMaps);
	

				if (errorCode == 0) {
					if (line.StartsWith("map_Kd ")) {
						FRTITextureStruct* ts = new FRTITextureStruct();
						ts->texture = texture;
						ts->textureType = ERTITextureType::E_Kd;
						materialStruct.textures.Add(fileName,*ts);
						continue;
					}
					if (line.StartsWith("map_Ks ")) {
						FRTITextureStruct* ts = new FRTITextureStruct();
						ts->texture = texture;
						ts->textureType = ERTITextureType::E_Ks;
						materialStruct.textures.Add(fileName, *ts);
						continue;
					}
					if (line.StartsWith("map_Ns ")) {
						FRTITextureStruct* ts = new FRTITextureStruct();
						ts->texture = texture;
						ts->textureType = ERTITextureType::E_Ns;
						materialStruct.textures.Add(fileName, *ts);
						continue;
					}
					if (line.StartsWith("map_d ")) {
						FRTITextureStruct* ts = new FRTITextureStruct();
						ts->texture = texture;
						ts->textureType = ERTITextureType::E_d;
						materialStruct.textures.Add(fileName, *ts);
						continue;
					}
					if (line.StartsWith("map_Bump ") || line.StartsWith("map_bump ") || line.StartsWith("bump ")) {
						FRTITextureStruct* ts = new FRTITextureStruct();
						ts->texture = texture;
						ts->textureType = ERTITextureType::E_bump;
						materialStruct.textures.Add(fileName, *ts);
						continue;
					}
					if (line.StartsWith("map_Disp") || line.StartsWith("map_disp") || line.StartsWith("disp ")) {
						FRTITextureStruct* ts = new FRTITextureStruct();
						ts->texture = texture;
						ts->textureType = ERTITextureType::E_disp;
						materialStruct.textures.Add(fileName, *ts);
						continue;
					}
					if (line.StartsWith("map_Decal") || line.StartsWith("map_decal") || line.StartsWith("decal ")) {
						FRTITextureStruct* ts = new FRTITextureStruct();
						ts->texture = texture;
						ts->textureType = ERTITextureType::E_decal;
						materialStruct.textures.Add(fileName, *ts);
						continue;
					}
					if (line.StartsWith("map_Refl") || line.StartsWith("map_refl") || line.StartsWith("refl ")) {
						FRTITextureStruct* ts = new FRTITextureStruct();
						ts->texture = texture;
						ts->textureType = ERTITextureType::E_refl;
						materialStruct.textures.Add(fileName, *ts);
						continue;
					}
					if (line.StartsWith("map_Pr ") || line.StartsWith("Pr ")) {
						FRTITextureStruct* ts = new FRTITextureStruct();
						ts->texture = texture;
						ts->textureType = ERTITextureType::E_Pr;
						materialStruct.textures.Add(fileName, *ts);
						continue;
					}
					if (line.StartsWith("map_Pm ") || line.StartsWith("Pm")) {
						FRTITextureStruct* ts = new FRTITextureStruct();
						ts->texture = texture;
						ts->textureType = ERTITextureType::E_Pm;
						materialStruct.textures.Add(fileName, *ts);
						continue;
					}
					if (line.StartsWith("map_Ps ") || line.StartsWith("Ps")) {
						FRTITextureStruct* ts = new FRTITextureStruct();
						ts->texture = texture;
						ts->textureType = ERTITextureType::E_Ps;
						materialStruct.textures.Add(fileName, *ts);
						continue;
					}
					if (line.StartsWith("Pc ")) {
						FRTITextureStruct* ts = new FRTITextureStruct();
						ts->texture = texture;
						ts->textureType = ERTITextureType::E_Pc;
						materialStruct.textures.Add(fileName, *ts);
						continue;
					}
					if (line.StartsWith("Pcr ")) {
						FRTITextureStruct* ts = new FRTITextureStruct();
						ts->texture = texture;
						ts->textureType = ERTITextureType::E_Pcr;
						materialStruct.textures.Add(fileName, *ts);
						continue;
					}
					if (line.StartsWith("map_Ke ") || line.StartsWith("Ke")) {
						FRTITextureStruct* ts = new FRTITextureStruct();
						ts->texture = texture;
						ts->textureType = ERTITextureType::E_Ke;
						materialStruct.textures.Add(fileName, *ts);
						continue;
					}
					if (line.StartsWith("map_Ansio") || line.StartsWith("map_ansio") || line.StartsWith("aniso ")) {
						FRTITextureStruct* ts = new FRTITextureStruct();
						ts->texture = texture;
						ts->textureType = ERTITextureType::E_aniso;
						materialStruct.textures.Add(fileName, *ts);
						continue;
					}
					if (line.StartsWith("map_Ansior") || line.StartsWith("map_ansior") || line.StartsWith("anisor ")) {
						FRTITextureStruct* ts = new FRTITextureStruct();
						ts->texture = texture;
						ts->textureType = ERTITextureType::E_anisor;
						materialStruct.textures.Add(fileName, *ts);
						continue;
					}
					if (line.StartsWith("map_Norm") || line.StartsWith("map_norm") || line.StartsWith("norm ")) {
						FRTITextureStruct* ts = new FRTITextureStruct();
						ts->texture = texture;
						ts->textureType = ERTITextureType::E_norm;
						materialStruct.textures.Add(fileName, *ts);
						continue;
					}
					FRTITextureStruct* ts = new FRTITextureStruct();
					ts->texture = texture;
					ts->textureType = ERTITextureType::E_un;
					materialStruct.textures.Add(fileName, *ts);
				}
			}
			continue;
		}
	}

	if (materialStruct.isEmpty == false) {
		materialStructs.Add(materialStruct.textureName, materialStruct);
	}
}



void URealTimeImportMesh::breakMeshStruct(struct FRTIMeshStruct meshStruct,
	FString& meshName,
	TArray<int32>& triangles,
	struct FRTIMaterialStruct& materialStruct) {

	meshName = meshStruct.meshName;
	triangles = meshStruct.triangles;
	materialStruct = meshStruct.materialData;
}



void URealTimeImportMesh::breakMaterialStruct(struct FRTIMaterialStruct materialData,
	bool& hasTexture,
	FString& textureName,
	TArray<struct FRTITextureStruct>& textures,
	FColor& ambient,
	FColor& diffuse,
	FColor& specular,
	float& specularExponent,
	float& dissolved,
	float& dissolvedInverted) {

	hasTexture = !materialData.isEmpty;
	textureName = materialData.textureName;
	materialData.textures.GenerateValueArray(textures);
	ambient = materialData.ambient;
	diffuse = materialData.diffuse;
	specular = materialData.specular;
	specularExponent = materialData.specularExponent;
	dissolved = materialData.dissolved;
	dissolvedInverted = materialData.dissolvedInverted;

}

void URealTimeImportMesh::breakTextureStruct(FRTITextureStruct textureData, UTexture2D*& texture, ERTITextureType& textureType) {
	texture = textureData.texture;
	textureType = textureData.textureType;
}
