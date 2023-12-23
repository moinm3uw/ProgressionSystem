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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="C++")
	ELevelType Map = ELevelType::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="C++")
	FPlayerTag Character = FPlayerTag::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="C++")
	TObjectPtr<UTexture> Icon = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="C++")
	FColor ActiveColor = FColor::Red;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="C++")
	FColor InactiveColor = FColor::Red;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="C++")
	int32 CurrentLevelProgression = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="C++")
	int32 PointsToUnlock = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="C++")
	bool IsLevelLocked = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="C++")
	int32 WinReward = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="C++")
	int32 DrawReward = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="C++")
	int32 LossReward = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="C++")
	float SecondsToSurvive = 0.f;
};

