#include "AssetActions/AssetActionExtender.h"
#include "AssetToolsModule.h"
#include "EditorUtilityLibrary.h"
#include "EditorAssetLibrary.h"
#include "ObjectTools.h"
#include "AssetActions/AssetActionExtenderSettings.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "EditorExtensions/DebugUtils.h"

void UAssetActionExtender::SmartDuplicate(const int32 NumOfDuplicates)
{
    if (NumOfDuplicates <= 0)
    {
       DebugHelper::ShowMsgDialog(EAppMsgType::Ok, "Number of wishing duplicates must be greater then 0.");
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
             DebugHelper::PrintLog("Error while duplicating Asset: " + CurrentAssetName);
          }
       }
       if (ToDoCounter != NumOfDuplicates)
       {
          DebugHelper::ShowNotifyInfo("Duplication status: Error (Check Log for details)");
       }
    }
    DebugHelper::ShowNotifyInfo("Duplication status: Finished");
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
    DebugHelper::ShowNotifyInfo("Prefix Job status: Finished");
}

void UAssetActionExtender::CleanupName(bool bReplaceBADPrefixes)
{
    TArray<UObject*> SelectedAssets = UEditorUtilityLibrary::GetSelectedAssets();
    if (bReplaceBADPrefixes)
    {
       AddOrChangePrefix();
    }

    const UAssetActionExtenderSettings* Settings = GetDefault<UAssetActionExtenderSettings>();

    for (UObject* Asset : SelectedAssets)
    {
       FString AssetName = Asset->GetName();
       FString NewName = AssetName;

       for (const FString& Mess : Settings->AssetMess)
       {
          NewName = NewName.Replace(*Mess, TEXT(""));
       }
       if (NewName != AssetName)
       {
          UEditorUtilityLibrary::RenameAsset(Asset, NewName);
       }
    }
    DebugHelper::ShowNotifyInfo("Cleaning status: Finished");
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
       DebugHelper::ShowNotifyInfo("Unused assets not found");
       return;
    }

    const int32 NumOfDeletedAssets = ObjectTools::DeleteAssets(UnusedAssetsData);

    if (NumOfDeletedAssets != 0)
    {
      DebugHelper::ShowNotifyInfo("Successfully deleted " + FString::FromInt(NumOfDeletedAssets) + " assets.");
    }
}

FString UAssetActionExtender::RequestPrefix(UObject* Obj) const
{
	const UAssetActionExtenderSettings* Settings = GetDefault<UAssetActionExtenderSettings>();

	if (!Obj)
	{
		return Settings->DefaultUnknownPrefixName;
	}

	for (UClass* CurrentClass = Obj->GetClass(); CurrentClass; CurrentClass = CurrentClass->GetSuperClass())
	{
		for (const FAssetPrefixData& Data : Settings->AssetPrefixes)
		{
			UClass* MapClass = Data.AssetClass.Get();
			if (!MapClass)
			{
				MapClass = Data.AssetClass.LoadSynchronous();
			}

			if (MapClass == CurrentClass)
			{
				return Data.Prefix;
			}
		}
	}

	return Settings->DefaultUnknownPrefixName;
}

void UAssetActionExtender::ChangeAssetPrefix(UObject* Obj, const FString& TargetPrefix, const FString& UsedPrefix) const
{
    FString NewObjName = Obj->GetName();
    if (!UsedPrefix.IsEmpty())
    {
       NewObjName.ReplaceInline(*UsedPrefix, *TargetPrefix);
    }
    else
    {
       NewObjName = TargetPrefix + NewObjName;
    }
    if (UsedPrefix != TargetPrefix)
    {
       UEditorUtilityLibrary::RenameAsset(Obj, NewObjName);
    }
}

bool UAssetActionExtender::IsPrefixExist(const FString& ObjName, FString& Prefix)
{
	const UAssetActionExtenderSettings* Settings = GetDefault<UAssetActionExtenderSettings>();
	bool bFound = false;
	int32 LongestPrefixLength = 0;

	for (const FAssetPrefixData& Data : Settings->AssetPrefixes)
	{
		if (ObjName.StartsWith(Data.Prefix) && Data.Prefix.Len() > LongestPrefixLength)
		{
			Prefix = Data.Prefix;
			LongestPrefixLength = Data.Prefix.Len();
			bFound = true;
		}
	}
	return bFound;
}

void UAssetActionExtender::FixUpRedirectors()
{
    TArray<UObjectRedirector*> RedirectorsFixArray;
    const FAssetRegistryModule& AssetRegistryModule = FModuleManager::Get().LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));

    FARFilter Filter;
    Filter.bRecursivePaths = true;
    Filter.PackagePaths.Emplace("/Game");
    Filter.ClassPaths.Emplace("ObjectRedirector");

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