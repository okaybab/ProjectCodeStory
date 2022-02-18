// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "ChattingWidget.generated.h"

/**
 * 
 */
UCLASS()
class MEE_HTTP_API UChattingWidget : public UUserWidget
{
	GENERATED_BODY()
	
	virtual void NativeConstruct() override;

public:
	UPROPERTY(meta = (BindWidget))
		class UScrollBox* ScrollBox_Recv;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UEditableText* EditableText_Input;

	UPROPERTY(meta = (BindWidget))
		class UButton* Button_Send;

	UFUNCTION()
		void OnClick_Send();

	UFUNCTION(BlueprintCallable)
		void SetScrollBox(FString _Message);
};
