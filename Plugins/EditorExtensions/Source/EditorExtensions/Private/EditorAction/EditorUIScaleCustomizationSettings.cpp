// Fill out your copyright notice in the Description page of Project Settings.


#include "EditorAction/EditorUIScaleCustomizationSettings.h"

void UEditorUIScaleCustomizationSettings::InitUIScaleCustomizationSettings()
{
	if (!IsRunningCommandlet() && FSlateApplication::IsInitialized())
	{
		const float CurrentScale = GetDefault<UEditorUIScaleCustomizationSettings>()->UIScale;
		FSlateApplication::Get().SetApplicationScale(CurrentScale);
	}
}

void UEditorUIScaleCustomizationSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.GetPropertyName() == "UIScale")
	{
		FSlateApplication::Get().SetApplicationScale(UIScale);
	}
}