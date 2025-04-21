// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FEditorExtensionsModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

public:
	bool DeleteAsset(const FAssetData& AssetData);
	bool DeleteAssets(TArray<FAssetData> SelectedAssetData);

	void GetFilteredAssetData(const TArray<TSharedPtr<FAssetData>>& AssetDataToFilter, TArray<TSharedPtr<FAssetData>>& FilteredAssetData);
private:
	TSharedRef<FExtender> CustomCBMenuExtender(const TArray<FString>& SelectedPaths);

	TArray<FString> FoldersPaths;
private:
	void InitCBMenuExtension();
	void AddCBMenuEntry(class FMenuBuilder& MenuBuilder);

	void OnDeleteUnusedAssets();
	void OnEmptyFoldersAndAssetsDelete();
	void OnAdvancedDeletion();
	
	void RegisterAdvancedDeletionTab();
	TSharedRef<SDockTab> OnSpawnAdvanceDeletion(const FSpawnTabArgs& SpawnTabArgs);

	TArray<TSharedPtr<FAssetData>> GetAllAssetDataUnderSelectedFolder();
	
};
