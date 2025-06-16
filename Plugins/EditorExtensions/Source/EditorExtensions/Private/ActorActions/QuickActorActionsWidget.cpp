#include "ActorActions/QuickActorActionsWidget.h"

#include "EditorExtensions/DebugUtils.h"
#include "Subsystems/EditorActorSubsystem.h"

void UQuickActorActionsWidget::SelectAllActorWithSimilarName()
{
	if (!GetEditorActorSubsystem())
	{
		return;
	}

	TArray<AActor*> SelectedActors = EditorActorSubsystem->GetSelectedLevelActors();
	uint32 SelectionCounter = 0;

	if (!FindSpecificActorName)
	{
		if (SelectedActors.IsEmpty())
		{
			DebugHelper::ShowNotifyInfo(TEXT("No actor selected."));
			return;
		}

		if (SelectedActors.Num() > 1)
		{
			DebugHelper::ShowNotifyInfo(TEXT("Only one actor must be selected."));
			return;
		}
	}
	else if (ActorNameToFind.IsEmpty())
	{
		DebugHelper::ShowNotifyInfo(TEXT("Not enought data specified."));
		return;
	}
	
	const FString NameToSearch = FindSpecificActorName ? ActorNameToFind : SelectedActors.Last()->GetActorLabel().LeftChop(2);

	TArray<AActor*> AllLevelActors = EditorActorSubsystem->GetAllLevelActors();

	for (AActor* ActorInLevel : AllLevelActors)
	{
		if (!ActorInLevel)
		{
			continue;
		}

		if (ActorInLevel->GetActorLabel().Contains(NameToSearch, SearchCase))
		{
			EditorActorSubsystem->SetActorSelectionState(ActorInLevel, true);
			SelectionCounter++;
		}
	}

	if (SelectionCounter > 0)
	{
		DebugHelper::ShowNotifyInfo(TEXT("Successfully selected: ") + FString::FromInt(SelectionCounter) + TEXT(" actors."));
	}
	else
	{
		DebugHelper::ShowNotifyInfo(TEXT("No actor with similar name was finded"));
	}
}
void UQuickActorActionsWidget::DuplicateActors()
{
	if (!GetEditorActorSubsystem())
	{
		return;
	}

	TArray<AActor*> SelectedActors = EditorActorSubsystem->GetSelectedLevelActors();
	uint32 SelectionCounter = 0;

	if (SelectedActors.IsEmpty())
	{
		DebugHelper::ShowNotifyInfo(TEXT("No actor selected."));
		return;
	}

	if (NumberOfDuplicates <= 0 || OffsetDistance == 0)
	{
		DebugHelper::ShowNotifyInfo(TEXT("Not enought data specified."));
		return;
	}

	for (AActor* SelectedActor : SelectedActors)
	{
		if (!SelectedActor)
		{
			continue;
		}

		for (int32 i = 0; i < NumberOfDuplicates; i++)
		{
			AActor* DuplicatedActor = EditorActorSubsystem->DuplicateActor(SelectedActor, SelectedActor->GetWorld());
			if (!DuplicatedActor)
			{
				continue;
			}

			const float DuplicationOffsetDist = (i + 1) * OffsetDistance;

			if (AxisForDuplication == E_QuickActionAxis::EDA_X)
			{
				DuplicatedActor->AddActorWorldOffset(FVector(DuplicationOffsetDist, 0.f, 0.f));
			}
			else if (AxisForDuplication == E_QuickActionAxis::EDA_Y)
			{
				DuplicatedActor->AddActorWorldOffset(FVector(0.f, DuplicationOffsetDist, 0.f));
			}
			else if (AxisForDuplication == E_QuickActionAxis::EDA_Z)
			{
				DuplicatedActor->AddActorWorldOffset(FVector(0.f, 0.f, DuplicationOffsetDist));
			}
			else if (AxisForDuplication == E_QuickActionAxis::EDA_MAX)
			{
				DuplicatedActor->AddActorWorldOffset(FVector(0.f, 0.f, 0.f));
			}
			else
			{
				DebugHelper::ShowNotifyInfo(TEXT("Unsuported axis specifier."));
				return;
			}

			EditorActorSubsystem->SetActorSelectionState(DuplicatedActor, true);
			SelectionCounter++;
		}
	}

	if (SelectionCounter > 0)
	{
		DebugHelper::ShowNotifyInfo(TEXT("Successfully duplicated: ") + FString::FromInt(SelectionCounter) + TEXT(" actors."));
	}
	else
	{
		DebugHelper::ShowNotifyInfo(TEXT("No actor was duplicated"));
	}
}
void UQuickActorActionsWidget::RandomTransformActors()
{
	if (!GetEditorActorSubsystem())
	{
		return;
	}

	TArray<AActor*> SelectedActors = EditorActorSubsystem->GetSelectedLevelActors();
	uint32 SelectionCounter = 0;

	if (SelectedActors.IsEmpty())
	{
		DebugHelper::ShowNotifyInfo(TEXT("No actor selected."));
		return;
	}

	if (RandomActorRotation.MaxRotationDegree == 0)
	{
		DebugHelper::ShowNotifyInfo(TEXT("Not enought data specified."));
		return;
	}

	for (AActor* SelectedActor : SelectedActors)
	{
		if (!SelectedActor)
		{
			continue;
		}
		if (!RandomRotateSpecificActor(*SelectedActor)
			&& !RandomScaleSpecificActor(*SelectedActor)
			&& !RandomOffsetSpecificActor(*SelectedActor))
		{
			continue;
		}

		SelectionCounter++;
	}

	if (SelectionCounter > 0)
	{
		DebugHelper::ShowNotifyInfo(TEXT("Successfully transform: ") + FString::FromInt(SelectionCounter) + TEXT(" actors."));
	}
	else
	{
		DebugHelper::ShowNotifyInfo(TEXT("No actor was rotated"));
	}
}
bool UQuickActorActionsWidget::GetEditorActorSubsystem()
{
	if (!EditorActorSubsystem)
	{
		EditorActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>();
	}

	return EditorActorSubsystem != nullptr;
}
bool UQuickActorActionsWidget::RandomRotateSpecificActor(AActor& SpecificActor)
{
	if (!GetEditorActorSubsystem() || !EnableRandomRotation)
	{
		return false;
	}

	const int32 MinRotationDegree = RandomActorRotation.NegativeMirroredRotationDegree ? -RandomActorRotation.MaxRotationDegree : 0;
	const int32 RandomDegree = FMath::RandRange(MinRotationDegree, RandomActorRotation.MaxRotationDegree);

	if (RandomActorRotation.AxisForRotation == E_QuickActionAxis::EDA_X)
	{
		SpecificActor.AddActorWorldRotation(FRotator(RandomDegree, 0.f, 0.f));
	}
	else if (RandomActorRotation.AxisForRotation == E_QuickActionAxis::EDA_Y)
	{
		SpecificActor.AddActorWorldRotation(FRotator(0.f, RandomDegree, 0.f));
	}
	else if (RandomActorRotation.AxisForRotation == E_QuickActionAxis::EDA_Z)
	{
		SpecificActor.AddActorWorldRotation(FRotator(0.f, 0.f, RandomDegree));
	}
	else if (RandomActorRotation.AxisForRotation == E_QuickActionAxis::EDA_MAX)
	{
		SpecificActor.AddActorWorldRotation(FRotator(0.f, 0.f, 0.f));
	}
	else
	{
		DebugHelper::ShowNotifyInfo(TEXT("Unsuported axis specifier."));
		return false;
	}
	return true;
}
bool UQuickActorActionsWidget::RandomScaleSpecificActor(AActor& SpecificActor)
{
	if (!GetEditorActorSubsystem() || !EnableRandomScale)
	{
		return false;
	}
	const FVector ActorScale = FVector(FMath::RandRange(RandomActorScale.ScaleMin, RandomActorScale.ScaleMax));
	SpecificActor.SetActorScale3D(ActorScale);

	return true;
}
bool UQuickActorActionsWidget::RandomOffsetSpecificActor(AActor& SpecificActor)
{
	if (!GetEditorActorSubsystem() || !EnableRandomOffset)
	{
		return false;
	}

	const float RandomOffsetValue = FMath::RandRange(RandomActorOffset.OffsetMin, RandomActorOffset.OffsetMax);
	SpecificActor.AddActorWorldOffset(FVector(RandomOffsetValue, RandomOffsetValue, 0.f));

	return true;
}