// Fill out your copyright notice in the Description page of Project Settings.

#include "AssetActions/AssetActionExtender.h"

#include "AssetToolsModule.h"
#include "EditorUtilityLibrary.h"
#include "EditorAssetLibrary.h"
#include "ObjectTools.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "EditorExtensions/DebugUtils.h"

void UAssetActionExtender::SmartDuplicate(int32 NumOfDuplicates)
{
	if (NumOfDuplicates <= 0)
	{
		ShowMsgDialog(EAppMsgType::Ok, "Number of wishing duplicates must be greater then 0.");
		return;
	}

	TArray<FAssetData> SelectedAssetsData = UEditorUtilityLibrary::GetSelectedAssetData();

	for (const FAssetData& CurrentAssetData : SelectedAssetsData)
	{
		uint32 ToDoCounter = 1;
		for (int32 i = 0; i < NumOfDuplicates; i++)
		{
			const FString SourceAssetPath = CurrentAssetData.ObjectPath.ToString();
			const FString CurrentAssetName = CurrentAssetData.AssetName.ToString();
			const FString NewAssetName = CurrentAssetName + TEXT("_") + FString::FromInt(ToDoCounter);
			const FString NewPathName = FPaths::Combine(CurrentAssetData.PackagePath.ToString(), NewAssetName);

			if (UEditorAssetLibrary::DuplicateAsset(SourceAssetPath, NewPathName))
			{
				++ToDoCounter;
				UEditorAssetLibrary::SaveAsset(NewPathName, false);
			}
			else
			{
				PrintLog("Error while duplicating Asset: " + CurrentAssetName);
			}
		}
		if (ToDoCounter != NumOfDuplicates)
		{
			ShowNotifyInfo("Duplication status: Error (Check Log for details)");
		}
	}
	ShowNotifyInfo("Duplication status: Finished");
}
void UAssetActionExtender::AddOrChangePrefix()
{
	TArray<UObject*> SelectedAssets = UEditorUtilityLibrary::GetSelectedAssets();

	for (UObject* Asset : SelectedAssets)
	{
		const FString TargetPrefix = RequestPrefix(Asset);

		FString UsedPrefix;
		IsPrefixExist(Asset->GetName(), UsedPrefix);
		ChangeAssetPrefix(Asset, TargetPrefix, UsedPrefix);
	}
	ShowNotifyInfo("Prefix Job status: Finished");
}
void UAssetActionExtender::CleanupName()
{
	TArray<UObject*> SelectedAssets = UEditorUtilityLibrary::GetSelectedAssets();
	const EAppReturnType::Type InputReturn = ShowMsgDialog(EAppMsgType::YesNo, "Find and Replace BAD used prefix?");
	if (InputReturn == EAppReturnType::Yes)
	{
		AddOrChangePrefix();
	}
	for (UObject* Asset : SelectedAssets)
	{
		FString AssetName = Asset->GetName();
		FString NewName = AssetName;

		for (const FString& Mess : AssetMess)
		{
			NewName = NewName.Replace(*Mess, TEXT(""));
		}
		if (NewName != AssetName)
		{
			UEditorUtilityLibrary::RenameAsset(Asset, NewName);
		}
	}
	ShowNotifyInfo("Cleaning status: Finished");
}
void UAssetActionExtender::RemoveUnusedAssets()
{
	TArray<FAssetData> SelectedAssetsData = UEditorUtilityLibrary::GetSelectedAssetData();
	TArray<FAssetData> UnusedAssetsData;
	for (FAssetData& SelectedAssetData : SelectedAssetsData)
	{

		TArray<FString> AssetReferences = UEditorAssetLibrary::FindPackageReferencersForAsset(SelectedAssetData.ObjectPath.ToString());
		if (AssetReferences.IsEmpty())
		{
			UnusedAssetsData.Add(SelectedAssetData);
		}
	}
	FixUpRedirectors();
	if (UnusedAssetsData.IsEmpty())
	{
		ShowNotifyInfo("Unused assets not found");
		return;
	}

	const int32 NumOfDeletedAssets = ObjectTools::DeleteAssets(UnusedAssetsData);

	if (NumOfDeletedAssets != 0)
	{
		ShowNotifyInfo("Successfully deleted " + FString::FromInt(NumOfDeletedAssets) + " assets.");
	}
}
FString UAssetActionExtender::RequestPrefix(UObject* Obj) const
{
	FString BadResult = "Unknown_";
	if (!Obj)
	{
		return BadResult;
	}

	const FString* Prefix = AssetPrefixMap.Find(Obj->GetClass()->GetName());
	return Prefix ? *Prefix : *BadResult;
}
void UAssetActionExtender::ChangeAssetPrefix(UObject* Obj, const FString& TargetPrefix, const FString& UsedPrefix) const
{
	FString NewObjName = Obj->GetName();
	if (UsedPrefix.IsEmpty())
	{
		NewObjName.ReplaceInline(*UsedPrefix, *TargetPrefix);
	}
	if (*UsedPrefix != TargetPrefix)
	{
		UEditorUtilityLibrary::RenameAsset(Obj, NewObjName);
	}
}
bool UAssetActionExtender::IsPrefixExist(const FString& ObjName, FString& Prefix)
{
	for (auto AssetPrefix : AssetPrefixMap)
	{
		if (ObjName.StartsWith(AssetPrefix.Value))
		{
			Prefix = AssetPrefix.Value;
			return true;
		}
	}
	return false;
}
void UAssetActionExtender::FixUpRedirectors()
{
	TArray<UObjectRedirector*> RedirectorsFixArray;

	const FAssetRegistryModule& AssetRegistryModule = FModuleManager::Get().LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));

	FARFilter Filter;
	Filter.bRecursivePaths = true;
	Filter.PackagePaths.Emplace("/Game");
	Filter.ClassNames.Emplace("ObjectRedirector");

	TArray<FAssetData> OutRedirectors;
	
	AssetRegistryModule.Get().GetAssets(Filter, OutRedirectors);

	for(const FAssetData& RedirectorData : OutRedirectors)
	{
		if(UObjectRedirector* RedirectorToFix = Cast<UObjectRedirector>(RedirectorData.GetAsset()))
		{
			RedirectorsFixArray.Add(RedirectorToFix);
		}
		
	}

	const FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));

	AssetToolsModule.Get().FixupReferencers(RedirectorsFixArray);
}
