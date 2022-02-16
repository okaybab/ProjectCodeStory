// Fill out your copyright notice in the Description page of Project Settings.


#include "ChattingWidget.h"
#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/UMGStyle.h" 
#include "Runtime/UMG/Public/Slate/SObjectWidget.h" 
#include "Runtime/UMG/Public/IUMGModule.h" 
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "UMG/Public/Components/Button.h"


void UChattingWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Button_Send->OnClicked.AddDynamic(this, &UChattingWidget::OnClick_Send);
}

void UChattingWidget::OnClick_Send()
{
	//실제로 메세지를 전송하는 'SendMessageToChannel'함수는 BP에서 호출한다.
	EditableText_Input->SetText(FText::GetEmpty());
}

void UChattingWidget::SetScrollBox(FString _Message)
{
	UTextBlock* NewTextBlock = NewObject<UTextBlock>(ScrollBox_Recv);
	NewTextBlock->SetText(FText::FromString(""));

	ScrollBox_Recv->AddChild(NewTextBlock);
	ScrollBox_Recv->ScrollToEnd();
}
