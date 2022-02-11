// Fill out your copyright notice in the Description page of Project Settings.


#include "ChattingWidget.h"

void UChattingWidget::NativeConstruct()
{
	Button_Send->OnClicked.AddDynamic(this, &UChattingWidget::OnClick_Send);
}

void UChattingWidget::OnClick_Send()
{
	//GEngine->AddOnScreenDebugMessage(-1, 100.0f, FColor::Red, TEXT("Send"));
}
