#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "AssetActionExtenderSettings.generated.h"

USTRUCT()
struct FAssetPrefixData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Naming")
	TSoftClassPtr<UObject> AssetClass;

	UPROPERTY(EditAnywhere, Category = "Naming")
	FString Prefix;
};

UCLASS(Config=EditorExtensions, defaultconfig, meta = (DisplayName="Asset Naming Settings"))
class EDITOREXTENSIONS_API UAssetActionExtenderSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UAssetActionExtenderSettings();
	
	virtual FName GetCategoryName() const override { return "Plugins"; }
	

	UPROPERTY(EditAnywhere, Config, Category = "Naming Configuration", meta = (ToolTip = "Material -> M_"))
	TArray<FAssetPrefixData> AssetPrefixes;

	UPROPERTY(EditAnywhere, Config, Category = "Naming Configuration", meta = (ToolTip = "Cleaning targets"))
	TArray<FString> AssetMess;
	
	UPROPERTY(EditAnywhere, Config, Category = "Naming Configuration")
	FString DefaultUnknownPrefixName = TEXT("Unknown_");
};
