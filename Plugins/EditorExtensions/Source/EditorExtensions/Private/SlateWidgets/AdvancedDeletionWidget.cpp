// Fill out your copyright notice in the Description page of Project Settings.

#include "SlateWidgets/AdvancedDeletionWidget.h"

#include "EditorExtensions.h"
#include "AssetActions/AssetActionExtender.h"

#define ListALL TEXT("All Assets")
#define ListUnused TEXT("Unused Assets")
#define ListDuplicated TEXT("Duplicated Assets")

void SAdvanceDeletionsTab::Construct(const FArguments& InArgs)
{
	bCanSupportFocus = true;

	StoredAssetsData = InArgs._AssetsDataToStore;
	DisplayedAssetsData = StoredAssetsData;

	FSlateFontInfo TitleTextFont = GetEmbossedTextFont();
	TitleTextFont.Size = 30;

	ComboSourceItems.Add(MakeShared<FString>(ListALL));
	ComboSourceItems.Add(MakeShared<FString>(ListUnused));

	ChildSlot
		[
			// Main VBOx
			SNew(SVerticalBox)

			// Header
			+ SVerticalBox::Slot()
				  .AutoHeight()
					  [ SNew(STextBlock)
							.Text(FText::FromString(TEXT("Advanced Deletion")))
							.Font(TitleTextFont)
							.Justification(ETextJustify::Center)
							.ColorAndOpacity(FColor::Cyan) ]

			// Specified listing condition
			+ SVerticalBox::Slot()
				  .AutoHeight()
					  [ SNew(SHorizontalBox)

						+ SHorizontalBox::Slot()
							  .AutoWidth()
								  [ ConstructGlobalCheckBox() ]

						+ SHorizontalBox::Slot()
							  .AutoWidth()
								  [ ConstructComboBox() ] ]

			// AssetList
			+ SVerticalBox::Slot()
				  .VAlign(VAlign_Fill)
					  [ SNew(SScrollBox)

						+ SScrollBox::Slot()
							[ ConstructAssetListView() ] ]

			// Buttons
			+ SVerticalBox::Slot()
				  .AutoHeight()
					  [ SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
							[ ConstructDeleteSelectedButton() ] ]

	];
}
FSlateFontInfo SAdvanceDeletionsTab::GetEmbossedTextFont() const
{
	return FCoreStyle::Get().GetFontStyle(FName("EmbossedText"));
}
void SAdvanceDeletionsTab::RefreshAssetListView()
{
	UAssetActionExtender::FixUpRedirectors();

	SelectedAssetData.Empty();
	CheckBoxesArray.Empty();
	if (!ConstructedAssetListView.IsValid())
	{
		return;
	}
	ConstructedAssetListView->RebuildList();
}
TSharedRef<SListView<TSharedPtr<FAssetData>>> SAdvanceDeletionsTab::ConstructAssetListView()
{
	ConstructedAssetListView = SNew(SListView<TSharedPtr<FAssetData>>)
								   .ItemHeight(24.f)
								   .ListItemsSource(&DisplayedAssetsData)
								   .OnGenerateRow(this, &SAdvanceDeletionsTab::OnGenerateRowForList);

	return ConstructedAssetListView.ToSharedRef();
}
TSharedRef<SComboBox<TSharedPtr<FString>>> SAdvanceDeletionsTab::ConstructComboBox()
{
	return SNew(SComboBox<TSharedPtr<FString>>)
		.OptionsSource(&ComboSourceItems)
		.OnGenerateWidget(this, &SAdvanceDeletionsTab::OnGenerateComboContent)
		.OnSelectionChanged(this, &SAdvanceDeletionsTab::OnComboSelectionChanged)
			[ SAssignNew(ComboDisplayBox, STextBlock)
				  .Text(FText::FromString(TEXT("List Assets Option"))) ];
}
TSharedRef<ITableRow> SAdvanceDeletionsTab::OnGenerateRowForList(TSharedPtr<FAssetData> AssetDataToDisplay, const TSharedRef<STableViewBase>& OwnerTable)
{
	if (!AssetDataToDisplay.IsValid())
	{
		return SNew(STableRow<TSharedPtr<FAssetData>>, OwnerTable);
	}
	const FString DisplayAssetClassName = AssetDataToDisplay->AssetClassPath.GetAssetName().ToString();
	const FString DisplayAssetPath = AssetDataToDisplay->PackagePath.ToString();
	const FString DisplayAssetName = AssetDataToDisplay->AssetName.ToString();

	FSlateFontInfo AssetClassNameFort = GetEmbossedTextFont();
	AssetClassNameFort.Size = 7;

	TSharedRef<STableRow<TSharedPtr<FAssetData>>> ListViewRowWidget =
		SNew(STableRow<TSharedPtr<FAssetData>>, OwnerTable)
			.Padding(FMargin(5.f))
				[ SNew(SHorizontalBox)
				  // CheckBoxSlot
				  + SHorizontalBox::Slot()
						.HAlign(HAlign_Left)
						.VAlign(VAlign_Center)
						.FillWidth(.02f)
							[ ConstructCheckBox(AssetDataToDisplay) ]
				  // AssetClassName
				  + SHorizontalBox::Slot()
						.HAlign(HAlign_Center)
						.VAlign(VAlign_Fill)
						.FillWidth(.3f)
							[ ConstructTextBLock(DisplayAssetClassName, AssetClassNameFort) ]
				  // AssetName
				  + SHorizontalBox::Slot()
					  [ SNew(STextBlock)
							.Text(FText::FromString(DisplayAssetName)) ]
				  // Asset Path
				  + SHorizontalBox::Slot()
						.HAlign(HAlign_Center)
						.VAlign(VAlign_Fill)
							[ ConstructTextBLock(DisplayAssetPath, AssetClassNameFort) ]
				  // ButtonToDeleteDirectly
				  + SHorizontalBox::Slot()
						.HAlign(HAlign_Center)
						.VAlign(VAlign_Fill)
							[ ConstructDeleteButton(AssetDataToDisplay) ] ];

	return ListViewRowWidget;
}
TSharedRef<SCheckBox> SAdvanceDeletionsTab::ConstructCheckBox(const TSharedPtr<FAssetData>& AssetDataToDisplay)
{
	TSharedRef<SCheckBox> ConstructedCheckBox = SNew(SCheckBox)
													.Type(ESlateCheckBoxType::CheckBox)
													.OnCheckStateChanged(this, &SAdvanceDeletionsTab::OnCheckBoxStateChange, AssetDataToDisplay)
													.Visibility(EVisibility::Visible);
	CheckBoxesArray.Add(ConstructedCheckBox);
	return ConstructedCheckBox;
}
TSharedRef<SCheckBox> SAdvanceDeletionsTab::ConstructGlobalCheckBox()
{
	return SNew(SCheckBox)
		.Type(ESlateCheckBoxType::CheckBox)
		.OnCheckStateChanged(this, &SAdvanceDeletionsTab::OnGlobalCheckBoxStateChange)
		.Visibility(EVisibility::Visible);
}
TSharedRef<STextBlock> SAdvanceDeletionsTab::ConstructTextBLock(const FString& TextContent, const FSlateFontInfo& FontToUse, const FColor& ColorToUse)
{
	return SNew(STextBlock)
		.Text(FText::FromString(TextContent))
		.Font(FontToUse)
		.ColorAndOpacity(ColorToUse);
}
TSharedRef<SButton> SAdvanceDeletionsTab::ConstructDeleteButton(const TSharedPtr<FAssetData>& AssetDataToDisplay)
{
	return SNew(SButton)
		.Text(FText::FromString("Delete"))
		.HAlign(HAlign_Right)
		.OnClicked(this, &SAdvanceDeletionsTab::OnDeleteButtonClicked, AssetDataToDisplay);
}
TSharedRef<SButton> SAdvanceDeletionsTab::ConstructDeleteSelectedButton()
{
	return SNew(SButton)
		.Text(FText::FromString("Delete Selected"))
		.OnClicked(this, &SAdvanceDeletionsTab::OnDeleteSelectedButtonClicked);
}
TSharedRef<SWidget> SAdvanceDeletionsTab::OnGenerateComboContent(TSharedPtr<FString> SourceItem)
{
	return SNew(STextBlock).Text(FText::FromString(*SourceItem.Get()));
}
void SAdvanceDeletionsTab::OnComboSelectionChanged(TSharedPtr<FString> SelectedOption, ESelectInfo::Type InSelectInfo)
{
	ComboDisplayBox->SetText(FText::FromString(*SelectedOption.Get()));

	FEditorExtensionsModule& EditorExtensionsModule = FModuleManager::LoadModuleChecked<FEditorExtensionsModule>(TEXT("EditorExtensions"));

	// Pass data to module filter
	if (*SelectedOption.Get() == ListALL)
	{
		DisplayedAssetsData = StoredAssetsData;
		RefreshAssetListView();
	}
	else if (*SelectedOption.Get() == ListUnused)
	{
		EditorExtensionsModule.GetFilteredAssetData(StoredAssetsData, DisplayedAssetsData);
		RefreshAssetListView();
	}
}
void SAdvanceDeletionsTab::OnCheckBoxStateChange(ECheckBoxState NewState, TSharedPtr<FAssetData> AssetData)
{
	switch (NewState)
	{
		case ECheckBoxState::Unchecked:
			if (SelectedAssetData.Contains(AssetData))
			{
				SelectedAssetData.Remove(AssetData);
			}
			break;
		case ECheckBoxState::Checked:
			SelectedAssetData.AddUnique(AssetData);
			break;
		case ECheckBoxState::Undetermined:
			break;
		default:
			break;
	}
}
void SAdvanceDeletionsTab::OnGlobalCheckBoxStateChange(ECheckBoxState NewState)
{
	if (CheckBoxesArray.Num() == 0)
	{
		return;
	}

	switch (NewState)
	{
		case ECheckBoxState::Unchecked:
			for (const TSharedRef<SCheckBox>& CheckBox : CheckBoxesArray)
			{
				if (CheckBox->IsChecked())
				{
					CheckBox->ToggleCheckedState();
				}
			}
			break;
		case ECheckBoxState::Checked:
			for (const TSharedRef<SCheckBox>& CheckBox : CheckBoxesArray)
			{
				if (!CheckBox->IsChecked())
				{
					CheckBox->ToggleCheckedState();
				}
			}
			break;
		case ECheckBoxState::Undetermined:
			break;
		default:
			break;
	}
}
FReply SAdvanceDeletionsTab::OnDeleteButtonClicked(TSharedPtr<FAssetData> AssetData)
{
	FEditorExtensionsModule& EditorExtensionsModule = FModuleManager::LoadModuleChecked<FEditorExtensionsModule>(TEXT("EditorExtensions"));

	// Refresh List
	if (EditorExtensionsModule.DeleteAsset(*AssetData.Get()))
	{
		// Update List Store Data
		if (StoredAssetsData.Contains(AssetData))
		{
			StoredAssetsData.Remove(AssetData);
		}
		if (DisplayedAssetsData.Contains(AssetData))
		{
			DisplayedAssetsData.Remove(AssetData);
		}
		RefreshAssetListView();
	}
	return FReply::Handled();
}
FReply SAdvanceDeletionsTab::OnDeleteSelectedButtonClicked()
{
	if (SelectedAssetData.IsEmpty())
	{
		return FReply::Handled();
	}

	TArray<FAssetData> AssetsDataToDelete;
	for (const TSharedPtr<FAssetData>& Data : SelectedAssetData)
	{
		AssetsDataToDelete.Add(*Data.Get());
	}

	FEditorExtensionsModule& EditorExtensionsModule = FModuleManager::LoadModuleChecked<FEditorExtensionsModule>(TEXT("EditorExtensions"));
	if (EditorExtensionsModule.DeleteAssets(AssetsDataToDelete))
	{
		for (const TSharedPtr<FAssetData>& Data : SelectedAssetData)
		{
			if (StoredAssetsData.Contains(Data))
			{
				StoredAssetsData.Remove(Data);
			}
			if (DisplayedAssetsData.Contains(Data))
			{
				DisplayedAssetsData.Remove(Data);
			}
		}
		RefreshAssetListView();
	}

	return FReply::Handled();
}
