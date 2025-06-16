// Fill out your copyright notice in the Description page of Project Settings.

#include "AssetActions/MaterialCreationWidget.h"

#include "AssetToolsModule.h"
#include "EditorAssetLibrary.h"
#include "EditorUtilityLibrary.h"
#include "EditorExtensions/DebugUtils.h"
#include "Factories/MaterialFactoryNew.h"
#include "Factories/MaterialFunctionInstanceFactory.h"
#include "Factories/MaterialInstanceConstantFactoryNew.h"
#include "Materials/MaterialExpressionTextureSample.h"
#include "Materials/MaterialInstanceConstant.h"

void UMaterialCreationWidget::CreateMaterialFromSelectedTexture()
{
	if (bCustomMaterialName)
	{
		if (MaterialName.IsEmpty())
		{
			DebugHelper::ShowMsgDialog(EAppMsgType::Ok, TEXT("Please enter valid material name."));
		}
		else if (!MaterialName.StartsWith("M_"))
		{
			DebugHelper::ShowMsgDialog(EAppMsgType::Ok, TEXT("Material prefix recommended to be `M_`"));
		}
	}

	TArray<FAssetData> SelectedAssetsData = UEditorUtilityLibrary::GetSelectedAssetData();
	TArray<UTexture2D*> SelectedTexturesArray;
	FString SelectedTextureFolderPath;

	if (!ProcessSelectedData(SelectedAssetsData, SelectedTexturesArray, SelectedTextureFolderPath))
	{
		return;
	}
	if (IsMaterialNameUsed(SelectedTextureFolderPath, MaterialName))
	{
		return;
	}

	UMaterial* CreatedMaterial = CreateMaterialAsset(MaterialName, SelectedTextureFolderPath);

	if (!CreatedMaterial)
	{
		DebugHelper::ShowMsgDialog(EAppMsgType::Ok, TEXT("Error while creating material."));
		return;
	}

	uint32 PinsConnectedCounter = 0;
	for (UTexture2D* SelectedTexture : SelectedTexturesArray)
	{
		if (!SelectedTexture)
		{
			continue;
		}

		if (ChannelPackingType == E_ChannelPackingType::ECPT_NoChannelPacking)
		{
			Default_CreateMaterialNodes(CreatedMaterial, SelectedTexture, PinsConnectedCounter);
		}
		else if (ChannelPackingType == E_ChannelPackingType::ECPT_ORM)
		{
			ORM_CreateMaterialNodes(CreatedMaterial, SelectedTexture, PinsConnectedCounter);
		}
		else if (ChannelPackingType == E_ChannelPackingType::ECPT_MAX)
		{
		}
	}

	if (bCreateMaterialInstanceOnFinish)
	{
		UMaterialInstance* CreatedMaterialInstance = CreateMaterialInstance(CreatedMaterial,MaterialInstanceName,SelectedTextureFolderPath);
	}

	if (PinsConnectedCounter > 0)
	{
		DebugHelper::ShowNotifyInfo(TEXT("Successfully connected ")
									+ FString::FromInt(PinsConnectedCounter) + (TEXT(" pins")));
	}
}
// Process the selected data, will filter out textures,and return false if non-texture selected
bool UMaterialCreationWidget::ProcessSelectedData(const TArray<FAssetData>& SelectedDataToProcess, TArray<UTexture2D*>& OutSelectedTextureArray, FString& OutSelectedTexturePackagePath)
{
	if (SelectedDataToProcess.IsEmpty())
	{
		DebugHelper::ShowMsgDialog(EAppMsgType::Ok, TEXT("No texture selected"));
		return false;
	}

	bool bMaterialNameSet = false;
	for (const FAssetData Data : SelectedDataToProcess)
	{
		UObject* SelectedAsset = Data.GetAsset();
		if (!SelectedAsset)
		{
			continue;
		}

		UTexture2D* SelectedTexture = Cast<UTexture2D>(SelectedAsset);

		if (!SelectedTexture)
		{
			DebugHelper::ShowMsgDialog(EAppMsgType::Ok, TEXT("Please select only textures.\nWarning awekened by: " + SelectedTexture->GetName()));
			continue;
		}

		if (OutSelectedTexturePackagePath.IsEmpty())
		{
			OutSelectedTexturePackagePath = Data.PackagePath.ToString();
		}

		if (!bCustomMaterialName && !bMaterialNameSet)
		{
			MaterialName = SelectedAsset->GetName();
			MaterialName.RemoveFromStart(TEXT("T_"));
			MaterialName = TEXT("M_") + MaterialName;

			bMaterialNameSet = true;
		}

		OutSelectedTextureArray.Add(SelectedTexture);
	}

	return true;
}
// Will return true if the material name is used by asset under the specified folder
bool UMaterialCreationWidget::IsMaterialNameUsed(const FString& MaterialFolderPath, const FString& MaterialNameToCheck)
{
	TArray<FString> ExistingAssetPaths = UEditorAssetLibrary::ListAssets(MaterialFolderPath, false);

	for (const FString& AssetPath : ExistingAssetPaths)
	{
		const FString ExistingAssetName = FPaths::GetBaseFilename(AssetPath);
		if (ExistingAssetName == MaterialNameToCheck)
		{
			DebugHelper::ShowMsgDialog(EAppMsgType::Ok, MaterialNameToCheck + TEXT(" is already used by asset"));

			return true;
		}
	}
	return false;
}
UMaterial* UMaterialCreationWidget::CreateMaterialAsset(const FString& NameOfTheMaterial, const FString& PathToPut)
{
	FAssetToolsModule& AssetModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));

	UMaterialFactoryNew* MaterialFactory = NewObject<UMaterialFactoryNew>();

	UObject* CreatedObject = AssetModule.Get().CreateAsset(NameOfTheMaterial,
														   PathToPut,
														   UMaterial::StaticClass(),
														   MaterialFactory);

	return Cast<UMaterial>(CreatedObject);
}

UMaterialInstance* UMaterialCreationWidget::CreateMaterialInstance(UMaterial* CreatedMaterial, const FString& NameOfTheMaterialInstance, const FString& PathToPut)
{
	FString MIName = NameOfTheMaterialInstance;
	if (!bCustomMaterialInstanceName)
	{
		MIName = CreatedMaterial->GetName();

		MIName.RemoveFromStart(TEXT("M_"));
		MIName.InsertAt(0, TEXT("MI_"));
	}

	UMaterialInstanceConstantFactoryNew* MIFactoryNew = NewObject<UMaterialInstanceConstantFactoryNew>();

	FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));

	UObject* CreatedObject = AssetToolsModule.Get().CreateAsset(MIName,
																PathToPut,
																UMaterialInstanceConstant::StaticClass(),
																MIFactoryNew);

	UMaterialInstanceConstant* CreatedMI = Cast<UMaterialInstanceConstant>(CreatedObject);
	if (!CreatedMI)
	{
		return nullptr;
	}

	
	CreatedMI->SetParentEditorOnly(CreatedMaterial);

	CreatedMI->PostEditChange();
	CreatedMaterial->PostEditChange();

	return CreatedMI;
}

void UMaterialCreationWidget::Default_CreateMaterialNodes(UMaterial* CreatedMaterial, UTexture2D* SelectedTexture, uint32& PinsConnCount)
{
	UMaterialExpressionTextureSample* TextureSampleNode = NewObject<UMaterialExpressionTextureSample>(CreatedMaterial);

	if (!TextureSampleNode)
	{
		return;
	}

	if (!CreatedMaterial->HasBaseColorConnected() && TryConnectBaseColor(TextureSampleNode, SelectedTexture, CreatedMaterial))
	{
		PinsConnCount++;
		return;
	}

	if (!CreatedMaterial->HasMetallicConnected() && TryConnectByParameter(TextureSampleNode, SelectedTexture, CreatedMaterial, MetallicArray, EMaterialProperty::MP_Metallic, -600, 240))
	{
		PinsConnCount++;
		return;
	}

	if (!CreatedMaterial->HasRoughnessConnected() && TryConnectByParameter(TextureSampleNode, SelectedTexture, CreatedMaterial, RoughnessArray, EMaterialProperty::MP_Roughness, -600, 480))
	{
		PinsConnCount++;
		return;
	}

	if (!CreatedMaterial->HasNormalConnected() && TryConnectByParameter(TextureSampleNode, SelectedTexture, CreatedMaterial, NormalArray, EMaterialProperty::MP_Normal, -600, 720))
	{
		PinsConnCount++;
		return;
	}

	if (!CreatedMaterial->HasAmbientOcclusionConnected() && TryConnectByParameter(TextureSampleNode, SelectedTexture, CreatedMaterial, AmbientOcclusionArray, EMaterialProperty::MP_AmbientOcclusion, -600, 960))
	{
		PinsConnCount++;
		return;
	}
}
void UMaterialCreationWidget::ORM_CreateMaterialNodes(UMaterial* CreatedMaterial, UTexture2D* SelectedTexture, uint32& PinsConnCount)
{
	UMaterialExpressionTextureSample* TextureSampleNode = NewObject<UMaterialExpressionTextureSample>(CreatedMaterial);

	if (!TextureSampleNode)
	{
		return;
	}

	if (!CreatedMaterial->HasBaseColorConnected() && TryConnectBaseColor(TextureSampleNode, SelectedTexture, CreatedMaterial))
	{
		PinsConnCount++;
		return;
	}

	if (!CreatedMaterial->HasNormalConnected() && TryConnectByParameter(TextureSampleNode, SelectedTexture, CreatedMaterial, NormalArray, EMaterialProperty::MP_Normal, -600, 720))
	{
		PinsConnCount++;
		return;
	}

	if (!CreatedMaterial->HasRoughnessConnected() && TryConnectORM(TextureSampleNode, SelectedTexture, CreatedMaterial))
	{
		PinsConnCount += 3;
		return;
	}
}
bool UMaterialCreationWidget::TryConnectBaseColor(UMaterialExpressionTextureSample* TextureSampleNode, UTexture2D* SelectedTexture, UMaterial* CreatedMaterial)
{
	for (const FString& BaseColorName : BaseColorArray)
	{
		if (SelectedTexture->GetName().Contains(BaseColorName))
		{
			TextureSampleNode->Texture = SelectedTexture;

			CreatedMaterial->GetExpressionCollection().AddExpression(TextureSampleNode);
			CreatedMaterial->GetExpressionInputForProperty(MP_BaseColor)->Expression = TextureSampleNode;
			CreatedMaterial->PostEditChange();

			TextureSampleNode->MaterialExpressionEditorX -= 600;

			return true;
		}
	}

	return false;
}
bool UMaterialCreationWidget::TryConnectByParameter(UMaterialExpressionTextureSample* TextureSampleNode,
													UTexture2D* SelectedTexture,
													UMaterial* CreatedMaterial,
													TArray<FString> ValidNamesContainer,
													EMaterialProperty MaterialProperty,
													int32 XPositionShift,
													int32 YPositionShift)
{
	for (const FString& ValidName : ValidNamesContainer)
	{
		if (SelectedTexture->GetName().Contains(ValidName))
		{
			SelectedTexture->CompressionSettings = TC_Default;
			SelectedTexture->SRGB = false;
			SelectedTexture->PostEditChange();

			TextureSampleNode->Texture = SelectedTexture;
			TextureSampleNode->SamplerType = SAMPLERTYPE_LinearColor;

			CreatedMaterial->GetExpressionCollection().AddExpression(TextureSampleNode);
			CreatedMaterial->GetExpressionInputForProperty(MaterialProperty)->Expression = TextureSampleNode;
			CreatedMaterial->PostEditChange();

			TextureSampleNode->MaterialExpressionEditorX += XPositionShift;
			TextureSampleNode->MaterialExpressionEditorY += YPositionShift;

			return true;
		}
	}

	return false;
}
bool UMaterialCreationWidget::TryConnectORM(UMaterialExpressionTextureSample* TextureSampleNode, UTexture2D* SelectedTexture, UMaterial* CreatedMaterial)
{
	for (const FString& ValidName : ORMArray)
	{
		if (SelectedTexture->GetName().Contains(ValidName))
		{
			SelectedTexture->CompressionSettings = TC_Masks;
			SelectedTexture->SRGB = false;
			SelectedTexture->PostEditChange();

			TextureSampleNode->Texture = SelectedTexture;
			TextureSampleNode->SamplerType = SAMPLERTYPE_Masks;

			CreatedMaterial->GetExpressionCollection().AddExpression(TextureSampleNode);
			CreatedMaterial->GetExpressionInputForProperty(MP_AmbientOcclusion)->Connect(1, TextureSampleNode);
			CreatedMaterial->GetExpressionInputForProperty(MP_Roughness)->Connect(2, TextureSampleNode);
			CreatedMaterial->GetExpressionInputForProperty(MP_Metallic)->Connect(3, TextureSampleNode);
			CreatedMaterial->PostEditChange();

			TextureSampleNode->MaterialExpressionEditorX += -600;
			TextureSampleNode->MaterialExpressionEditorY += 240;

			return true;
		}
	}

	return false;
}
