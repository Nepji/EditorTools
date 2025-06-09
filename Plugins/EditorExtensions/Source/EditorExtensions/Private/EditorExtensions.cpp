// Copyright Epic Games, Inc. All Rights Reserved.

#include "EditorExtensions.h"

#include "ContentBrowserModule.h"
#include "EditorAssetLibrary.h"
#include "ObjectTools.h"
#include "AssetActions/AssetActionExtender.h"
#include "CustomStyle/EditorExtensionStyle.h"
#include "EditorExtensions/DebugUtils.h"
#include "SlateWidgets/AdvancedDeletionWidget.h"

#define LOCTEXT_NAMESPACE "FEditorExtensionsModule"

void FEditorExtensionsModule::StartupModule()
{
	FEditorExtensionStyle::InitializeIcons();
	InitCBMenuExtension();
	RegisterAdvancedDeletionTab();
}

void FEditorExtensionsModule::ShutdownModule()
{
	FEditorExtensionStyle::ShutDown();
}
bool FEditorExtensionsModule::DeleteAsset(const FAssetData& AssetData)
{
	const TArray<FAssetData> AssetArray = { AssetData };
	return ObjectTools::DeleteAssets(AssetArray) != 0;
}
bool FEditorExtensionsModule::DeleteAssets(TArray<FAssetData> SelectedAssetData)
{
	return ObjectTools::DeleteAssets(SelectedAssetData) != 0;
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
	// Delete Unused Assets
	MenuBuilder.AddMenuEntry(
		FText::FromString(TEXT("Delete Unused Assets")),
		FText::FromString(TEXT("Safety delete all unused assets under folder")),
		FSlateIcon(FEditorExtensionStyle::GetStyleSetName(), "ContentBrowser.DeleteUnusedAssets"),
		FExecuteAction::CreateRaw(this, &FEditorExtensionsModule::OnDeleteUnusedAssets));

	// Delete Unused Folders and Assets
	MenuBuilder.AddMenuEntry(
		FText::FromString(TEXT("Delete Unused Folders and Assets")),
		FText::FromString(TEXT("Safety delete all unused folders and assets under selected folder")),
		FSlateIcon(FEditorExtensionStyle::GetStyleSetName(), "ContentBrowser.DeleteEmptyFolders"),
		FExecuteAction::CreateRaw(this, &FEditorExtensionsModule::OnEmptyFoldersAndAssetsDelete));

	// Advance Deletion
	MenuBuilder.AddMenuEntry(
		FText::FromString(TEXT("Advance Deletion")),
		FText::FromString(TEXT("Not implemented yet.")),
		FSlateIcon(FEditorExtensionStyle::GetStyleSetName(), "ContentBrowser.AdvanceDeletion"),
		FExecuteAction::CreateRaw(this, &FEditorExtensionsModule::OnAdvancedDeletion));
}
void FEditorExtensionsModule::OnDeleteUnusedAssets()
{
	if (FoldersPaths.Num() <= 0)
	{
		return;
	}
	if (FoldersPaths.Num() > 1)
	{
		DebugHelper::ShowMsgDialog(EAppMsgType::Ok, TEXT("For now you can only select one folder"));
		return;
	}

	UAssetActionExtender::FixUpRedirectors();

	DebugHelper::Print(TEXT("Currently running through folder: ") + FoldersPaths[ 0 ], FColor::Blue);
	TArray<FString> AssetList = UEditorAssetLibrary::ListAssets(FoldersPaths[ 0 ]);

	if (AssetList.IsEmpty())
	{
		DebugHelper::ShowNotifyInfo(TEXT("No asset found under selected folder"));
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
			DebugHelper::ShowNotifyInfo(TEXT("Don`t touch root folders"));
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
		DebugHelper::ShowNotifyInfo(TEXT("No unused asset found under selected folder"));
		return;
	}

	const int32 NumOfDeletedAssets = ObjectTools::DeleteAssets(UnusedAssetsData);

	if (NumOfDeletedAssets > 0)
	{
		DebugHelper::ShowNotifyInfo("Successfully deleted " + FString::FromInt(NumOfDeletedAssets) + " assets.");
	}
}
void FEditorExtensionsModule::OnEmptyFoldersAndAssetsDelete()
{
	OnDeleteUnusedAssets();

	UAssetActionExtender::FixUpRedirectors();

	TArray<FString> FolderPathSelected = UEditorAssetLibrary::ListAssets(FoldersPaths[ 0 ], true, true);
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
			DebugHelper::ShowNotifyInfo(TEXT("Don`t touch root folders"));
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
		DebugHelper::ShowNotifyInfo(TEXT("No unused folder found under selected folder"));
		return;
	}

	const EAppReturnType::Type ConfirmReturn = DebugHelper::ShowMsgDialog(EAppMsgType::OkCancel, TEXT("Found ") + FString::FromInt(EmptyFolderPathsArray.Num()) + "\nList: " + EmptyFolderPathsName + "\n\nConfirm to delete?");

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
	DebugHelper::ShowNotifyInfo("Successfully deleted " + FString::FromInt(FolderCounter) + " folders.");
}
void FEditorExtensionsModule::OnAdvancedDeletion()
{
	UAssetActionExtender::FixUpRedirectors();

	FGlobalTabmanager::Get()->TryInvokeTab(FName("AdvancedDeletion"));
}
void FEditorExtensionsModule::RegisterAdvancedDeletionTab()
{
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
								FName("AdvancedDeletion"),
								FOnSpawnTab::CreateRaw(this, &FEditorExtensionsModule::OnSpawnAdvanceDeletion))
		.SetDisplayName(FText::FromString(TEXT("Advanced Deletion")))
		.SetIcon(FSlateIcon(FEditorExtensionStyle::GetStyleSetName(), "ContentBrowser.AdvanceDeletion"));
}
TSharedRef<SDockTab> FEditorExtensionsModule::OnSpawnAdvanceDeletion(const FSpawnTabArgs& SpawnTabArgs)
{
	return SNew(SDockTab).TabRole(ETabRole::NomadTab)
		[ SNew(SAdvanceDeletionsTab)
			  .AssetsDataToStore(GetAllAssetDataUnderSelectedFolder()) ];
}
TArray<TSharedPtr<FAssetData>> FEditorExtensionsModule::GetAllAssetDataUnderSelectedFolder()
{
	TArray<TSharedPtr<FAssetData>> AvailableAssetData;

	if (FoldersPaths.IsEmpty())
	{
		return AvailableAssetData;
	}

	TArray<FString> AssetPathNames = UEditorAssetLibrary::ListAssets(FoldersPaths[ 0 ]);

	for (const FString& AssetPath : AssetPathNames)
	{
		if (AssetPath.Contains(TEXT("Developers"))
			|| AssetPath.Contains(TEXT("Colletions"))
			|| AssetPath.Contains(TEXT("__ExternalActors__"))
			|| AssetPath.Contains(TEXT("__ExternalObjects__")))
		{
			DebugHelper::ShowNotifyInfo(TEXT("Don`t touch root folders"));
			continue;
		}

		if (!UEditorAssetLibrary::DoesAssetExist(AssetPath))
		{
			continue;
		}

		const FAssetData Data = UEditorAssetLibrary::FindAssetData(AssetPath);
		AvailableAssetData.Add(MakeShared<FAssetData>(Data));
	}
	return AvailableAssetData;
}
void FEditorExtensionsModule::GetUnusedAssetData(const TArray<TSharedPtr<FAssetData>>& AssetDataToFilter, TArray<TSharedPtr<FAssetData>>& FilteredAssetData)
{
	FilteredAssetData.Empty();
	for (const TSharedPtr<FAssetData>& Data : AssetDataToFilter)
	{
		TArray<FString> AssetRef =
			UEditorAssetLibrary::FindPackageReferencersForAsset(Data->GetObjectPathString());

		if (!AssetRef.IsEmpty())
		{
			continue;
		}

		FilteredAssetData.Add(Data);
	}
}
void FEditorExtensionsModule::GetDuplicatedAssetData(const TArray<TSharedPtr<FAssetData>>& AssetDataToFilter, TArray<TSharedPtr<FAssetData>>& FilteredAssetData)
{
	FilteredAssetData.Empty();

	TMultiMap<FString, TSharedPtr<FAssetData>> AssetsInfoMultiMap;

	for (const TSharedPtr<FAssetData>& Data : AssetDataToFilter)
	{
		AssetsInfoMultiMap.Emplace(Data->AssetName.ToString(), Data);
	}

	for (const TSharedPtr<FAssetData>& Data : AssetDataToFilter)
	{
		TArray<TSharedPtr<FAssetData>> OutAssetData;
		AssetsInfoMultiMap.MultiFind(Data->AssetName.ToString(), OutAssetData);

		if (OutAssetData.Num() <= 1)
		{
			continue;
		}

		for (const TSharedPtr<FAssetData>& SameData : OutAssetData)
		{
			if (SameData.IsValid())
			{
				FilteredAssetData.AddUnique(SameData);
			}
		}
	}
}
void FEditorExtensionsModule::SyncCBToClickedAsset(const FString& AssetPath)
{
	const TArray<FString> AssetPaths = { AssetPath };
	UEditorAssetLibrary::SyncBrowserToObjects(AssetPaths);
}
#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FEditorExtensionsModule, EditorExtensions)