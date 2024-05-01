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

void UPSSaveGameData::SetRowByTag(FPlayerTag PlayerTag)
{
	for (const auto& KeyValue : SavedProgressionRowsInternal)
	{
		FPSRowData RowData = KeyValue.Value;

		if (RowData.Character == PlayerTag)
		{
			CurrentRowNameInternal = KeyValue.Key;
			break;
		}
	}
}

void UPSSaveGameData::UnlockLevelByName(FName RowName)
{
	FPSRowData& CurrentRowRef = SavedProgressionRowsInternal[RowName];
	CurrentRowRef.IsLevelLocked = false;
}

void UPSSaveGameData::SavePoints(EEndGameState EndGameState)
{
	FPSRowData& CurrentRowRef = SavedProgressionRowsInternal[CurrentRowNameInternal];
	CurrentRowRef.CurrentLevelProgression += GetProgressionReward(EndGameState);

	if (CurrentRowRef.CurrentLevelProgression >= CurrentRowRef.PointsToUnlock)
	{
		NextLevelProgressionRowData();
	}
	UPSWorldSubsystem::Get().SaveDataAsync();
}

void UPSSaveGameData::NextLevelProgressionRowData()
{
	bool bFound = false;

	for (const TTuple<FName, FPSRowData>& KeyValue : SavedProgressionRowsInternal)
	{
		if (bFound)
		{
			UnlockLevelByName(KeyValue.Key);
			break;
		}

		if (KeyValue.Key == CurrentRowNameInternal)
		{
			bFound = true;
		}
	}
	// Ensure if FName is not found
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
int32 UPSSaveGameData::GetProgressionReward(EEndGameState EndGameState)
{
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
		return 0;
	}
}
