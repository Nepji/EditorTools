// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AssetActionUtility.h"
#include "AssetActionExtender.generated.h"

/**
 *
 */
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



private:
	TMap<FString, FString> AssetPrefixMap = {
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
		{ "Blueprint", "BP_" },
		{ "SoundWave", "SW_" },
		{ "SoundCue", "SCue_" },
		{ "Level", "LVL_" },
		{ "World", "WRLD_" },
		{ "ParticleSystem", "PS_" },
		{ "NiagaraSystem", "FXS_" },
		{ "NiagaraEmitter", "FXE_" },
		{ "NiagaraFunction", "FXF_" },
		{ "PhysicsAsset", "PHYS_" },
		{ "PhysicsMaterial", "PM_" },
		{ "PostProcessMaterial", "PPM_" },
		{ "Skeleton", "SKEL_" },
		{ "Montages", "AM_" },
		{ "BlendSpace", "BS_" },
		{ "AnimationBlueprint", "ABP_" },
		{ "WidgetBlueprint", "WBP_" },
		{ "DataTable", "DT_" },
		{ "CurveTable", "CT_" },
		{ "Enum", "E_" },
		{ "Structure", "F_" },
		{ "Rig", "Rig_" },
		{ "ActorComponent", "AC_" },
		{ "BlueprintInterface", "BI_" },
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
		{ "HDRI", "HDR_" }
	};

	TArray<FString> AssetMess = {
		"_Inst"
	};
};

