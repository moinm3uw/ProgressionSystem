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
const FPSRowData& UPSSaveGameData::GetSavedProgressionRowByIndex(int32 Index) const
{
	int32 Idx = 0;
	for (const TTuple<FName, FPSRowData>& It : SavedProgressionRowsInternal)
	{
		if (Idx == Index)
		{
			return It.Value;
		}
	}
	return FPSRowData::EmptyData;
}
// Sets the current progression row based on the provided index.
// This updates the CurrentRowNameInternal to the FName corresponding to the InIndex in SavedProgressionRowsInternal.
void UPSSaveGameData::SetCurrentProgressionRowByIndex(int32 InIndex)
{
	int32 Idx = 0;
	for (const TTuple<FName, FPSRowData>& It : SavedProgressionRowsInternal)
	{
		if (Idx == InIndex)
		{
			CurrentRowNameInternal = It.Key;
			return;; // Exit the function once the correct index is found
		}
		Idx++;
	}
}

// Sets the progression map with a new set of progression rows. Ensures the new map is not empty before assignment.
void UPSSaveGameData::SetProgressionMap(const TMap<FName, FPSRowData>& ProgressionRows)
{
	if (ensureMsgf(!ProgressionRows.IsEmpty(), TEXT("ASSERT: ProgressionRows is empty")))
	{
		SavedProgressionRowsInternal = ProgressionRows;
	}
}
// Retrieves the current row data based on the internally stored row name. Returns empty data if the row name isn't found.
const FPSRowData& UPSSaveGameData::GetCurrentRow() const
{
	static const FPSRowData EmptyData; // Ensure EmptyData is a static member to safely return it by reference
	
	for (const TTuple<FName, FPSRowData>& KeyValue : SavedProgressionRowsInternal)
	{
		if (KeyValue.Key == CurrentRowNameInternal)
		{
			return KeyValue.Value;
		}
	}
	return FPSRowData::EmptyData;
}
// Sets the current row based on the provided player tag. The first row matching the tag becomes the current row.
void UPSSaveGameData::SetRowByTag(FPlayerTag PlayerTag)
{
	for (const auto& KeyValue : SavedProgressionRowsInternal)
	{
		const FPSRowData& RowData = KeyValue.Value;

		if (RowData.Character == PlayerTag)
		{
			CurrentRowNameInternal = KeyValue.Key;
			return;  // Exit immediately after finding the match
		}
	}
}

// Unlocks the level specified by RowName if it exists in the saved progression rows.
void UPSSaveGameData::UnlockLevelByName(FName RowName)
{
	// Using the operator [] on a TMap like SavedProgressionRowsInternal[RowName] can inadvertently create a new entry in the map if RowName does not exist
	// Check if the row exists to avoid inadvertently creating a new entry
	if (SavedProgressionRowsInternal.Contains(RowName))
	{
		FPSRowData& CurrentRowRef = SavedProgressionRowsInternal[RowName];
		CurrentRowRef.IsLevelLocked = false;
	}
	else
	{
		// log an error for the case where RowName does not exist
		UE_LOG(LogTemp, Warning, TEXT("UnlockLevelByName: Row name '%s' not found in the progression rows."), *RowName.ToString());
	}
}

// Updates the current level's progression based on the end game state and proceeds to the next level if unlocked.
void UPSSaveGameData::SavePoints(EEndGameState EndGameState)
{
	// Check if the current row exists in the map before attempting to update it
	if (SavedProgressionRowsInternal.Contains(CurrentRowNameInternal))
	{
		FPSRowData& CurrentRowRef = SavedProgressionRowsInternal[CurrentRowNameInternal];
		// Increase the current level's progression by the reward from the end game state
		CurrentRowRef.CurrentLevelProgression += GetProgressionReward(EndGameState);

		// Check if the current level progression has reached or surpassed the points needed to unlock
		if (CurrentRowRef.CurrentLevelProgression >= CurrentRowRef.PointsToUnlock)
		{
			NextLevelProgressionRowData();  // Advance to the next level's progression data
		}
		UPSWorldSubsystem::Get().SaveDataAsync();  // Asynchronously save the updated data
	}
}

// Advances to the next level progression row and unlocks it, if available, after the current row.
void UPSSaveGameData::NextLevelProgressionRowData()
{
	bool bNextRowFound = false;

	for (const TTuple<FName, FPSRowData>& KeyValue : SavedProgressionRowsInternal)
	{
		if (bNextRowFound)
		{
			UnlockLevelByName(KeyValue.Key);
			break;
		}

		if (KeyValue.Key == CurrentRowNameInternal)
		{
			bNextRowFound = true;  // Indicate that the current row has been found
		}
	}
}

// Unlocks all levels and set maximum allowed progression points
void UPSSaveGameData::UnlockAllLevels()
{
	for (TTuple<FName, FPSRowData>& KeyValue : SavedProgressionRowsInternal)
	{
		UnlockLevelByName(KeyValue.Key);
		KeyValue.Value.CurrentLevelProgression = KeyValue.Value.PointsToUnlock;
	}
}

// @h4rdmol - make function const
// Retrieves the progression reward based on the end game state for the current level.
float UPSSaveGameData::GetProgressionReward(EEndGameState EndGameState)
{
	// Verify that the current row exists in the map to prevent creating a new entry
	if (!SavedProgressionRowsInternal.Contains(CurrentRowNameInternal))
	{
		UE_LOG(LogTemp, Warning, TEXT("GetProgressionReward: CurrentRowName '%s' not found."), *CurrentRowNameInternal.ToString());
		return 0.f;  // Return a default reward of 0.f if the row does not exist
	}
	
	const FPSRowData& CurrentRow = SavedProgressionRowsInternal[CurrentRowNameInternal];

	switch (EndGameState)
	{
	case EEndGameState::Win:
		return CurrentRow.WinReward;
	case EEndGameState::Draw:
		return CurrentRow.DrawReward;
	case EEndGameState::Lose:
		return CurrentRow.LossReward;
	default:
		return 0.f; // Return a default reward of 0.f if the row does not exist
	}
}