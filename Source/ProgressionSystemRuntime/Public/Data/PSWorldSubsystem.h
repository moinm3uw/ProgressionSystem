// Copyright (c) Valerii Rotermel & Yevhenii Selivanov

#pragma once

#include "PSTypes.h"
#include "Subsystems/WorldSubsystem.h"
#include "PoolManagerTypes.h"
#include "PSWorldSubsystem.generated.h"

enum class ECurrentGameState : uint8;
enum class EPSStarActorState : uint8;

/**
 * Implements the world subsystem to access different components in the module 
 */
UCLASS(BlueprintType, Blueprintable, Config = "ProgressionSystem", DefaultConfig)
class PROGRESSIONSYSTEMRUNTIME_API UPSWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCurrentRowDataChanged, const FPlayerTag, SavedProgressionRowData);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPSOnInitialize);

	/** Returns this Subsystem, is checked and will crash if it can't be obtained.*/
	static UPSWorldSubsystem& Get();
	static UPSWorldSubsystem& Get(const UObject& WorldContextObject);

	/** Is called to initialize the world subsystem. It's a BeginPlay logic for the PS module */
	UFUNCTION(BlueprintNativeEvent, Category= "C++", meta = (BlueprintProtected))
	void OnWorldSubSystemInitialize();

	/** Cleanup used on unloading module to remove properties that should not be available by other objects. */
	UFUNCTION(BlueprintCallable, Category = "C++", meta = (BlueprintProtected))
	void PerformCleanUp();
	
	/** Set current row of progression system by tag*/
	UFUNCTION(BlueprintCallable, Category = "C++")
	void SetCurrentRowByTag(FPlayerTag NewRowPlayerTag);

	/* Delegate for informing row data changed */
	UPROPERTY(BlueprintAssignable, Transient, Category = "C++")
	FCurrentRowDataChanged OnCurrentRowDataChanged;

	/* Delegate for informing save game file is loaded/created if empty */
	UPROPERTY(BlueprintAssignable, Transient, Category = "C++")
	FPSOnInitialize OnInitialize;

	/** Returns the data asset that contains all the assets of Progression System game feature.
	 * @see UPSWorldSubsystem::PSDataAssetInternal. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "C++")
	const class UPSDataAsset* GetPSDataAsset() const;

	/** Returns a progression System component reference */
	UFUNCTION(BlueprintPure, Category = "C++")
	FORCEINLINE class UPSHUDComponent* GetProgressionSystemHUDComponent() const { return PSHUDComponentInternal; }

	/** Returns a current progression row name */
	UFUNCTION(BlueprintPure, Category = "C++")
	FORCEINLINE FName GetCurrentRowName() const { return CurrentRowNameInternal; }

	/** Returns a current progression save game data */
	UFUNCTION(BlueprintPure, Category = "C++")
	FORCEINLINE class UPSSaveGameData* GetCurrentSaveGameData() const { return SaveGameDataInternal; }

	/** Returns first save to disk row data */
	UFUNCTION(BlueprintPure, Category = "C++")
	FName GetFirstSaveToDiskRowName() const;

	/** Returns a current save to disk row by name */
	UFUNCTION(BlueprintPure, Category = "C++")
	const FPSSaveToDiskData& GetCurrentSaveToDiskRowByName() const;

	/** Returns a current progression row settings data row by name */
	UFUNCTION(BlueprintPure, Category = "C++")
	const FPSRowData& GetCurrentProgressionSettingsRowByName() const;

	/** Set the progression system component */
	UFUNCTION(BlueprintCallable, Category = "C++")
	void SetHUDComponent(class UPSHUDComponent* MyHUDComponent);

	/** Set the progression system spot component */
	UFUNCTION(BlueprintCallable, Category = "C++")
	void RegisterSpotComponent(class UPSSpotComponent* MyHUDComponent);

	/** Set the progression system spot component */
	UFUNCTION(BlueprintCallable, Category = "C++")
	void SetCurrentSpotComponent(class UPSSpotComponent* MyHUDComponent);

	/** Saves the progression to the local files */
	UFUNCTION()
	void SaveDataAsync();

	/** Removes all saved data of the Progression system and creates a new empty data */
	UFUNCTION(BlueprintCallable, Category = "C++")
	void ResetSaveGameData();

	/** Unlocks all levels of the Progression System */
	UFUNCTION(BlueprintCallable, Category = "C++")
	void UnlockAllLevels();

	/** Returns difficultyMultiplier */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="C++")
	float GetDifficultyMultiplier() const;

	/** Returns current spot component returns null if spot is not found */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="C++")
	UPSSpotComponent* GetCurrentSpot() const;

protected:
	/** Contains all the assets and tweaks of Progression System game feature.
	 * Note: Since Subsystem is code-only, there is config property set in BaseProgressionSystem.ini.
	 * Property is put to subsystem because its instance is created before any other object.
	 * It can't be put to DevelopSettings class because it does work properly for MGF-modules. */
	UPROPERTY(Config, VisibleInstanceOnly, BlueprintReadWrite, Category = "C++", meta = (BlueprintProtected, DisplayName = "Progression System Data Asset"))
	TSoftObjectPtr<const class UPSDataAsset> PSDataAssetInternal;

	/** Progression System component reference*/
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Transient, Category = "C++", meta = (BlueprintProtected, DisplayName = "Progression System HUD Component"))
	TObjectPtr<class UPSHUDComponent> PSHUDComponentInternal = nullptr;

	/** Progression System Array of Spot Components */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Transient, Category = "C++", meta = (BlueprintProtected, DisplayName = "Progression System Spot Array"))
	TArray<class UPSSpotComponent*> PSSpotComponentArrayInternal;

	/** Progression System Spot Component reference*/
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Transient, Category = "C++", meta = (BlueprintProtected, DisplayName = "Progression System Spot Component"))
	TObjectPtr<class UPSSpotComponent> PSCurrentSpotComponentInternal = nullptr;

	/** Store the current save game instance
	 * Contains the FPSSaveToDiskData which has actual data from save file */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "C++", meta = (BlueprintProtected, DisplayName = "Save Game Data Internal"))
	TObjectPtr<class UPSSaveGameData> SaveGameDataInternal = nullptr;

	/** Store default values from the progression settings data table cached once on load and never changed later */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Transient, Category = "C++", meta = (BlueprintProtected, DisplayName = "Save Game Instance"))
	TMap<FName, FPSRowData> ProgressionSettingsDataInternal;

	/** Store the current row name */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Transient, Category = "C++", meta = (BlueprintProtected, DisplayName = "Current Row Name"))
	FName CurrentRowNameInternal = NAME_None;

	/** Array of pool actors handlers which should be released */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Transient, Category = "C++", meta = (BlueprintProtected, DisplayName = "Pool Actors Handlers"))
	TArray<FPoolObjectHandle> PoolActorHandlersInternal;

	/** Store the material for dynamic progress material fill for a star actor */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "C++", meta = (BlueprintProtected, DisplayName = "Star Dynamic Progress Material"))
	TObjectPtr<class UMaterialInstanceDynamic> StarDynamicProgressMaterial = nullptr;

	/*********************************************************************************************
	* Protected functions
	********************************************************************************************* */
protected:
	/** Called when progression module ready
	 * Once the save file is loaded it activates the functionality of this class */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "C++", meta = (BlueprintProtected))
	void OnInitialized();
	
	/** Called when world is ready to start gameplay before the game mode transitions to the correct state and call BeginPlay on all actors */
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	/** Clears all transient data created by this subsystem. */
	virtual void Deinitialize() override;

	/** Is called when a player character is ready */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "C++", meta = (BlueprintProtected))
	void OnLocalCharacterReady(class APlayerCharacter* PlayerCharacter, int32 CharacterID);

	/** Is called when a player has been changed */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "C++", meta = (BlueprintProtected))
	void OnPlayerTypeChanged(FPlayerTag PlayerTag);

	/** Called when the current game state was changed. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "C++", meta = (BlueprintProtected))
	void OnGameStateChanged(ECurrentGameState CurrentGameState);

	/** Set first element as current active */
	UFUNCTION(BlueprintCallable, Category= "C++", meta = (BlueprintProtected))
	void SetFirstElementAsCurrent();

	/** Updates the stars actors for a spot by Spawning/adding the stars actors for a spot */
	UFUNCTION(BlueprintCallable, Category="C++", meta=(BlueprintProtected))
	void UpdateProgressionStarActors();

	/**
	 * Dynamically adds Star actors which representing unlocked and locked progression above the character
	 * @param CreatedObjects - Handles of objects from Pool Manager
	 */
	UFUNCTION(BlueprintCallable, Category= "C++")
	void OnTakeActorsFromPoolCompleted(const TArray<FPoolObjectData>& CreatedObjects);

	/** Triggers when a spot is loaded */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="C++", meta=(BlueprintProtected))
	void OnSpotComponentLoad(class UPSSpotComponent* SpotComponent);

	/** Is called from AsyncLoadGameFromSlot once Save Game is loaded, or null if it failed to load. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "C++", meta = (BlueprintProtected))
	void OnAsyncLoadGameFromSlotCompleted(const FString& SlotName, int32 UserIndex, class USaveGame* SaveGame);

	/** Is called to update the stars actors and in widgets when finish to save date in save file */
	UFUNCTION(BlueprintCallable, Category = "C++", meta = (BlueprintProtected))
	void UpdateProgressionUI();
};
