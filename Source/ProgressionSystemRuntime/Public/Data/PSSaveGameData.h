// Copyright (c) Valerii Rotermel & Yevhenii Selivanov

#pragma once

#include "Data/PSTypes.h"
#include "GameFramework/SaveGame.h"
#include "PSSaveGameData.generated.h"


/**
 * Defines the standard process for the saving slots names and index 
 */
UCLASS()
class PROGRESSIONSYSTEMRUNTIME_API UPSSaveGameData : public USaveGame
{
	GENERATED_BODY()

public:
	/** Returns the name of the save slot. */
	UFUNCTION(BlueprintPure, Category = "C++")
	static FString GetSaveSlotName(int32 SaveSlotVersion);

	/** Returns the Slot Index of the save slot. */
	UFUNCTION(BlueprintPure, Category = "C++")
	static int32 GetSaveSlotIndex() { return 0; }

	/** Returns the Slot Index of the save slot. */
	UFUNCTION(BlueprintPure, Category = "C++")
	FORCEINLINE TMap<FName, FPSSaveToDiskData>& GetProgressionSettingsRowDataInternal() { return ProgressionSettingsRowDataInternal; }

	/** Returns the ProgressionRow by Index */
	UFUNCTION(BlueprintPure, Category = "C++")
	FName GetSavedProgressionRowByIndex(int32 Index) const;

	/** Update the ProgressionRows map */
	UFUNCTION(BlueprintCallable, Category = "C++")
	void SetProgressionMap(FName RowName, const FPSSaveToDiskData& ProgressionRows);

	/** Unlock level by Index, used only for the first level */
	UFUNCTION(BlueprintCallable, Category = "C++")
	void UnlockLevelByName(FName RowName);

	/** Unlock level by Index, used only for the first level */
	UFUNCTION(BlueprintCallable, Category = "C++")
	void SavePoints(EEndGameState EndGameState);

	/** Unlocks the next level*/
	UFUNCTION(BlueprintCallable, Category = "C++")
	void NextLevelProgressionRowData();

	/** Unlocks all levels and set maximum allowed progression points */
	UFUNCTION(BlueprintCallable, Category = "C++")
	void UnlockAllLevels();

	/** Returns the endgame reward. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="C++")
	float GetProgressionReward(EEndGameState EndGameState) const;

	/** Returns the current save to disk data by name. */
	UFUNCTION(BlueprintCallable, Category="C++")
	const FPSSaveToDiskData& GetSaveToDiskDataByName(FName CurrentRowName);

	/** Returns the maximum number of levels possible from all levels. If Maya - 3 points to unlock, Hugo - 5 points, returned amount:  5) */
	UFUNCTION(BlueprintCallable, Category = "C++")
	const int32 GetMaxNumberOfUnlockableLevels();

protected:
	/** The current Saved Progression of a player. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "C++", meta = (BlueprintProtected, DisplayName = "Saved Progression Rows"))
	TMap<FName, FPSSaveToDiskData> ProgressionSettingsRowDataInternal;
};
