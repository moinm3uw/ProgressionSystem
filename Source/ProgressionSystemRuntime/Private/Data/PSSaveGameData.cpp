// Copyright (c) Valerii Rotermel & Yevhenii Selivanov

#include "Data/PSSaveGameData.h"

#include "Data/PSDataAsset.h"
#include "Data/PSWorldSubsystem.h"
#include "Engine/CurveTable.h"
#include "Subsystems/GameDifficultySubsystem.h"
#include "Subsystems/GlobalEventsSubsystem.h"
#include "UtilityLibraries/MyBlueprintFunctionLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PSSaveGameData)

// Retrieves the name of the save slot, safely initializing the name statically to ensure thread safety and initialization order.
FString UPSSaveGameData::GetSaveSlotName(int32 SaveSlotVersion)
{
	// Using a function-static variable to avoid the static initialization order fiasco
	return FString::Printf(TEXT("%s-%d"), *StaticClass()->GetName(), SaveSlotVersion);
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
		const FPSRowData& CurrentProgressionSettingsRowData = UPSWorldSubsystem::Get().GetCurrentProgressionSettingsRowByName();

		// do nothing if max start achieved. Max stars of level is amount of point to unlock for a level
		if (CurrentSaveToDiskDataRowRef->CurrentLevelProgression >= CurrentProgressionSettingsRowData.PointsToUnlock)
		{
			return;
		}

		const float NewProgression = CurrentSaveToDiskDataRowRef->CurrentLevelProgression + GetProgressionReward(EndGameState);
		CurrentSaveToDiskDataRowRef->CurrentLevelProgression = FMath::Min(NewProgression, CurrentProgressionSettingsRowData.PointsToUnlock);

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

	int32 Index = 0;

	for (const TTuple<FName, FPSSaveToDiskData>& KeyValue : ProgressionSettingsRowDataInternal)
	{
		Index++;

		if (bNextRowFound)
		{
			UnlockLevelByName(KeyValue.Key);
			break;
		}

		if (KeyValue.Key == UPSWorldSubsystem::Get().GetCurrentRowName())
		{
			bNextRowFound = true; // Indicate that the current row has been found

			if (Index == ProgressionSettingsRowDataInternal.Num())
			{
				UGlobalEventsSubsystem::Get().OnGameProgressionCompleted.Broadcast();
			}
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

// Retrieves the progression reward based on the end game state for the current level.
float UPSSaveGameData::GetProgressionReward(EEndGameState EndGameState)
{
	constexpr float DefaultProgressionReward = 0.f;

	const UCurveTable* DifficultyCurveTable = UPSDataAsset::Get().GetProgressionDifficultyMultiplierCurveTable();
	if (!ensureMsgf(DifficultyCurveTable, TEXT("ASSERT: [%i] %hs:\n'DifficultyCurveTable' is not valid!"), __LINE__, __FUNCTION__))
	{
		return DefaultProgressionReward;
	}

	const FString ContextString = UEnum::GetDisplayValueAsText(EndGameState).ToString();
	const FName RowName = *ContextString;

	FCurveTableRowHandle Handle;
	Handle.CurveTable = DifficultyCurveTable;
	Handle.RowName = RowName;

	const FRealCurve* Curve = Handle.CurveTable->FindCurve(RowName, ContextString);
	if (!Curve)
	{
		return DefaultProgressionReward;
	}

	float MinTime = 0.f;
	float MaxTime = 0.f;
	Curve->GetTimeRange(/*out*/MinTime, /*out*/MaxTime);

	float DifficultyType = static_cast<float>(UGameDifficultySubsystem::Get().GetDifficultyLevel());

	DifficultyType = FMath::Clamp(DifficultyType, MinTime, MaxTime);

	float FoundProgressionReward = 0.f;
	const bool bIsFound = Handle.Eval(DifficultyType, /*out*/ &FoundProgressionReward, ContextString);

	return bIsFound ? FoundProgressionReward : DefaultProgressionReward;
}

// Returns the current save to disk data by name
const FPSSaveToDiskData& UPSSaveGameData::GetSaveToDiskDataByName(FName CurrentRowName)
{
	if (const FPSSaveToDiskData* FoundSeeting = ProgressionSettingsRowDataInternal.Find(CurrentRowName))
	{
		return *FoundSeeting;
	}

	return FPSSaveToDiskData::EmptyData;
}
