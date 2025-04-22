// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class SAdvanceDeletionsTab : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SAdvanceDeletionsTab) {}

	SLATE_ARGUMENT(TArray<TSharedPtr<FAssetData>>, AssetsDataToStore);

	SLATE_END_ARGS();

public:
	void Construct(const FArguments& InArgs);

private:
	FSlateFontInfo GetEmbossedTextFont() const;
	void RefreshAssetListView();

	TSharedRef<SListView<TSharedPtr<FAssetData>>> ConstructAssetListView();
	TSharedRef<SComboBox<TSharedPtr<FString>>> ConstructComboBox();
	TSharedRef<ITableRow> OnGenerateRowForList(TSharedPtr<FAssetData> AssetDataToDisplay, const TSharedRef<STableViewBase>& OwnerTable);
	TSharedRef<SCheckBox> ConstructCheckBox(const TSharedPtr<FAssetData>& AssetDataToDisplay);
	TSharedRef<SCheckBox> ConstructGlobalCheckBox();
	TSharedRef<STextBlock> ConstructTextBLock(const FString& TextContent, const FSlateFontInfo& FontToUse, const FColor& ColorToUse = FColor::White);
	TSharedRef<SButton> ConstructDeleteButton(const TSharedPtr<FAssetData>& AssetDataToDisplay);
	TSharedRef<SButton> ConstructDeleteSelectedButton();

	TSharedRef<SWidget> OnGenerateComboContent(TSharedPtr<FString> SourceItem);
	void OnComboSelectionChanged(TSharedPtr<FString> SelectedOption, ESelectInfo::Type InSelectInfo);
	void OnRowWidgetMouseButtonClicked(TSharedPtr<FAssetData> ClickedData);

	void OnCheckBoxStateChange(ECheckBoxState NewState, TSharedPtr<FAssetData> AssetData);
	void OnGlobalCheckBoxStateChange(ECheckBoxState NewState);
	FReply OnDeleteButtonClicked(TSharedPtr<FAssetData> AssetData);
	FReply OnDeleteSelectedButtonClicked();



private:
	TArray<TSharedPtr<FAssetData>> StoredAssetsData;
	TArray<TSharedPtr<FAssetData>> DisplayedAssetsData;
	
	TSharedPtr<SListView<TSharedPtr<FAssetData>>> ConstructedAssetListView;
	TArray<TSharedPtr<FAssetData>> SelectedAssetData;

	TArray< TSharedRef <SCheckBox> > CheckBoxesArray;
	
	TArray<TSharedPtr<FString>> ComboSourceItems;

	TSharedPtr<STextBlock> ComboDisplayBox;
};