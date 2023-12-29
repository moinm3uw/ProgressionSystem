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
	static const FString& GetSaveSlotName();

	/** Returns the Slot Index of the save slot. */
	UFUNCTION(BlueprintPure, Category = "C++")
	static int32 GetSaveSlotIndex() { return 0; }

	/** The current Saved Progression of a player. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "C++", meta = (BlueprintProtected, DisplayName = "Saved Progression Row Data"))
	FPSRowData SavedProgressionRowDataInternal = FPSRowData::EmptyData;

	/** The current Saved Progression of a player. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "C++", meta = (BlueprintProtected, DisplayName = "Saved Progression Rows"))
	TMap<FName, FPSRowData> SavedProgressionRows;
};
