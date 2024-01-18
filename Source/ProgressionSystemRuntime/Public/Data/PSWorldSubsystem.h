// Copyright (c) Valerii Rotermel & Yevhenii Selivanov

#pragma once

#include "PSTypes.h"
#include "Subsystems/WorldSubsystem.h"
#include "PSWorldSubsystem.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FCurrentRowDataChanged, FPSRowData);
/**
 * Implements the world subsystem to access different components in the module 
 */
UCLASS(BlueprintType, Blueprintable, Config = "ProgressionSystem", DefaultConfig)
class PROGRESSIONSYSTEMRUNTIME_API UPSWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	/** Returns this Subsystem, is checked and wil crash if can't be obtained.*/
	static UPSWorldSubsystem& Get();
	static UPSWorldSubsystem& Get(const UObject& WorldContextObject);

	/* Delegate for informing row data changed */
	FCurrentRowDataChanged OnCurrentRowDataChanged;
	
	/** Returns the data asset that contains all the assets of Progression System game feature.
	 * @see UPSWorldSubsystem::PSDataAssetInternal. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "C++")
	const class UPSDataAsset* GetPSDataAsset() const;
	
	/** Returns a progression System component reference */
	UFUNCTION(BlueprintPure, Category = "C++")
	FORCEINLINE class UPSHUDComponent* GetProgressionSystemHUDComponent() const { return PSHUDComponentInternal;	}

	/** Returns a current row data */
	UFUNCTION(BlueprintPure, Category = "C++")
	FORCEINLINE FPSRowData GetCurrentRowData() const { return SavedProgressionRowDataInternal; }

	/** Returns a current save game instance */
	UFUNCTION(BlueprintPure, Category = "C++")
	FORCEINLINE UPSSaveGameData* GetCurrentSaveGameData() const { return SaveGameInstanceInternal; }

	/** Set the progression system component */
	UFUNCTION(BlueprintCallable, Category = "C++")
	void SetProgressionSystemComponent(UPSHUDComponent* MyProgressionSystemComponent);

	/** Saves the progression to the local files */ 
	UFUNCTION()
	void SaveDataAsync();

protected:
	/** Contains all the assets and tweaks of Progression System game feature.
	 * Note: Since Subsystem is code-only, there is config property set in BasePSDataAssetInternal.ini.
	 * Property is put to subsystem because its instance is created before any other object.
	 * It can't be put to DevelopSettings class because it does work properly for MGF-modules. */
	UPROPERTY(Config, VisibleInstanceOnly, BlueprintReadWrite, Category = "C++", meta = (BlueprintProtected, DisplayName = "Progression System Data Asset"))
	TSoftObjectPtr<const class UPSDataAsset> PSDataAssetInternal;
	
	/** Progression System component reference*/
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Transient, Category = "C++", meta = (BlueprintProtected, DisplayName = "Progression System HUD Component"))
	TObjectPtr<class UPSHUDComponent> PSHUDComponentInternal = nullptr;

	/** Store the current save game instance */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "C++", meta = (BlueprintProtected, DisplayName = "Save Game Instance"))
	TObjectPtr<class UPSSaveGameData> SaveGameInstanceInternal = nullptr;

	/** The current Saved Progression of a player. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "C++", meta = (BlueprintProtected, DisplayName = "Saved Progression Row Data"))
	FPSRowData SavedProgressionRowDataInternal = FPSRowData::EmptyData;

	/** The Progression Data Table that is responsible for progression configuration. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Transient, Category = "C++", meta = (BlueprintProtected, DisplayName = "Progression Data Table"))
	TObjectPtr<UDataTable> ProgressionDataTableInternal = nullptr;
	
	/*********************************************************************************************
	* Protected functions
	********************************************************************************************* */
protected:
	
	/** Called when world is ready to start gameplay before the game mode transitions to the correct state and call BeginPlay on all actors */
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	/** Is called to handle character possession event */
	UFUNCTION(BlueprintCallable, Category = "C++", meta = (BlueprintProtected))
	void OnCharacterPossessed(class APawn* MyPawn);

	/** Is called when a player has been changed */
	UFUNCTION(BlueprintCallable, Category= "C++", meta = (BlueprintProtected))
	void OnPlayerTypeChanged(FPlayerTag PlayerTag);

	/** Load game from save */
	UFUNCTION(BlueprintCallable, Category= "C++", meta = (BlueprintProtected))
	void LoadGameFromSave();

	/** Set first element as current active */
	UFUNCTION(BlueprintCallable, Category= "C++", meta = (BlueprintProtected))
	void SetFirstElemetAsCurrent();

	
};
