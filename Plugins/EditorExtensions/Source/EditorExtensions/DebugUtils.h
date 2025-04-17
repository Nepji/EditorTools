#pragma once

#include "IToolMenusEditorModule.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "Framework/Notifications/NotificationManager.h"

namespace DebugHepler
{
	static void Print(const FString& Message, const FColor& Color = FColor::Red)
	{
		if(GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1,5.f,Color,Message);
		}
	}

	static void PrintLog(const FString& Message)
	{
		UE_LOG(LogTemp,Warning,TEXT("%s"), *Message);
	}


	static EAppReturnType::Type ShowMsgDialog(EAppMsgType::Type MsgType, const FString& Message, bool bShowMsgAsWarning = false)
	{
		const FString TitleMsg = bShowMsgAsWarning?"Warning":"Interaction";
		const FText TitleText = FText::FromString(TitleMsg);

		return FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(Message), &TitleText);

	}

	static void ShowNotifyInfo(const FString& Message)
	{
		FNotificationInfo NotifyInfo(FText::FromString(Message));
		NotifyInfo.bUseLargeFont = true;
		NotifyInfo.FadeOutDuration = 7.f;

		FSlateNotificationManager::Get().AddNotification(NotifyInfo);
	}
}

