#include "CustomOutlinerColumn/OutlinerSelectionColumn.h"

#include "ActorTreeItem.h"
#include "EditorExtensions.h"
#include "ISceneOutlinerTreeItem.h"
#include "CustomStyle/EditorExtensionStyle.h"
SHeaderRow::FColumn::FArguments FOutlinerSelectionColumn::ConstructHeaderRowColumn()
{
	return SHeaderRow::Column(GetColumnID())
		.FixedWidth(24.f)
		.HAlignHeader(HAlign_Center)
		.VAlignHeader(VAlign_Center)
		.HAlignCell(HAlign_Center)
		.VAlignCell(VAlign_Center)
		.DefaultTooltip(FText::FromString("Actor Selection Lock - Press icon to lock actor selection"))
			[ SNew(SImage)
				  .ColorAndOpacity(FSlateColor::UseForeground())
				  .Image(FEditorExtensionStyle::GetCreatedSlateStyleSet()->GetBrush(FName("ContentBrowser.Lock"))) ];
}
const TSharedRef<SWidget> FOutlinerSelectionColumn::ConstructRowWidget(FSceneOutlinerTreeItemRef TreeItem, const STableRow<FSceneOutlinerTreeItemPtr>& Row)
{
	FActorTreeItem* ActorTreeItem = TreeItem->CastTo<FActorTreeItem>();
	if (!ActorTreeItem || !ActorTreeItem->IsValid())
	{
		return SNullWidget::NullWidget;
	}

	FEditorExtensionsModule& EditorExtensionsModule = FModuleManager::LoadModuleChecked<FEditorExtensionsModule>(TEXT("EditorExtensions"));

	const bool ActorSelectionLocked = EditorExtensionsModule.CheckIsActorSelectionLocked(ActorTreeItem->Actor.Get());

	const FCheckBoxStyle& ToggleButtonStyle = FEditorExtensionStyle::GetCreatedSlateStyleSet()->GetWidgetStyle<FCheckBoxStyle>(FName("SceneOutliner.SelectionLock"));

	return SNew(SCheckBox)
		.HAlign(HAlign_Center)
		.IsChecked(ActorSelectionLocked ? ECheckBoxState::Checked : ECheckBoxState::Unchecked)
		.OnCheckStateChanged(this, &FOutlinerSelectionColumn::OnRowWidgetCheckStateChanged, ActorTreeItem->Actor)
		.Visibility(EVisibility::Visible)
		.Type(ESlateCheckBoxType::ToggleButton)
		.Style(&ToggleButtonStyle);
}
void FOutlinerSelectionColumn::OnRowWidgetCheckStateChanged(ECheckBoxState NewState, TWeakObjectPtr<AActor> CorrespondingActor)
{
	FEditorExtensionsModule& EditorExtensionsModule = FModuleManager::LoadModuleChecked<FEditorExtensionsModule>(TEXT("EditorExtensions"));

	if (NewState == ECheckBoxState::Unchecked)
	{
		EditorExtensionsModule.ProcessLockingForOutliner(CorrespondingActor.Get(), false);
	}
	else if (NewState == ECheckBoxState::Checked)
	{
		EditorExtensionsModule.ProcessLockingForOutliner(CorrespondingActor.Get(), true);
	}
}