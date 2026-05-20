// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AssetActionUtility.h"
#include "AssetActionExtender.generated.h"

USTRUCT(Blueprintable)
struct FAdditionalAssetClassesDetails
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Default")
	FString ClassName = FString();
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Default")
	FString ClassPrefix = FString();

	static const FAdditionalAssetClassesDetails* Find(const TArray<FAdditionalAssetClassesDetails>& From, const FString& Find)
	{
		if (Find.IsEmpty())
		{ 
			return nullptr;
		}
		for (const FAdditionalAssetClassesDetails& StructInstance : From)
		{
			if (StructInstance.ClassName == Find || StructInstance.ClassPrefix == Find)
			{
				return &StructInstance;
			}
		}
		return nullptr;
	}
};

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



protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (TitleProperty = "ClassName"), Category="Default")
	TArray<FAdditionalAssetClassesDetails> AssetClassPrefixes = {
		{ "Material", "M_" },
		{ "MaterialInstance", "MI_" },
		{ "MaterialInstanceConstant", "MI_" },
		{ "MaterialFunction", "MF_" },
		{ "Texture", "T_" },
		{ "Texture2D", "T_" },
		{ "TextureCube", "T_" },
		{ "StaticMesh", "SM_" },
		{ "SkeletalMesh", "SK_" },
		{ "AnimSequence", "AS_" },
		{ "Montages", "AM_" },
		{ "AnimationBlueprint", "ABP_" },
		{ "Blueprint", "BP_" },
		{ "BlueprintInterface", "BI_" },
		{ "WidgetBlueprint", "WBP_" },
		{ "ActorComponent", "AC_" },
		{ "Level", "LVL_" },
		{ "World", "WRLD_" },
		{ "SoundWave", "SW_" },
		{ "SoundCue", "SCue_" },
		{ "ParticleSystem", "PS_" },
		{ "NiagaraSystem", "FXS_" },
		{ "NiagaraEmitter", "FXE_" },
		{ "NiagaraFunction", "FXF_" },
		{ "PhysicsAsset", "PHYS_" },
		{ "PhysicsMaterial", "PM_" },
		{ "PostProcessMaterial", "PPM_" },
		{ "Skeleton", "SKEL_" },
		{ "BlendSpace", "BS_" },
		{ "DataTable", "DT_" },
		{ "CurveTable", "CT_" },
		{ "Enum", "E_" },
		{ "Structure", "F_" },
		{ "Rig", "Rig_" },
		{ "LevelSequence", "LS_" },
		{ "SequencerEdits", "EDIT_" },
		{ "MediaSource", "MS_" },
		{ "MediaOutput", "MO_" },
		{ "MediaPlayer", "MP_" },
		{ "MediaProfile", "MPR_" },
		{ "LevelSnapshots", "SNAP_" },
		{ "RemoteControlPreset", "RCP_" },
		{ "NDisplayConfiguration", "NDC_" },
		{ "OCIOProfile", "OCIO_" },
		{ "HDRI", "HDR_" },
		{ "Blueprint", "GE_" },
		{ "GameplayAbility", "GA_" },
		{ "GameplayCue", "GC_" },
		{ "GameplayCueNotify", "GCN_" },
		{ "GameplayCueNotify_Static", "GCNS_" },
		{ "GameplayCueNotify_Looping", "GCNL_" },
		{ "AttributeSet", "AS_" },
		{ "AbilityTask", "AT_" },
		{ "AbilitySystemComponent", "ASC_" },
		{ "GameplayTag", "Tag_" },
		{ "GameplayTagContainer", "Tags_" }
	};

	TArray<FString> AssetMess = {
		"_Inst"
	};
};

