// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "UMG/Public/Components/Button.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "SignupWidget.generated.h"

#define SIGNUP_URL "http://192.168.110.200:3000/user/authentication/signup"
/**
 * 
 */
UCLASS()
class MEE_HTTP_API USignupWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	FHttpModule* Http;

	UFUNCTION()
		void HttpCall_Signup(FString id, FString username, FString password);

		void OnResponseReceived_Signup(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	UPROPERTY(meta = (BindWidget))
		class UEditableText* EditableText_id;
	UPROPERTY(meta = (BindWidget))
		class UEditableText* EditableText_password;
	UPROPERTY(meta = (BindWidget))
		class UEditableText* EditableText_username;

	UPROPERTY(meta = (BindWidget))
		class UButton* Button_Signup;
	UPROPERTY(meta = (BindWidget))
		class UButton* Button_Cancle;

	/*UPROPERTY(meta = (BindWidget))
		class UTextWidget* TextBlock_Message;*/

	UFUNCTION()
		void OnClick_Signup();

	UFUNCTION()
		void OnClick_Cancle();
};
