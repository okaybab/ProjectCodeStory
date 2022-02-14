// Copyright 2018-2020 David Romanski(Socke). All Rights Reserved.

#include "RealTimeImportBPLibrary.h"


URealTimeImportBPLibrary* URealTimeImportBPLibrary::RealTimeImportBPLibrary;
URealTimeImportBPLibrary::URealTimeImportBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer){
	RealTimeImportBPLibrary = this;
}

/*Delegate functions*/
void URealTimeImportBPLibrary::LoadFileEventDelegate(const int32 errorCode, const FString eventID, const TArray<uint8>& bytes , const FString fileName, const FString errorMessage) {}
void URealTimeImportBPLibrary::SaveFileEventDelegate(const int32 errorCode, const FString eventID, const FString fileName, const FString fullPath, const FString errorMessage) {}
void URealTimeImportBPLibrary::LoadAllFilesFromDirectoryEventDelegate(const FString eventID, const int32 &count, const  TArray<FString> &files, const TArray<FString> &filePaths) {}
void URealTimeImportBPLibrary::CreateSoundWaveFromFileEventDelegate(const int32 errorCode, const FString eventID, const USoundWaveProcedural* Sound, const FString fileName, const FString errorMessage) {}
void URealTimeImportBPLibrary::CreateSoundWaveFromBytesEventDelegate(const int32 errorCode, const FString eventID, const USoundWaveProcedural* Sound, const FString errorMessage) {}
void URealTimeImportBPLibrary::PlaySoundEventDelegate(const int32 errorCode, const FString fileID, const FString errorMessage, const float durationInSeconds){}
void URealTimeImportBPLibrary::ActiveSoundPositionEventDelegate(const FString fileID, const float positionSecondsStart, const float positionSecondsEnd, const float positionPercent){}
void URealTimeImportBPLibrary::ActiveSoundEnvelopeValueEventDelegate(const float averageEnvelopeValue, const float maxEnvelope, const int32 numWaveInstances){}
void URealTimeImportBPLibrary::StopSoundEventDelegate(const FString fileID){}
void URealTimeImportBPLibrary::LoadImageFileEventDelegate(const int32 errorCode, const FString eventID, const UTexture2D* texture, const FString fileName, const FString errorMessage) {}
void URealTimeImportBPLibrary::LoadImageBytesEventDelegate(const int32 errorCode, const FString eventID, const UTexture2D* texture, const FString errorMessage) {}

URealTimeImportBPLibrary * URealTimeImportBPLibrary::getRealTimeImportTarget() {
	return RealTimeImportBPLibrary;
}

void URealTimeImportBPLibrary::LoadFile(ERTIDirectoryType directoryType, FString filePath, FString& fileName, TArray<uint8>& bytes, int32& errorCode, FString& errorMessage){
	UFileFunctionsRealTimeImport::getFileFunctionsRealTimeImport()->LoadFile(directoryType, filePath, fileName, bytes, errorCode, errorMessage);
}

URealTimeImportBPLibrary* URealTimeImportBPLibrary::LoadFileAsync(ERTIDirectoryType directoryType, FString filePath, FString eventID){
	UFileFunctionsRealTimeImport::getFileFunctionsRealTimeImport()->LoadFileAsync(directoryType, filePath, eventID);
	return this;
}

void URealTimeImportBPLibrary::SaveFile(ERTIDirectoryType directoryType, FString filePath, bool createDirectories, bool overwriteFile, TArray<uint8> bytes, FString& fileName, FString& fullPath, int32& errorCode, FString& errorMessage){
	UFileFunctionsRealTimeImport::getFileFunctionsRealTimeImport()->SaveFile(directoryType, filePath, createDirectories, overwriteFile, bytes, fileName, fullPath, errorCode, errorMessage);
}

URealTimeImportBPLibrary* URealTimeImportBPLibrary::SaveFileAsync(ERTIDirectoryType directoryType, FString filePath, bool createDirectories, bool overwriteFile, TArray<uint8> bytes, FString eventID){
	UFileFunctionsRealTimeImport::getFileFunctionsRealTimeImport()->SaveFileAsync(directoryType, filePath, createDirectories, overwriteFile, bytes, eventID);
	return this;
}

void URealTimeImportBPLibrary::LoadAllFilesFromDirectory(ERTIDirectoryType directoryType, ERTIFileType fileType, FString dirPath, bool recursiveSearch, int32& count, TArray<FString>& files, TArray<FString>& filePaths){
	UFileFunctionsRealTimeImport::getFileFunctionsRealTimeImport()->LoadAllFilesFromDirectory(directoryType, fileType, dirPath, recursiveSearch, count, files, filePaths);
}

URealTimeImportBPLibrary* URealTimeImportBPLibrary::LoadAllFilesFromDirectoryAsync(ERTIDirectoryType directoryType, ERTIFileType fileType, FString dirPath, bool recursiveSearch, FString eventID){
	UFileFunctionsRealTimeImport::getFileFunctionsRealTimeImport()->LoadAllFilesFromDirectoryAsync(directoryType, fileType, dirPath, recursiveSearch, eventID);
	return this;
}

//void URealTimeImportBPLibrary::testTexture(UTexture2D* texture, UTexture2D* texture2)
//{
//	UE_LOG(LogTemp, Warning, TEXT("testTexture2: %s"), *texture2->PlatformData->DerivedDataKey);
//	for (int32 i = 0; i < texture2->PlatformData->Mips.Num(); i++){
//		UE_LOG(LogTemp, Warning, TEXT("testTexture2: %s"), *texture2->PlatformData->Mips[i].DerivedDataKey);
//	}
//
//	UE_LOG(LogTemp, Warning, TEXT("testTexture: %s"), *texture->PlatformData->DerivedDataKey);
//	for (int32 i = 0; i < texture->PlatformData->Mips.Num(); i++) {
//		UE_LOG(LogTemp, Warning, TEXT("testTexture: %s"), *texture->PlatformData->Mips[i].DerivedDataKey);
//	}
//	
//	//texture->UpdateResource();
//}
//
//void URealTimeImportBPLibrary::testMesh(TArray<FVector> vertices, TArray<FVector2D> UV0, TArray<int32> triangles){
//	for (int32 i = 0; i < vertices.Num(); i++) {
//		UE_LOG(LogTemp, Error, TEXT("%f|%f|%f"), vertices[i].X, vertices[i].Y, vertices[i].Z);
//	}
//	for (int32 i = 0; i < UV0.Num(); i++) {
//	UE_LOG(LogTemp, Error, TEXT("%f|%f"), UV0[i].X, UV0[i].Y);
//	}
//	for (int32 i = 0; i < UV0.Num(); i++) {
//		UE_LOG(LogTemp, Warning, TEXT("%f|%f|%f  %f|%f"), vertices[i].X, vertices[i].Y, vertices[i].Z, UV0[i].X, UV0[i].Y);
//	}
//	for (int32 i = 0; i < triangles.Num(); i++) {
//		UE_LOG(LogTemp, Display, TEXT("%i %f|%f|%f"), triangles[i], vertices[triangles[i]].X, vertices[triangles[i]].Y, vertices[triangles[i]].Z);
//	}
//}




