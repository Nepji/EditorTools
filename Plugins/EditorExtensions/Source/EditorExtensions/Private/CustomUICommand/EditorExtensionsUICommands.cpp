#include "CustomUICommand/EditorExtensionsUICommands.h"

#define LOCTEXT_NAMESPACE "FEditorExtensionsModule"

void FEditorExtensionsUICommands::RegisterCommands()
{
	UI_COMMAND(LockActorSelection,
			   "Lock Actor Selction",
			   "Lock actor selection in level, once triggered - actor can no longer be selected.",
			   EUserInterfaceActionType::Button,
			   FInputChord(EKeys::W, EModifierKey::Alt));

	UI_COMMAND(UnlockActorSelection,
		   "Unlock existed actor",
		   "Unlock actor in level, once triggered - actor can be selected.",
		   EUserInterfaceActionType::Button,
		   FInputChord(EKeys::W, EModifierKey::Alt | EModifierKey::Shift));
}

#undef LOCTEXT_NAMESPACE