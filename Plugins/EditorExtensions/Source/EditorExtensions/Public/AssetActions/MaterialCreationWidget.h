// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "Materials/MaterialExpressionTextureSample.h"
#include "MaterialCreationWidget.generated.h"

UENUM(BlueprintType)
enum class E_ChannelPackingType : uint8
{
	ECPT_NoChannelPacking UMETA (DisplayName = "No Channel Packing"),

	ECPT_ORM UMETA (DisplayName = "OcclusionRoughnessMetallic"),

	ECPT_MAX UMETA (DisplayName = "DefaultMAX")
};


UCLASS()
class EDITOREXTENSIONS_API UMaterialCreationWidget : public UEditorUtilityWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void CreateMaterialFromSelectedTexture();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Create Material")
	E_ChannelPackingType ChannelPackingType = E_ChannelPackingType::ECPT_NoChannelPacking;	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Create Material")
	bool bCustomMaterialName = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Create Material", meta = (EditCondition = "bCustomMaterialName"))
	FString MaterialName = TEXT("M_NewMaterial");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Create Material")
	bool bCreateMaterialInstanceOnFinish = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Create Material", meta = (EditCondition = "bCreateMaterialInstanceOnFinish"))
	bool bCustomMaterialInstanceName = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Create Material", meta = (EditCondition = "bCustomMaterialInstanceName && bCreateMaterialInstanceOnFinish"))
	FString MaterialInstanceName = TEXT("MI_NewMaterialInstance");
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Supported Texture Names")
	TArray<FString> BaseColorArray = {
		TEXT("_BaseColor"),
		TEXT("_Albedo"),
		TEXT("_Diffuse"),
		TEXT("_diff")
	};

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Supported Texture Names")
	TArray<FString> MetallicArray = {
		TEXT("_Metallic"),
		TEXT("_metal")
	};

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Supported Texture Names")
	TArray<FString> RoughnessArray = {
		TEXT("_Roughness"),
		TEXT("_RoughnessMap"),
		TEXT("_rough")
	};
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Supported Texture Names")
	TArray<FString> NormalArray = {
		TEXT("_Normal"),
		TEXT("_NormalMap"),
		TEXT("_nor")
	};
	              
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Supported Texture Names")
	TArray<FString> AmbientOcclusionArray = {
		TEXT("_AmbientOcclusion"),
		TEXT("_AmbientOcclusionMap"),
		TEXT("_AO")
	};

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Supported Texture Names")
	TArray<FString> ORMArray = {
		TEXT("_arm"),
		TEXT("_OcclusionRoughnessMetallic"),
		TEXT("_ORM")
	};


private:
	bool ProcessSelectedData(const TArray<FAssetData>& SelectedDataToProcess, TArray<UTexture2D*>& OutSelectedTextureArray, FString& OutSelectedTexturePackagePath);
	bool IsMaterialNameUsed(const FString& MaterialFolderPath, const FString& MaterialNameToCheck);
	UMaterial* CreateMaterialAsset(const FString& NameOfTheMaterial, const FString& PathToPut);
	UMaterialInstance* CreateMaterialInstance(UMaterial* CreatedMaterial, const FString& NameOfTheMaterialInstance, const FString& PathToPut);

	void Default_CreateMaterialNodes(UMaterial* CreatedMaterial, UTexture2D* SelectedTexture, uint32& PinsConnCount);
	void ORM_CreateMaterialNodes(UMaterial* CreatedMaterial, UTexture2D* SelectedTexture, uint32& PinsConnCount);
	
	bool TryConnectBaseColor(UMaterialExpressionTextureSample* TextureSampleNode, UTexture2D* SelectedTexture, UMaterial* CreatedMaterial);
	bool TryConnectByParameter(UMaterialExpressionTextureSample* TextureSampleNode, UTexture2D* SelectedTexture, UMaterial* CreatedMaterial,TArray<FString> ValidNamesContainer, EMaterialProperty MaterialProperty, int32 XPositionShift = 0, int32 YPositionShift = 0);
	bool TryConnectORM(UMaterialExpressionTextureSample* TextureSampleNode, UTexture2D* SelectedTexture, UMaterial* CreatedMaterial);
};
