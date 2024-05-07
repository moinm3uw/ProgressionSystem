// Copyright (c) Valerii Rotermel & Yevhenii Selivanov


#include "PSCheatExtension.h"

#include "Components/PSHUDComponent.h"
#include "Data/PSWorldSubsystem.h"

// Removes a save file of the Progression System (reset progression) 
void UPSCheatExtension::ResetProgressionSystemSaves()
{
	UPSWorldSubsystem::Get().ResetSaveGameData();
}

// Unlocks all levels of the Progression System (reset progression)
void UPSCheatExtension::UnlockAllLevels()
{
	UPSWorldSubsystem::Get().UnlockAllLevels();
}
