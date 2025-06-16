#include "CustomStyle/EditorExtensionStyle.h"

#include "SSceneOutliner.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleRegistry.h"

FName FEditorExtensionStyle::StyleSetName = FName("EditorExtensionStyle");
TSharedPtr<FSlateStyleSet> FEditorExtensionStyle::CreatedSlateStyleSet = nullptr;

void FEditorExtensionStyle::InitializeIcons()
{
	if (!CreatedSlateStyleSet.IsValid())
	{
		CreatedSlateStyleSet = CreateSlateStyleSlate();
	}
	FSlateStyleRegistry::RegisterSlateStyle(*CreatedSlateStyleSet);
}
void FEditorExtensionStyle::ShutDown()
{
	if (CreatedSlateStyleSet.IsValid())
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*CreatedSlateStyleSet);
		CreatedSlateStyleSet.Reset();
	}
}

FName FEditorExtensionStyle::GetStyleSetName()
{
	return StyleSetName;
}
TSharedRef<FSlateStyleSet> FEditorExtensionStyle::CreateSlateStyleSlate()
{
	const FVector2D Icon16x16(16.f, 16.f);

	TSharedRef<FSlateStyleSet> CustomStyleSet = MakeShareable(new FSlateStyleSet(StyleSetName));

	const FString IconDirectory = IPluginManager::Get().FindPlugin(TEXT("EditorExtensions"))->GetBaseDir() / "Resources";
	CustomStyleSet->SetContentRoot(IconDirectory);

	CustomStyleSet->Set("ContentBrowser.DeleteUnusedAssets", new FSlateImageBrush(IconDirectory / "ToolIcon.png", Icon16x16));
	CustomStyleSet->Set("ContentBrowser.DeleteEmptyFolders", new FSlateImageBrush(IconDirectory / "ToolIcon.png", Icon16x16));
	CustomStyleSet->Set("ContentBrowser.AdvanceDeletion", new FSlateImageBrush(IconDirectory / "AdvancedDeletion.png", Icon16x16));
	CustomStyleSet->Set("ContentBrowser.Lock", new FSlateImageBrush(IconDirectory / "Lock.png", Icon16x16));
	CustomStyleSet->Set("ContentBrowser.Unlock", new FSlateImageBrush(IconDirectory / "Unlock.png", Icon16x16));

	const FCheckBoxStyle SelectionLockToggleButtonStyle = FCheckBoxStyle()
															  .SetCheckBoxType(ESlateCheckBoxType::ToggleButton)
															  .SetPadding(FMargin(10.f))

															  .SetUncheckedImage(FSlateImageBrush(IconDirectory / "Lock.png", Icon16x16, FStyleColors::White25))
															  .SetUncheckedHoveredImage(FSlateImageBrush(IconDirectory / "Lock.png", Icon16x16, FStyleColors::AccentBlue))
															  .SetUncheckedPressedImage(FSlateImageBrush(IconDirectory / "Lock.png", Icon16x16, FStyleColors::Foreground))

															  .SetCheckedImage(FSlateImageBrush(IconDirectory / "Lock.png", Icon16x16, FStyleColors::Foreground))
															  .SetCheckedHoveredImage(FSlateImageBrush(IconDirectory / "Lock.png", Icon16x16, FStyleColors::AccentBlack))
															  .SetCheckedPressedImage(FSlateImageBrush(IconDirectory / "Lock.png", Icon16x16, FStyleColors::AccentGray));

	CustomStyleSet->Set("SceneOutliner.SelectionLock", SelectionLockToggleButtonStyle);

	return CustomStyleSet;
}