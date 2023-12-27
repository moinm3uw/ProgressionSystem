// Copyright (c) Valerii Rotermel & Yevhenii Selivanov

#pragma once

#include "Bomber.h"
#include "Structures/PlayerTag.h"
#include "ModuleStructures.generated.h"

/**
 * Basic structure for all save data information regarding the progression.
 * Same structure will reflected in the save file. Initial load performed based on the data in the DT Table 
 */
USTRUCT(BlueprintType)
struct FProgressionRowData : public FTableRowBase
{
	GENERATED_BODY()

	/** Stores the value of the map for progression system component */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="C++")
	ELevelType Map = ELevelType::None;

	/** Contains the character player tag used in the save/load progression system */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="C++")
	FPlayerTag Character = FPlayerTag::None;

	/** Icon for each level. Currently not in use*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="C++")
	TObjectPtr<UTexture> Icon = nullptr;

	/** Stores the required color for level. Not used at the moment  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="C++")
	FColor ActiveColor = FColor::Red;

	/** Stores the required color for level. Not used at the moment  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="C++")
	FColor InactiveColor = FColor::Red;

	/** Current progression for each level  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="C++")
	int32 CurrentLevelProgression = 0;

	/** Required about of points to unlock level  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="C++")
	int32 PointsToUnlock = 0;

	/** Defines if level is locked or not */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="C++")
	bool IsLevelLocked = true;

	/** Amount of points to gain after win */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="C++")
	int32 WinReward = 0;

	/** Amount of points to gain after draw */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="C++")
	int32 DrawReward = 0;

	/** Amount of points to gain after loss */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="C++")
	int32 LossReward = 0;

	/** Amount of seconds to survive in order to provide loss rewards. 
	* Usage will depends if the game will provide any loss reward.
	* If yes, then this is against loss abuse system
	* NOTE: Currently, is not used. 
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="C++")
	float SecondsToSurvive = 0.f;
};

