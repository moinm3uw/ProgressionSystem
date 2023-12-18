// Copyright (c) Yevhenii Selivanov

#pragma once

#include "CoreMinimal.h"
#include "ModuleStructures.h"
#include "GameFramework/SaveGame.h"
#include "PSCSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class PROGRESSIONSYSTEMRUNTIME_API UPSCSaveGame : public USaveGame
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
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Saved Progression", ShowOnlyInnerProperties))
	FProgressionRowData SavedProgressionRowDataInternal;

	/** The current Saved Progression of a player. */
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Saved Progression TMap", ShowOnlyInnerProperties))
	TMap<FName, FProgressionRowData> SavedProgressionRows;
	
};
