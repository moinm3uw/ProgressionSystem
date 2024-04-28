// Copyright (c) Valerii Rotermel & Yevhenii Selivanov


#include "PSCheatExtension.h"

#include "Components/PSHUDComponent.h"
#include "Data/PSWorldSubsystem.h"

// Removes a save file of the Progression System (reset progression) 
void UPSCheatExtension::ResetNewMainMenuSaves()
{
	UPSWorldSubsystem::Get().ResetSaveGameData();
	UPSHUDComponent* PSHUDComponent = UPSWorldSubsystem::Get().GetProgressionSystemHUDComponent();
	PSHUDComponent->UpdateProgressionWidgetForPlayer();
}
