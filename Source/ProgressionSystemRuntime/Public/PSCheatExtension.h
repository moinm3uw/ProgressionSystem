// Copyright (c) Valerii Rotermel & Yevhenii Selivanov

#pragma once

#include "CoreMinimal.h"
#include "MetaCheatManagerExtension.h"
#include "PSCheatExtension.generated.h"

/**
 * Extends cheat manager with Progression System-related console commands.
 */
UCLASS()
class PROGRESSIONSYSTEMRUNTIME_API UPSCheatExtension : public UMetaCheatManagerExtension
{
	GENERATED_BODY()
public: 
	/** Removes a save file of the Progression System (reset progression)  */
	UFUNCTION(Exec, meta = (CheatName = "Bomber.Saves.Reset.ProgressionSystem"))
	static void ResetNewMainMenuSaves();
	
};
