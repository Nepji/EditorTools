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

	void GetUnusedAssetData(const TArray<TSharedPtr<FAssetData>>& AssetDataToFilter, TArray<TSharedPtr<FAssetData>>& FilteredAssetData);
	void GetDuplicatedAssetData(const TArray<TSharedPtr<FAssetData>>& AssetDataToFilter, TArray<TSharedPtr<FAssetData>>& FilteredAssetData);

	void SyncCBToClickedAsset(const FString& AssetPath);

	bool CheckIsActorSelectionLocked(AActor* ActorToProcess);
	void ProcessLockingForOutliner(AActor* ActorToProcess, bool bShouldLock);
private:
	TSharedRef<FExtender> CustomCBMenuExtender(const TArray<FString>& SelectedPaths);

	TArray<FString> FoldersPaths;

	FName LockedActorTagName = "Locked";
	TWeakObjectPtr<class UEditorActorSubsystem> WeakEditorActorSubsystem;

	TSharedPtr<class FUICommandList> CustomUICommands;
private:
	void InitCBMenuExtension();
	void AddCBMenuEntry(class FMenuBuilder& MenuBuilder);

	void OnDeleteUnusedAssets();
	void OnEmptyFoldersAndAssetsDelete();
	void OnAdvancedDeletion();
	
	void RegisterAdvancedDeletionTab();
	TSharedRef<SDockTab> OnSpawnAdvanceDeletion(const FSpawnTabArgs& SpawnTabArgs);

	TArray<TSharedPtr<FAssetData>> GetAllAssetDataUnderSelectedFolder();

	void InitLevelEditorExtension();

	TSharedRef<FExtender> CustomLevelEditorMenuExtender(const TSharedRef<FUICommandList> UICommandList, const TArray<AActor*> SelectedActors);
	void AddSVMenuEntry(FMenuBuilder& MenuBuilder);
	
	void OnLockActorSelectionButtonClicked();
	void OnUnlockActorSelectionButtonClicked();

	void InitCustomSelectionEvent();
	void OnActorSelected(UObject* SelectedObject);

	void LockActorSelection(AActor* ActorToProcess);
	void UnlockActorSelection(AActor* ActorToProcess);

	void RefreshSceneOutliner();

	bool GetEditorActorSubSystem();

	void InitCustomUICommands();

	void InitSceneOutlinerExtension();
	TSharedRef<class ISceneOutlinerColumn> OnCreateSelectionLockColumn(class ISceneOutliner&  SceneOutliner);
	void UnRegisterSceneOutlinerColumn();
	
	
};
