// Copyright Epic Games, Inc. All Rights Reserved.

#include "EditorExtensions.h"

#include "ContentBrowserModule.h"
#include "EditorAssetLibrary.h"
#include "ObjectTools.h"
#include "AssetActions/AssetActionExtender.h"
#include "EditorExtensions/DebugUtils.h"

#define LOCTEXT_NAMESPACE "FEditorExtensionsModule"

void FEditorExtensionsModule::StartupModule()
{
	InitCBMenuExtension();
}

void FEditorExtensionsModule::ShutdownModule()
{
}
TSharedRef<FExtender> FEditorExtensionsModule::CustomCBMenuExtender(const TArray<FString>& SelectedPaths)
{
	TSharedRef<FExtender> MenuExtender(new FExtender());

	if (SelectedPaths.Num() > 0)
	{
		FoldersPaths = SelectedPaths;

		MenuExtender->AddMenuExtension(
			FName("Delete"),
			EExtensionHook::After,
			TSharedPtr<FUICommandList>(),
			FMenuExtensionDelegate::CreateRaw(this, &FEditorExtensionsModule::AddCBMenuEntry));
	}

	return MenuExtender;
}
void FEditorExtensionsModule::InitCBMenuExtension()
{
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));

	TArray<FContentBrowserMenuExtender_SelectedPaths>& BrowserContentExtenders = ContentBrowserModule.GetAllPathViewContextMenuExtenders();

	BrowserContentExtenders.Add(FContentBrowserMenuExtender_SelectedPaths::CreateRaw(this,
		&FEditorExtensionsModule::CustomCBMenuExtender));
}
void FEditorExtensionsModule::AddCBMenuEntry(FMenuBuilder& MenuBuilder)
{
	MenuBuilder.AddMenuEntry(
		FText::FromString(TEXT("Delete Unused Assets")),
		FText::FromString(TEXT("Safety delete all unused assets under folder")),
		FSlateIcon(),
		FExecuteAction::CreateRaw(this, &FEditorExtensionsModule::OnDeleteUnusedAssetsDelete));

	MenuBuilder.AddMenuEntry(
		FText::FromString(TEXT("Delete Unused Folders and Assets")),
		FText::FromString(TEXT("Safety delete all unused folders and assets under selected folder")),
		FSlateIcon(),
		FExecuteAction::CreateRaw(this, &FEditorExtensionsModule::OnEmptyFoldersAndAssetsDelete));
}
void FEditorExtensionsModule::OnDeleteUnusedAssetsDelete()
{
	if (FoldersPaths.Num() <= 0)
	{
		return;
	}
	if (FoldersPaths.Num() > 1)
	{
		DebugHepler::ShowMsgDialog(EAppMsgType::Ok, TEXT("For now you can only select one folder"));
		return;
	}

	UAssetActionExtender::FixUpRedirectors();

	DebugHepler::Print(TEXT("Currently running through folder: ") + FoldersPaths[0], FColor::Blue);
	TArray<FString> AssetList = UEditorAssetLibrary::ListAssets(FoldersPaths[0]);

	if (AssetList.IsEmpty())
	{
		DebugHepler::ShowNotifyInfo(TEXT("No asset found under selected folder"));
		return;
	}

	TArray<FAssetData> UnusedAssetsData;
	for (const FString& AssetPathName : AssetList)
	{
		if (AssetPathName.Contains(TEXT("Developers"))
			|| AssetPathName.Contains(TEXT("Colletions"))
			|| AssetPathName.Contains(TEXT("__ExternalActors__"))
			|| AssetPathName.Contains(TEXT("__ExternalObjects__")))
		{
			DebugHepler::ShowNotifyInfo(TEXT("Don`t touch root folders"));
			continue;
		}

		if (!UEditorAssetLibrary::DoesAssetExist(AssetPathName))
		{
			continue;
		}

		TArray<FString> AssetsReferences = UEditorAssetLibrary::FindPackageReferencersForAsset(AssetPathName);
		if (AssetsReferences.IsEmpty())
		{
			const FAssetData UnusedAssetData = UEditorAssetLibrary::FindAssetData(AssetPathName);
			UnusedAssetsData.Add(UnusedAssetData);
		}
	}

	if (UnusedAssetsData.IsEmpty())
	{
		DebugHepler::ShowNotifyInfo(TEXT("No unused asset found under selected folder"));
		return;
	}

	const int32 NumOfDeletedAssets = ObjectTools::DeleteAssets(UnusedAssetsData);

	if (NumOfDeletedAssets > 0)
	{
		DebugHepler::ShowNotifyInfo("Successfully deleted " + FString::FromInt(NumOfDeletedAssets) + " assets.");
	}
}
void FEditorExtensionsModule::OnEmptyFoldersAndAssetsDelete()
{
	OnDeleteUnusedAssetsDelete();
	
	UAssetActionExtender::FixUpRedirectors();

	TArray<FString> FolderPathSelected = UEditorAssetLibrary::ListAssets(FoldersPaths[0], true, true);
	uint32 FolderCounter = 0;

	FString EmptyFolderPathsName;
	TArray<FString> EmptyFolderPathsArray;

	for (const FString& FolderPath : FolderPathSelected)
	{
		if (FolderPath.Contains(TEXT("Developers"))
			|| FolderPath.Contains(TEXT("Colletions"))
			|| FolderPath.Contains(TEXT("__ExternalActors__"))
			|| FolderPath.Contains(TEXT("__ExternalObjects__")))
		{
			DebugHepler::ShowNotifyInfo(TEXT("Don`t touch root folders"));
			continue;
		}

		if (!UEditorAssetLibrary::DoesDirectoryExist(FolderPath))
		{
			continue;
		}

		if (!UEditorAssetLibrary::DoesDirectoryHaveAssets(FolderPath))
		{
			EmptyFolderPathsName.Append(FolderPath + TEXT("\n"));
			EmptyFolderPathsArray.Add(FolderPath);
		}
	}

	if (EmptyFolderPathsArray.IsEmpty())
	{
		DebugHepler::ShowNotifyInfo(TEXT("No unused folder found under selected folder"));
		return;
	}

	EAppReturnType::Type ConfirmReturn = DebugHepler::ShowMsgDialog(EAppMsgType::OkCancel, TEXT("Found ") + FString::FromInt(EmptyFolderPathsArray.Num()) + "\nList: " + EmptyFolderPathsName + "\n\nConfirm to delete?");

	if (ConfirmReturn == EAppReturnType::Cancel)
	{
		return;
	}
	for (const FString& EmptyFolderPath : EmptyFolderPathsArray)
	{
		if (UEditorAssetLibrary::DeleteDirectory(EmptyFolderPath))
		{
			++FolderCounter;
		}
	}
	DebugHepler::ShowNotifyInfo("Successfully deleted " + FString::FromInt(FolderCounter) + " folders.");
}
#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FEditorExtensionsModule, EditorExtensions)