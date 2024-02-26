// Copyright (c) Valerii Rotermel & Yevhenii Selivanov

#include "Data/PSSaveGameData.h"

#include "Data/PSWorldSubsystem.h"
#include "UtilityLibraries/MyBlueprintFunctionLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PSSaveGameData)

const FString& UPSSaveGameData::GetSaveSlotName()
{
	static const FString SaveSlotName = StaticClass()->GetName();
	return SaveSlotName;
}

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

void UPSSaveGameData::SetCurrentProgressionRowByIndex(int32 InIndex)
{
	int32 Idx = 0;
	for (const TTuple<FName, FPSRowData>& It : SavedProgressionRowsInternal)
	{
		if (Idx == InIndex)
		{
			CurrentRowName = It.Key;
			break;
		}
		Idx++;
	}
}

void UPSSaveGameData::SetProgressionMap(const TMap<FName, FPSRowData> ProgressionRows)
{
	if (ensureMsgf(!ProgressionRows.IsEmpty(), TEXT("ASSERT: ProgressionRows is empty")))
	{
		SavedProgressionRowsInternal = ProgressionRows;
	}
}

const FPSRowData& UPSSaveGameData::GetCurrentRow() const
{
	for (const auto& KeyValue : SavedProgressionRowsInternal)
	{
		if (KeyValue.Key == CurrentRowName)
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
			CurrentRowName = KeyValue.Key;
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
	FPSRowData& CurrentRowRef = SavedProgressionRowsInternal[CurrentRowName];
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

		if (KeyValue.Key == CurrentRowName)
		{
			bFound = true;
		}
	}
	// Ensure if FName is not found
}

// @h4rdmol - make function const 
int32 UPSSaveGameData::GetProgressionReward(EEndGameState EndGameState)
{
	const FPSRowData& CurrentRow = SavedProgressionRowsInternal[CurrentRowName];

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
