// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "UMG/Public/Components/Button.h"
#include "Components/Button.h"
#include "Mee_GameInstance.h"
#include "LoginWidget.generated.h"

#define LOGIN_URL "http://192.168.110.200:3000/user/authentication/signin"
#define SIGNUP_URL "http://192.168.110.200:3000/user/authentication/signup"
#define GET_USERDATA_URL "http://192.168.110.200:3000/user/information/"

/**
 *
 */
UCLASS()
class MEE_HTTP_API ULoginWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	FHttpModule* Http;

	UMee_GameInstance* GameInstance;

	UFUNCTION()
		void HttpCall_Login(FString id, FString password);
	UFUNCTION()
		void HttpCall_Signup(FString id, FString username, FString password);

	UFUNCTION()
		void HttpCall_GetUserData();

	// http GET 요청 직후 호출되는 콜백함수
	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void OnResponseReceived_GetUserData(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
		class UEditableText* EditableText_id;
	UPROPERTY(meta = (BindWidget))
		class UEditableText* EditableText_password;
	UPROPERTY(meta = (BindWidget))
		class UEditableText* EditableText_username;


	UPROPERTY(meta = (BindWidget))
		class UButton* Button_Signup;
	UPROPERTY(meta = (BindWidget))
		class UButton* Button_Login;

	UFUNCTION()
		void OnClick_Signup();

	UFUNCTION()
		void OnClick_Login();

};