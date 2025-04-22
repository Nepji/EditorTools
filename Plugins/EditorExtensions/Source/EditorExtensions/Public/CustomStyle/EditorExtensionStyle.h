#pragma once

#include "Styling/SlateStyle.h"


class FEditorExtensionStyle
{
public:
	static void InitializeIcons();
	static void ShutDown();

	static FName GetStyleSetName();


private:
	static TSharedRef<FSlateStyleSet> CreateSlateStyleSlate();
private:
	static FName StyleSetName;
	static TSharedPtr<FSlateStyleSet> CreatedSlateStyleSet;
};