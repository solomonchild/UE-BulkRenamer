// Copyright Epic Games, Inc. All Rights Reserved.

#include "BulkRenamerCommands.h"

#define LOCTEXT_NAMESPACE "FBulkRenamerModule"

void FBulkRenamerCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "BulkRenamer", "Execute BulkRenamer action", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
