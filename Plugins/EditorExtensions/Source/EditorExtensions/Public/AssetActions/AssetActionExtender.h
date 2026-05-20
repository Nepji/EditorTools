#pragma once

#include "CoreMinimal.h"
#include "AssetActionUtility.h"
#include "AssetActionExtender.generated.h"

UCLASS()
class EDITOREXTENSIONS_API UAssetActionExtender : public UAssetActionUtility
{
	GENERATED_BODY()

public:
	static void FixUpRedirectors();

	UFUNCTION(CallInEditor)
	void SmartDuplicate(int32 NumOfDuplicates);

	UFUNCTION(CallInEditor)
	void AddOrChangePrefix();

	UFUNCTION(CallInEditor)
	void CleanupName(bool bReplaceBADPrefixes);

	UFUNCTION(CallInEditor)
	void RemoveUnusedAssets();

private:
	FString RequestPrefix(UObject* Obj) const;
	void ChangeAssetPrefix(UObject* Obj, const FString& TargetPrefix, const FString& UsedPrefix) const;
	bool IsPrefixExist(const FString& ObjName, FString& Prefix);
};