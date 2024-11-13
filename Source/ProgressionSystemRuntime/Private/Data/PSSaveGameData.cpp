// Copyright (c) Valerii Rotermel & Yevhenii Selivanov

#include "Data/PSSaveGameData.h"

#include "Data/PSWorldSubsystem.h"
#include "UtilityLibraries/MyBlueprintFunctionLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PSSaveGameData)

// Retrieves the name of the save slot, safely initializing the name statically to ensure thread safety and initialization order.
const FString& UPSSaveGameData::GetSaveSlotName()
{
	// Using a function-static variable to avoid the static initialization order fiasco
	static const FString SaveSlotName = StaticClass()->GetName();
	return SaveSlotName;
}

// Retrieves the saved game progression row by index from internal saved rows. If the index is out of range, returns a static empty data object. 
FName UPSSaveGameData::GetSavedProgressionRowByIndex(int32 Index) const
{
	int32 Idx = 0;
	for (const TTuple<FName, FPSSaveToDiskData>& It : ProgressionSettingsRowDataInternal)
	{
		if (Idx == Index)
		{
			return It.Key;
		}
	}
	return FName();
}

// Sets the progression map with a new set of progression rows. Ensures the new map is not empty before assignment.
void UPSSaveGameData::SetProgressionMap(FName RowName, const FPSSaveToDiskData& ProgressionRows)
{
	ProgressionSettingsRowDataInternal.Add(RowName, ProgressionRows);
}

// Unlocks the level specified by RowName if it exists in the saved progression rows.
void UPSSaveGameData::UnlockLevelByName(FName RowName)
{
	// Using the operator [] on a TMap like SavedProgressionRowsInternal[RowName] can inadvertently create a new entry in the map if RowName does not exist
	// Check if the row exists to avoid inadvertently creating a new entry
	if (ProgressionSettingsRowDataInternal.Contains(RowName))
	{
		FPSSaveToDiskData& CurrentRowRef = ProgressionSettingsRowDataInternal[RowName];
		CurrentRowRef.IsLevelLocked = false;
	}
}

// Updates the current level's progression based on the end game state and proceeds to the next level if unlocked.
void UPSSaveGameData::SavePoints(EEndGameState EndGameState)
{
	// Check if the current row exists in the map before attempting to update it
	if (ProgressionSettingsRowDataInternal.Contains(UPSWorldSubsystem::Get().GetCurrentRowName()))
	{
		// Increase the current level's progression by the reward from the end game state
		FName CurrentRowName = UPSWorldSubsystem::Get().GetCurrentRowName();
		FPSSaveToDiskData* CurrentSaveToDiskDataRowRef = ProgressionSettingsRowDataInternal.Find(CurrentRowName);
		CurrentSaveToDiskDataRowRef->CurrentLevelProgression += GetProgressionReward(EndGameState);

		const FPSRowData& CurrentProgressionSettingsRowData = UPSWorldSubsystem::Get().GetCurrentProgressionSettingsRowByName();

		// Check if the current level progression has reached or surpassed the points needed to unlock
		if (CurrentSaveToDiskDataRowRef->CurrentLevelProgression >= CurrentProgressionSettingsRowData.PointsToUnlock)
		{
			NextLevelProgressionRowData(); // Advance to the next level's progression data
		}
		UPSWorldSubsystem::Get().SaveDataAsync(); // Asynchronously save the updated data
	}
}

// Advances to the next level progression row and unlocks it, if available, after the current row.
void UPSSaveGameData::NextLevelProgressionRowData()
{
	bool bNextRowFound = false;

	for (const TTuple<FName, FPSSaveToDiskData>& KeyValue : ProgressionSettingsRowDataInternal)
	{
		if (bNextRowFound)
		{
			UnlockLevelByName(KeyValue.Key);
			break;
		}

		if (KeyValue.Key == UPSWorldSubsystem::Get().GetCurrentRowName())
		{
			bNextRowFound = true; // Indicate that the current row has been found
		}
	}
}

// Unlocks all levels and set maximum allowed progression points
void UPSSaveGameData::UnlockAllLevels()
{
	for (TTuple<FName, FPSSaveToDiskData>& KeyValue : ProgressionSettingsRowDataInternal)
	{
		UnlockLevelByName(KeyValue.Key);
		KeyValue.Value.CurrentLevelProgression = UPSWorldSubsystem::Get().GetCurrentProgressionSettingsRowByName().PointsToUnlock;
	}
}

// @h4rdmol - make function const
// Retrieves the progression reward based on the end game state for the current level.
float UPSSaveGameData::GetProgressionReward(EEndGameState EndGameState)
{
	constexpr float DefaultMultiplier = 1.0f;
	const float DifficultyMultiplier = UPSWorldSubsystem::Get().GetDifficultyMultiplier();
	const FPSRowData& CurrentProgressionSettingsRowData = UPSWorldSubsystem::Get().GetCurrentProgressionSettingsRowByName();

	const float* LevelReward = CurrentProgressionSettingsRowData.ProgressionEndGameValues.Find(EndGameState);
	const float ProgressionReward = LevelReward ? *LevelReward : DefaultMultiplier;
	return ProgressionReward * DifficultyMultiplier;
}

// Returns the current save to disk data by name
FPSSaveToDiskData& UPSSaveGameData::GetSaveToDiskDataByName(FName CurrentRowName)
{
	return *ProgressionSettingsRowDataInternal.Find(CurrentRowName);
}
