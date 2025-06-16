#pragma once
#include "Framework/Commands/Commands.h"

class FEditorExtensionsUICommands : public TCommands<FEditorExtensionsUICommands>
{
public:
	FEditorExtensionsUICommands()
		: TCommands<FEditorExtensionsUICommands>(TEXT("EditorExtensions"),
												 FText::FromString(TEXT("Editor Extensions UI Commands")),
												 NAME_None,
												 TEXT("EditorExtensions")) {}

	virtual void RegisterCommands() override;

	TSharedPtr<FUICommandInfo> LockActorSelection;
	TSharedPtr<FUICommandInfo> UnlockActorSelection;
};

