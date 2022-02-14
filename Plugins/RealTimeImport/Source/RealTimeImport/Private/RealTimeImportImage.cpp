// Copyright 2018-2020 David Romanski(Socke). All Rights Reserved.

#include "RealTimeImportImage.h"


URealTimeImportImage* URealTimeImportImage::realTimeImportImage;
URealTimeImportImage::URealTimeImportImage(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) {
	realTimeImportImage = this;
}


URealTimeImportImage* URealTimeImportImage::getRealTimeImportImage() {
	return realTimeImportImage;
}

void URealTimeImportImage::LoadImageFile(ERTIDirectoryType directoryType, FString filePath, UTexture2D*& texture, FString& fileName, int32& errorCode, FString& errorMessage, TextureCompressionSettings compressionSettings, bool SRGB, bool createMipMaps){
	IFileManager& FileManager = IFileManager::Get();

	FString dir;
	if (directoryType == ERTIDirectoryType::E_ad) {
		dir = FPaths::ConvertRelativePathToFull(filePath);
	}
	else {
		FString ProjectDir = FPaths::ProjectDir();
		dir = FPaths::ConvertRelativePathToFull(ProjectDir + filePath);
	}
	FString cacheID = dir + "_" + FileManager.GetTimeStamp(*dir).ToString();
	fileName = FPaths::GetCleanFilename(filePath);

	if (textureCache.Find(cacheID) != nullptr) {
		errorCode = 0;
		texture = *(textureCache.Find(cacheID));
		return;
	}

	TArray<uint8> fileData;
	URealTimeImportBPLibrary::getRealTimeImportTarget()->LoadFile(directoryType, filePath, fileName, fileData, errorCode, errorMessage);
	if (errorCode > 0) {
		return;
	}
	texture = createTexture2D(fileData, fileName, errorCode, errorMessage, compressionSettings, SRGB, createMipMaps);
	if (errorCode == 0) {
		texture->AddToRoot();
		textureCache.Add(cacheID, texture);
	}
}

void URealTimeImportImage::LoadImageFileAsync(ERTIDirectoryType directoryType, FString filePath, FString eventID, TextureCompressionSettings compressionSettings, bool SRGB, bool createMipMaps) {
	IFileManager& FileManager = IFileManager::Get();
	FString dir;
	if (directoryType == ERTIDirectoryType::E_ad) {
		dir = FPaths::ConvertRelativePathToFull(filePath);
	}
	else {
		FString ProjectDir = FPaths::ProjectDir();
		dir = FPaths::ConvertRelativePathToFull(ProjectDir + filePath);
	}
	FString cacheID = dir + "_" + FileManager.GetTimeStamp(*dir).ToString();

	if (textureCache.Find(cacheID) != nullptr) {
		URealTimeImportBPLibrary::getRealTimeImportTarget()->onLoadImageFileEventDelegate.Broadcast(0, eventID, *(textureCache.Find(cacheID)), FPaths::GetCleanFilename(filePath), "");
		return;
	}

	if (loadImageFileThread == nullptr) {
		loadImageFileThread = new FLoadImageFileThread(this, directoryType, filePath, eventID, compressionSettings, SRGB, createMipMaps);
	}
	else {
		loadImageFileThread->restartThread(this, directoryType, filePath, eventID, compressionSettings, SRGB, createMipMaps);
	}
}


void URealTimeImportImage::LoadImageBytesAsync(FString name, TArray<uint8> bytes, FString eventID, TextureCompressionSettings compressionSettings, bool SRGB, bool createMipMaps) {
	FString cacheID = name + "_" + FString::FromInt(bytes.Num());
	if (textureCache.Find(cacheID) != nullptr) {
		URealTimeImportBPLibrary::getRealTimeImportTarget()->onLoadImageBytesEventDelegate.Broadcast(0, eventID, *(textureCache.Find(cacheID)), "");
		return;
	}


	if (loadImageBytesThread == nullptr) {
		loadImageBytesThread = new FLoadImageBytesThread(this, name, bytes, eventID, compressionSettings, SRGB, createMipMaps);
	}
	else {
		loadImageBytesThread->restartThread(this, name, bytes, eventID, compressionSettings, SRGB, createMipMaps);
	}
}


void URealTimeImportImage::LoadImageBytes(FString name, TArray<uint8> fileData, UTexture2D*& texture, int32& errorCode, FString& errorMessage, TextureCompressionSettings compressionSettings, bool SRGB, bool createMipMaps) {
	FString cacheID = name + "_" + FString::FromInt(fileData.Num());
	if (textureCache.Find(cacheID) != nullptr) {
		errorCode = 0;
		texture = *(textureCache.Find(cacheID));
		return;
	}
	texture = createTexture2D(fileData, name, errorCode, errorMessage, compressionSettings, SRGB, createMipMaps);
	if (errorCode == 0) {
		texture->AddToRoot();
		textureCache.Add(cacheID, texture);
	}
}

//
//void URealTimeImportImage::LoadImageAsMipMap(UTexture2D* texture, ERTIDirectoryType directoryType, FString filePath, int32& errorCode, FString& errorMessage) {
//	IFileManager& FileManager = IFileManager::Get();
//
//	FString dir;
//	if (directoryType == ERTIDirectoryType::E_ad) {
//		dir = FPaths::ConvertRelativePathToFull(filePath);
//	}
//	else {
//		FString ProjectDir = FPaths::ProjectDir();
//		dir = FPaths::ConvertRelativePathToFull(ProjectDir + filePath);
//	}
//
//	if (texture == nullptr || texture->PlatformData == nullptr) {
//		errorCode = 101;
//		errorMessage = "You cannot add a Mip Map to this texture.";
//		return;
//	}
//
//	if (texture->PlatformData->Mips[0].SizeX != texture->PlatformData->Mips[0].SizeY) {
//		errorCode = 104;
//		errorMessage = "You cannot add a Mip Map to this texture. Wrong image size. Only squares in 4096,2048,1024,512,256,128,64,32,16,8,4 are possible. Your texture is " + FString::FromInt(texture->PlatformData->Mips[0].SizeX) + "x" + FString::FromInt(texture->PlatformData->Mips[0].SizeY);
//		return;
//	}
//
//	//too lazy for mathematics 
//	if (texture->PlatformData->Mips[0].SizeX != 4096 &&
//		texture->PlatformData->Mips[0].SizeX != 2048 &&
//		texture->PlatformData->Mips[0].SizeX != 1024 &&
//		texture->PlatformData->Mips[0].SizeX != 512 &&
//		texture->PlatformData->Mips[0].SizeX != 256 &&
//		texture->PlatformData->Mips[0].SizeX != 128 &&
//		texture->PlatformData->Mips[0].SizeX != 64 &&
//		texture->PlatformData->Mips[0].SizeX != 32 &&
//		texture->PlatformData->Mips[0].SizeX != 16 &&
//		texture->PlatformData->Mips[0].SizeX != 8 &&
//		texture->PlatformData->Mips[0].SizeX != 4) {
//		errorCode = 105;
//		errorMessage = "You cannot add a Mip Map to this texture. Wrong image size. Only squares in 4096,2048,1024,512,256,128,64,32,16,8,4 are possible. Your texture is " + FString::FromInt(texture->PlatformData->Mips[0].SizeX) + "x" + FString::FromInt(texture->PlatformData->Mips[0].SizeY);
//		return;
//	}
//
//	FString fileName;
//	TArray<uint8> fileData;
//	URealTimeImportBPLibrary::LoadFile(directoryType, filePath, fileName, fileData, errorCode, errorMessage);
//	if (errorCode > 0) {
//		return;
//	}
//
//	auto& imageWrapperModule = FModuleManager::GetModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
//
//	auto imageFormat = imageWrapperModule.DetectImageFormat(fileData.GetData(), fileData.Num());
//	if (imageFormat == EImageFormat::Invalid) {
//		errorCode = 4;
//		errorMessage = "Wrong image format. Please use only PNG,JPG,BMP";
//		return;
//	}
//
//	TSharedPtr<IImageWrapper> imageWrapper = imageWrapperModule.CreateImageWrapper(imageFormat);
//	if (!imageWrapper.IsValid()) {
//		errorCode = 5;
//		errorMessage = "Can't create image wrapper.";
//		return;
//	}
//
//
//	TArray<uint8> decompressedImageData;
//	imageWrapper->SetCompressed(fileData.GetData(), fileData.Num());
//	bool successDecompress = imageWrapper->GetRaw(ERGBFormat::BGRA, 8, decompressedImageData);
//	if (!successDecompress || decompressedImageData.Num() == 0) {
//		errorCode = 6;
//		errorMessage = "Can't decompress image";
//		fileData.Empty();
//		return;
//	}
//
//	addMipMap(texture, decompressedImageData, imageWrapper->GetWidth(), imageWrapper->GetHeight(), errorCode, errorMessage);
//
//	decompressedImageData.Empty();
//	fileData.Empty();
//	
//
//}

void URealTimeImportImage::addMipMap(UTexture2D* texture, TArray<uint8> decompressedImageData, int32 width, int32 height, int32& errorCode, FString& errorMessage){

	if (decompressedImageData.Num() == 0) {
		errorCode = 111;
		errorMessage = "Can't create MipMaps. Image data is empty.";
		return;
	}


	//if (width != height) {
	//	errorCode = 106;
	//	errorMessage = "You cannot use this image as mipmap. Wrong image size. Only squares in 4096,2048,1024,512,256,128,64,32,16,8,4 are possible. Your image is " + FString::FromInt(width) + "x" + FString::FromInt(height);
	//	return;
	//}

	////too lazy for mathematics 
	//if (width != 4096 &&
	//	width != 2048 &&
	//	width != 1024 &&
	//	width != 512 &&
	//	width != 256 &&
	//	width != 128 &&
	//	width != 64 &&
	//	width != 32 &&
	//	width != 16 &&
	//	width != 8 &&
	//	width != 4 &&
	//	width != 2 &&
	//	width != 1){
	//	errorCode = 107;
	//	errorMessage = "You cannot use this image as mipmap. Wrong image size. Only squares in 4096,2048,1024,512,256,128,64,32,16,8,4 are possible. Your image is " + FString::FromInt(width) + "x" + FString::FromInt(height);
	//	return;
	//}

	//if (texture->PlatformData->Mips.Num() > 0) {
	//	if ((texture->PlatformData->Mips.Last().SizeX / 2) != width) {
	//		errorCode = 108;
	//		errorMessage = "You must load the mipmaps in the correct order. After 1024 comes 512 and then 256 and so on.";
	//		return;
	//	}
	//}


	int32 NumBlocksX = width / GPixelFormats[EPixelFormat::PF_B8G8R8A8].BlockSizeX;
	int32 NumBlocksY = height / GPixelFormats[EPixelFormat::PF_B8G8R8A8].BlockSizeY;
	FTexture2DMipMap* Mip = new FTexture2DMipMap();
	texture->PlatformData->Mips.Add(Mip);
	Mip->SizeX = width;
	Mip->SizeY = height;
	Mip->SizeZ = 1;
	//if (width >= 128) {
	//	Mip->DerivedDataKey = texture->PlatformData->DerivedDataKey + "_MIP"+FString::FromInt(texture->PlatformData->Mips.Num()-1)+"_"+FString::FromInt(width)+"x"+ FString::FromInt(height);
	//}
	Mip->BulkData.Lock(LOCK_READ_WRITE);
	Mip->BulkData.Realloc(NumBlocksX * NumBlocksY * GPixelFormats[EPixelFormat::PF_B8G8R8A8].BlockBytes);
	Mip->BulkData.Unlock();

	int32 mipIndex = texture->PlatformData->Mips.Num() - 1;

	if (mipIndex < 0) {
		errorCode = 102;
		errorMessage = "You cannot add a Mip Map to this texture.";
		return;
	}

	uint8* mipData = (uint8*)texture->PlatformData->Mips[mipIndex].BulkData.Lock(LOCK_READ_WRITE);
	FMemory::Memcpy(mipData, decompressedImageData.GetData(), decompressedImageData.Num());
	texture->PlatformData->Mips[mipIndex].BulkData.Unlock();

	decompressedImageData.Empty();
	errorCode = 0;
}


void URealTimeImportImage::triggerLoadImageFileEventDelegate(int32 errorCode, FString eventID, UTexture2D* texture, FString fileName, FString errorMessage)
{
	AsyncTask(ENamedThreads::GameThread, [texture, fileName, eventID, errorCode, errorMessage]() {
		URealTimeImportBPLibrary::getRealTimeImportTarget()->onLoadImageFileEventDelegate.Broadcast(errorCode, eventID, texture, fileName, errorMessage);
	});

}

void URealTimeImportImage::triggerLoadImageBytesEventDelegate(int32 errorCode, FString eventID, UTexture2D* texture, FString errorMessage){
	AsyncTask(ENamedThreads::GameThread, [eventID, texture, errorCode, errorMessage]() {
		URealTimeImportBPLibrary::getRealTimeImportTarget()->onLoadImageBytesEventDelegate.Broadcast(errorCode, eventID, texture, errorMessage);
	});
}



UTexture2D* URealTimeImportImage::createTexture2D(TArray<uint8>& fileData, FString name, int32& errorCode, FString& errorMessage, TextureCompressionSettings compressionSettings, bool SRGB, bool createMipMaps) {
	if (fileData.Num() == 0) {
		errorCode = 14;
		errorMessage = "No texture data found.";
		return nullptr;
	}
	
	auto& imageWrapperModule = FModuleManager::GetModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));

	auto imageFormat = imageWrapperModule.DetectImageFormat(fileData.GetData(), fileData.Num());
	if (imageFormat == EImageFormat::Invalid) {
		errorCode = 4;
		errorMessage = "Wrong image format. Please use only PNG,JPG,BMP,EXR";
		return nullptr;
	}

	TSharedPtr<IImageWrapper> imageWrapper = imageWrapperModule.CreateImageWrapper(imageFormat);
	if (!imageWrapper.IsValid()) {
		errorCode = 5;
		errorMessage = "Can't create image wrapper.";
		return nullptr;
	}


	TArray<uint8> decompressedImageData;
	imageWrapper->SetCompressed(fileData.GetData(), fileData.Num());
	bool successDecompress = imageWrapper->GetRaw(ERGBFormat::BGRA, 8, decompressedImageData);
	if (!successDecompress || decompressedImageData.Num() == 0) {
		errorCode = 6;
		errorMessage = "Can't decompress image";
		return nullptr;
	}

	//if (imageWrapper->GetWidth() % 2 != 0 || imageWrapper->GetHeight() % 2 != 0) {
	//	errorCode = 3;
	//	errorMessage = "Texture cannot be created. The resolution must be divisible by 2.";
	//	return nullptr;
	//}


	//UTexture2D* texture = NewObject<UTexture2D>(GetTransientPackage(),FName(FGuid::NewGuid().ToString()),RF_Transient);

	//if (texture == NULL) {
	//	errorCode = 7;
	//	errorMessage = "Invalid parameters specified for UTexture2D::Create()";
	//	return NULL;
	//}


	//texture->PlatformData = new FTexturePlatformData();
	//texture->PlatformData->SizeX = imageWrapper->GetWidth();
	//texture->PlatformData->SizeY = imageWrapper->GetHeight();
	//texture->PlatformData->PixelFormat = EPixelFormat::PF_B8G8R8A8;

	//addMipMap(texture, decompressedImageData, imageWrapper->GetWidth(), imageWrapper->GetHeight(), errorCode, errorMessage);


	UTexture2D* texture = UTexture2D::CreateTransient(imageWrapper->GetWidth(), imageWrapper->GetHeight(), EPixelFormat::PF_B8G8R8A8);
	if (texture == nullptr) {
		errorCode = 7;
		errorMessage = "Invalid parameters specified for UTexture2D::Create()";
		return nullptr;
	}

	uint8* mipData = (uint8*)texture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
	FMemory::Memcpy(mipData, decompressedImageData.GetData(), decompressedImageData.Num());
	texture->PlatformData->Mips[0].BulkData.Unlock();
	texture->CompressionSettings = compressionSettings;
	texture->SRGB = SRGB;


	texture->CompressionSettings = compressionSettings;
	texture->SRGB = SRGB;
	
	//texture->PlatformData->DerivedDataKey = FGuid::NewGuid().ToString();
	//if (imageWrapper->GetWidth() >= 128) {
	//	texture->PlatformData->Mips[0].DerivedDataKey = texture->PlatformData->DerivedDataKey + "_MIP0" + FString::FromInt(imageWrapper->GetWidth()) + "x" + FString::FromInt(imageWrapper->GetHeight());
	//}

	texture->PlatformData->PackedData = 1;
	if (createMipMaps) {

		if (imageWrapper->GetWidth() != imageWrapper->GetHeight() || imageWrapper->GetWidth() < 4 || imageWrapper->GetWidth() > 4096 || imageWrapper->GetWidth() % 2 != 0.f){
			UE_LOG(LogTemp, Warning, TEXT("RealTimeImport: Can't create MipMaps. UE4 allows MipMaps only in certain sizes.The texture into which the MipMap is added must also follow these rules.The texture must be 4096x4096 or half that size(2048x2048) or half that size and so on down to 4x4.If you have a texture in 1024x1024 you can only add a MipMap in 512x512.If you want to add another MipMap the image must be 256x256 and so on."));
		}
		else {
			int32 targetX = texture->GetSizeX() / 2;
			int32 targetY = texture->GetSizeY() / 2;
			while (targetX >= 1 && targetX >= 1) {
				TArray<uint8> outData = resizeRawImageData(decompressedImageData, texture->GetSizeX(), texture->GetSizeY(), targetX, targetY, texture->GetPixelFormat());
	
				addMipMap(texture, outData, targetX, targetY, errorCode, errorMessage);

				if (errorCode != 0) {
					return nullptr;
				}


				if (targetX <= 1 || targetY <= 1) {
					break;
				}

				targetX = targetX / 2;
				targetY = targetY / 2;

				//break;
			}
		}
	
	}


	decompressedImageData.Empty();
	fileData.Empty();
	errorCode = 0;



	if (IsInGameThread()) {
		texture->UpdateResource();
	}
	else {
		AsyncTask(ENamedThreads::GameThread, [texture]() {
			texture->UpdateResource();
		});
	}

	return texture;
}



TArray<uint8> URealTimeImportImage::resizeRawImageData(TArray<uint8> sourceData, int32 sourceX, int32 sourceY, int32 targetX, int32 targetY, EPixelFormat pixelformat){
	
	TArray<uint8> outData;

	const FColor* OriginalBytes = reinterpret_cast<const FColor*>(sourceData.GetData());
	TArrayView<const FColor> OriginalColors(OriginalBytes, sourceX * sourceY);

	TArray<FColor> TargetColors;
	TargetColors.SetNumUninitialized(targetX * targetY);

	//if (targetX != sourceX || targetY != sourceY){
		FImageUtils::ImageResize(sourceX, sourceY, OriginalColors, targetX, targetY, TArrayView<FColor>(TargetColors), false);
	//}
	//else{
	//	outData = TArray<FColor>(OriginalColors.GetData(), OriginalColors.GetTypeSize() * OriginalColors.Num());
	//}


		int32 ImageBytes = CalculateImageBytes(targetX, targetX, 0, pixelformat);
		outData.AddUninitialized(ImageBytes);
	
		FMemory::Memcpy(outData.GetData(), TargetColors.GetData(), ImageBytes);

	//IImageWrapperModule& imageWrapperModule = FModuleManager::GetModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	//TSharedPtr<IImageWrapper> imageWrapper2 = imageWrapperModule.CreateImageWrapper(EImageFormat::PNG);
	//imageWrapper2->SetRaw(TargetColors.GetData(), TargetColors.GetAllocatedSize(), targetX, targetY, ERGBFormat::BGRA, 8);
	//TArray64<uint8> compressedBytes = imageWrapper2->GetCompressed(100);
	//imageWrapper2->SetCompressed(compressedBytes.GetData(), compressedBytes.Num());
	//bool successDecompress = imageWrapper2->GetRaw(ERGBFormat::BGRA, 8, outData);

	//FFileHelper::SaveArrayToFile(compressedBytes, *FString("D:/test_"+FString::FromInt(targetX)+"x" + FString::FromInt(targetY) + ".png"));

	return outData;
}

void URealTimeImportImage::removeTextureFromCacheAndDestroyTexture(UTexture2D* texture) {
	FString key;
	for (auto& element : textureCache) {
		if (texture == element.Value) {
			key = element.Key;
			break;
		}
	}
	if (key.IsEmpty() == false) {
		textureCache.Remove(key);
		texture->RemoveFromRoot();
		for (int32 i = 0; i < texture->PlatformData->Mips.Num(); i++){
			texture->PlatformData->Mips[i].BulkData.RemoveBulkData();
		}
		texture->ConditionalBeginDestroy();
	}
}

void URealTimeImportImage::emptyTextureCacheAndDestroyTextures() {
	for (auto& element : textureCache) {
		if (element.Value != nullptr) {
			element.Value->RemoveFromRoot();
			for (int32 i = 0; i < element.Value->PlatformData->Mips.Num(); i++) {
				element.Value->PlatformData->Mips[i].BulkData.RemoveBulkData();
			}
			element.Value->ConditionalBeginDestroy();
		}
	}

	textureCache.Empty();
}

TArray<uint8> URealTimeImportImage::textureRenderTarget2D_ToByteArray(UTextureRenderTarget2D* textureRenderTarget2D, ERTIExportImageType type, int32 quality) {
	TArray<uint8> compressedBytes;
	check(textureRenderTarget2D != nullptr);
	if (textureRenderTarget2D->GetFormat() == PF_B8G8R8A8) {
		FRenderTarget* RenderTarget = textureRenderTarget2D->GameThread_GetRenderTargetResource();
		FIntPoint Size = RenderTarget->GetSizeXY();

		TArray64<uint8> rawImageData;
		if (getRawImageData(textureRenderTarget2D, rawImageData)) {

			if (type == ERTIExportImageType::E_bmp) {
				rawImageDataToBMPData(compressedBytes, Size.X, Size.Y, (FColor*)rawImageData.GetData(), rawImageData.Num());
			}
			else {

				if (quality < 1) {
					quality = 1;
				}
				if (quality > 100) {
					quality = 100;
				}

				IImageWrapperModule& ImageWrapperModule = FModuleManager::Get().LoadModuleChecked<IImageWrapperModule>(TEXT("ImageWrapper"));
				EImageFormat format = EImageFormat::PNG;
				switch (type)
				{
				case ERTIExportImageType::E_jpg:
					format = EImageFormat::JPEG;
					break;
				case ERTIExportImageType::E_png:
					format = EImageFormat::PNG;
					break;
				}

				TSharedPtr<IImageWrapper> imageWrapper = ImageWrapperModule.CreateImageWrapper(format);
				imageWrapper->SetRaw(rawImageData.GetData(), rawImageData.GetAllocatedSize(), Size.X, Size.Y, ERGBFormat::BGRA, 8);
				compressedBytes = imageWrapper->GetCompressed(quality);
			}

		}

		UE_LOG(LogTemp, Display, TEXT("X2:%i %i"), compressedBytes.Num(), rawImageData.Num());
		rawImageData.Empty();

	}
	return compressedBytes;
}


TArray<uint8> URealTimeImportImage::texture2dToByteArray(UTexture2D* texture2D, ERTIExportImageType type, int32 quality) {

	check(texture2D != nullptr);
	TArray<uint8> compressedBytes;

	if (texture2D->GetPixelFormat() == PF_B8G8R8A8) {

		TArray64<uint8> rawImageData;
		rawImageData.AddUninitialized(texture2D->PlatformData->Mips[0].BulkData.GetBulkDataSize());
		uint8* mipData = (uint8*)texture2D->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
		FMemory::Memcpy(rawImageData.GetData(), mipData, texture2D->PlatformData->Mips[0].BulkData.GetBulkDataSize());
		texture2D->PlatformData->Mips[0].BulkData.Unlock();

		if (rawImageData.Num() > 0) {

			if (type == ERTIExportImageType::E_bmp) {
				rawImageDataToBMPData(compressedBytes, texture2D->GetSizeX(), texture2D->GetSizeY(), (FColor*)rawImageData.GetData(), rawImageData.Num());
			}
			else {

				if (quality < 1) {
					quality = 1;
				}
				if (quality > 100) {
					quality = 100;
				}


				IImageWrapperModule& ImageWrapperModule = FModuleManager::Get().LoadModuleChecked<IImageWrapperModule>(TEXT("ImageWrapper"));
				EImageFormat format = EImageFormat::PNG;
				switch (type)
				{
				case ERTIExportImageType::E_jpg:
					format = EImageFormat::JPEG;
					break;
				case ERTIExportImageType::E_png:
					format = EImageFormat::PNG;
					break;
				}

				TSharedPtr<IImageWrapper> imageWrapper = ImageWrapperModule.CreateImageWrapper(format);
				imageWrapper->SetRaw(rawImageData.GetData(), rawImageData.GetAllocatedSize(), texture2D->GetSizeX(), texture2D->GetSizeY(), ERGBFormat::BGRA, 8);
				compressedBytes = imageWrapper->GetCompressed(quality);
			}

		}
		rawImageData.Empty();

	}
	return compressedBytes;
}





bool URealTimeImportImage::getRawImageData(UTextureRenderTarget2D* textureRenderTarget2D, TArray64<uint8>& rawImageData) {
	FRenderTarget* RenderTarget = textureRenderTarget2D->GameThread_GetRenderTargetResource();
	EPixelFormat Format = textureRenderTarget2D->GetFormat();

	int32 ImageBytes = CalculateImageBytes(textureRenderTarget2D->SizeX, textureRenderTarget2D->SizeY, 0, Format);
	rawImageData.AddUninitialized(ImageBytes);
	bool bReadSuccess = false;
	switch (Format)
	{
	case PF_FloatRGBA:
	{
		TArray<FFloat16Color> FloatColors;
		bReadSuccess = RenderTarget->ReadFloat16Pixels(FloatColors);
		FMemory::Memcpy(rawImageData.GetData(), FloatColors.GetData(), ImageBytes);
	}
	break;
	case PF_B8G8R8A8:
		bReadSuccess = RenderTarget->ReadPixelsPtr((FColor*)rawImageData.GetData());
		break;
	}
	if (bReadSuccess == false)
	{
		rawImageData.Empty();
	}
	return bReadSuccess;
}


void URealTimeImportImage::rawImageDataToBMPData(TArray<uint8>& bmpData, int32 SourceWidth, int32 SourceHeight, const FColor* Data, int64 size) {

	bool bInWriteAlpha = false;
	FIntRect Src(0, 0, SourceWidth, SourceHeight);
	struct FIntRect* SubRectangle = &Src;

	// Types.
#if PLATFORM_SUPPORTS_PRAGMA_PACK
#pragma pack (push,1)
#endif
	struct BITMAPFILEHEADER
	{
		uint16 bfType GCC_PACK(1);
		uint32 bfSize GCC_PACK(1);
		uint16 bfReserved1 GCC_PACK(1);
		uint16 bfReserved2 GCC_PACK(1);
		uint32 bfOffBits GCC_PACK(1);
	} FH;
	struct BITMAPINFOHEADER
	{
		uint32 biSize GCC_PACK(1);
		int32  biWidth GCC_PACK(1);
		int32  biHeight GCC_PACK(1);
		uint16 biPlanes GCC_PACK(1);
		uint16 biBitCount GCC_PACK(1);
		uint32 biCompression GCC_PACK(1);
		uint32 biSizeImage GCC_PACK(1);
		int32  biXPelsPerMeter GCC_PACK(1);
		int32  biYPelsPerMeter GCC_PACK(1);
		uint32 biClrUsed GCC_PACK(1);
		uint32 biClrImportant GCC_PACK(1);
	} IH;
	struct BITMAPV4HEADER
	{
		uint32 bV4RedMask GCC_PACK(1);
		uint32 bV4GreenMask GCC_PACK(1);
		uint32 bV4BlueMask GCC_PACK(1);
		uint32 bV4AlphaMask GCC_PACK(1);
		uint32 bV4CSType GCC_PACK(1);
		uint32 bV4EndpointR[3] GCC_PACK(1);
		uint32 bV4EndpointG[3] GCC_PACK(1);
		uint32 bV4EndpointB[3] GCC_PACK(1);
		uint32 bV4GammaRed GCC_PACK(1);
		uint32 bV4GammaGreen GCC_PACK(1);
		uint32 bV4GammaBlue GCC_PACK(1);
	} IHV4;
#if PLATFORM_SUPPORTS_PRAGMA_PACK
#pragma pack (pop)
#endif

	int32 Width = SubRectangle->Width();
	int32 Height = SubRectangle->Height();
	uint32 BytesPerPixel = bInWriteAlpha ? 4 : 3;
	uint32 BytesPerLine = Align(Width * BytesPerPixel, 4);

	uint32 InfoHeaderSize = sizeof(BITMAPINFOHEADER) + (bInWriteAlpha ? sizeof(BITMAPV4HEADER) : 0);

	// File header.
	FH.bfType = INTEL_ORDER16((uint16)('B' + 256 * 'M'));
	FH.bfSize = INTEL_ORDER32((uint32)(sizeof(BITMAPFILEHEADER) + InfoHeaderSize + BytesPerLine * Height));
	FH.bfReserved1 = INTEL_ORDER16((uint16)0);
	FH.bfReserved2 = INTEL_ORDER16((uint16)0);
	FH.bfOffBits = INTEL_ORDER32((uint32)(sizeof(BITMAPFILEHEADER) + InfoHeaderSize));

	bmpData.Append((uint8*)&FH, sizeof(FH));

	// Info header.
	IH.biSize = INTEL_ORDER32((uint32)InfoHeaderSize);
	IH.biWidth = INTEL_ORDER32((uint32)Width);
	IH.biHeight = INTEL_ORDER32((uint32)Height);
	IH.biPlanes = INTEL_ORDER16((uint16)1);
	IH.biBitCount = INTEL_ORDER16((uint16)BytesPerPixel * 8);
	if (bInWriteAlpha)
	{
		IH.biCompression = INTEL_ORDER32((uint32)3); //BI_BITFIELDS
	}
	else
	{
		IH.biCompression = INTEL_ORDER32((uint32)0); //BI_RGB
	}
	IH.biSizeImage = INTEL_ORDER32((uint32)BytesPerLine * Height);
	IH.biXPelsPerMeter = INTEL_ORDER32((uint32)0);
	IH.biYPelsPerMeter = INTEL_ORDER32((uint32)0);
	IH.biClrUsed = INTEL_ORDER32((uint32)0);
	IH.biClrImportant = INTEL_ORDER32((uint32)0);

	bmpData.Append((uint8*)&IH, sizeof(IH));

	// If we're writing alpha, we need to write the extra portion of the V4 header
	if (bInWriteAlpha)
	{
		IHV4.bV4RedMask = INTEL_ORDER32((uint32)0x00ff0000);
		IHV4.bV4GreenMask = INTEL_ORDER32((uint32)0x0000ff00);
		IHV4.bV4BlueMask = INTEL_ORDER32((uint32)0x000000ff);
		IHV4.bV4AlphaMask = INTEL_ORDER32((uint32)0xff000000);
		IHV4.bV4CSType = INTEL_ORDER32((uint32)'Win ');
		IHV4.bV4GammaRed = INTEL_ORDER32((uint32)0);
		IHV4.bV4GammaGreen = INTEL_ORDER32((uint32)0);
		IHV4.bV4GammaBlue = INTEL_ORDER32((uint32)0);

		bmpData.Append((uint8*)&IHV4, sizeof(IHV4));
	}

	// Colors.
	for (int32 i = SubRectangle->Max.Y - 1; i >= SubRectangle->Min.Y; i--)
	{
		for (int32 j = SubRectangle->Min.X; j < SubRectangle->Max.X; j++)
		{

			bmpData.Append((uint8*)&Data[i * SourceWidth + j].B, 1);
			bmpData.Append((uint8*)&Data[i * SourceWidth + j].G, 1);
			bmpData.Append((uint8*)&Data[i * SourceWidth + j].R, 1);

			if (bInWriteAlpha)
			{
				bmpData.Append((uint8*)&Data[i * SourceWidth + j].A, 1);
			}
		}

		// Pad each row's length to be a multiple of 4 bytes.

		for (uint32 PadIndex = Width * BytesPerPixel; PadIndex < BytesPerLine; PadIndex++)
		{
			uint8 B = 0;
			bmpData.Append(&B, 1);
		}
	}
}


