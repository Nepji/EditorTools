// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "QuickActorActionsWidget.generated.h"

UENUM(BlueprintType)
enum class E_QuickActionAxis : uint8
{
	EDA_X UMETA(DisplayName = "X Axis"),
	EDA_Y UMETA(DisplayName = "Y Axis"),
	EDA_Z UMETA(DisplayName = "Z Axis"),
	EDA_MAX UMETA(DisplayName = "Default Max"),
};

USTRUCT(BlueprintType)
struct FRandomActorRotation
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	E_QuickActionAxis AxisForRotation = E_QuickActionAxis::EDA_X;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool NegativeMirroredRotationDegree = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMax = 360, ClampMin = -360))
	int32 MaxRotationDegree = 180;
};

USTRUCT(BlueprintType)
struct FRandomActorScale
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float ScaleMin = .8f;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float ScaleMax = 1.2f;
};

USTRUCT(BlueprintType)
struct FRandomActorOffset
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float OffsetMin  = -50.f;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float OffsetMax  = 50.f;
};

UCLASS()
class EDITOREXTENSIONS_API UQuickActorActionsWidget : public UEditorUtilityWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SelectAllActorWithSimilarName();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ActorBatchSelection")
	TEnumAsByte<ESearchCase::Type> SearchCase = ESearchCase::IgnoreCase;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ActorBatchSelection")
	bool FindSpecificActorName = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ActorBatchSelection", meta = (EditCondition = "FindSpecificActorName"))
	FString ActorNameToFind = FString();
	
	UFUNCTION(BlueprintCallable)
	void DuplicateActors();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ActorBatchDuplication")
	E_QuickActionAxis AxisForDuplication = E_QuickActionAxis::EDA_X;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ActorBatchDuplication")
	int32 NumberOfDuplicates = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ActorBatchDuplication")
	float OffsetDistance = 300.f;


	UFUNCTION(BlueprintCallable)
	void RandomTransformActors();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ActorBatchRandomTransform")
	bool EnableRandomRotation = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ActorBatchRandomTransform", meta = (EditCondition = "EnableRandomRotation"))
	FRandomActorRotation RandomActorRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ActorBatchRandomTransform")
	bool EnableRandomScale = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ActorBatchRandomTransform", meta = (EditCondition = "EnableRandomScale"))
	FRandomActorScale RandomActorScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ActorBatchRandomTransform")
	bool EnableRandomOffset = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ActorBatchRandomTransform", meta = (EditCondition = "EnableRandomOffset"))
	FRandomActorOffset RandomActorOffset;

private:
	UPROPERTY()
	class UEditorActorSubsystem* EditorActorSubsystem;

	bool GetEditorActorSubsystem();
	bool RandomRotateSpecificActor(AActor& SpecificActor);
	bool RandomScaleSpecificActor(AActor& SpecificActor);
	bool RandomOffsetSpecificActor(AActor& SpecificActor);
};
