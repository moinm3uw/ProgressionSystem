// Copyright (c) Valerii Rotermel & Yevhenii Selivanov

#pragma once

#include "Bomber.h"
#include "Structures/PlayerTag.h"
#include "Engine/DataTable.h"
#include "PSTypes.generated.h"

/**
 * Basic structure for all save data information regarding the progression.
 * Same structure will reflected in the save file. Initial load performed based on the data in the DT Table 
 */
USTRUCT(BlueprintType)
struct FPSRowData : public FTableRowBase
{
	GENERATED_BODY()

	static const FPSRowData EmptyData;

	/** Default constructor. */
	FPSRowData() = default;

	/** Stores the value of the map for progression system component */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="C++")
	ELevelType Map = ELevelType::None;

	/** Contains the character player tag used in the save/load progression system */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="C++")
	FPlayerTag Character = FPlayerTag::None;

	/** Current progression for each level  */
	UPROPERTY()
	float CurrentLevelProgression = 0.f;

	/** Required about of points to unlock level  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="C++")
	float PointsToUnlock = 0.f;

	/** Amount of points to gain after win */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="C++")
	float WinReward = 0.f;

	/** Amount of points to gain after draw */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="C++")
	float DrawReward = 0.f;

	/** Amount of points to gain after loss */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="C++")
	float LossReward = 0.f;

	/** Defines if level is locked or not */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="C++")
	bool IsLevelLocked = true;
};
