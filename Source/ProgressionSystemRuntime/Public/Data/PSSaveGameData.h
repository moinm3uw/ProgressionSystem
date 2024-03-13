// Copyright (c) Valerii Rotermel & Yevhenii Selivanov

#pragma once

#include "Data/PSTypes.h"
#include "Templates/Tuple.h"
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
	static const FString& GetSaveSlotName();

	/** Returns the Slot Index of the save slot. */
	UFUNCTION(BlueprintPure, Category = "C++")
	static int32 GetSaveSlotIndex() { return 0; }

	/** Returns the ProgressionRow by Index */
	UFUNCTION(BlueprintPure, Category = "C++")
	const FPSRowData& GetSavedProgressionRowByIndex(int32 Index) const;

	/** Set element by index */
	UFUNCTION(BlueprintCallable, Category= "C++")
	void SetCurrentProgressionRowByIndex(int32 InIndex);

	/** Update the ProgressionRows map */
	UFUNCTION(BlueprintCallable, Category = "C++")
	void SetProgressionMap(const TMap<FName, FPSRowData>& ProgressionRows);

	/** Get current progression row */
	UFUNCTION(BlueprintPure, Category = "C++")
	const FPSRowData& GetCurrentRow() const;

	/** Get current progression row */
	UFUNCTION(BlueprintPure, Category = "C++")
	FORCEINLINE FName GetCurrentRowName() const { return CurrentRowNameInternal; }

	/** Set the current level by player tag*/
	UFUNCTION(BlueprintCallable, Category = "C++")
	void SetRowByTag(FPlayerTag PlayerTag);

	/** Unlock level by Index, used only for the first level */
	UFUNCTION(BlueprintCallable, Category = "C++")
	void UnlockLevelByName(FName RowName);

	/** Unlock level by Index, used only for the first level */
	UFUNCTION(BlueprintCallable, Category = "C++")
	void SavePoints(EEndGameState EndGameState);

	/** Unlocks the next level*/
	UFUNCTION(BlueprintCallable, Category = "C++")
	void NextLevelProgressionRowData();

	/** Returns the endgame reward. */
	UFUNCTION(BlueprintCallable, Category="C++")
	int32 GetProgressionReward(EEndGameState EndGameState);

protected:
	/** The current Index of Saved Progression. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="C++", meta = (BlueprintProtected, DisplayName = "Current Row Name"))
	FName CurrentRowNameInternal = NAME_None;

	/** The current Saved Progression of a player. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "C++", meta = (BlueprintProtected, DisplayName = "Saved Progression Rows"))
	TMap<FName, FPSRowData> SavedProgressionRowsInternal;
};
