// Copyright (c) Valerii Rotermel & Yevhenii Selivanov

#pragma once

#include "ModuleStructures.h"
#include "ProgressionSystemDataAsset.h"
#include "Components/ActorComponent.h"
#include "Structures/PlayerTag.h"
//---
#include "ProgressionSystemComponent.generated.h"

/**
 * Implements the core logic on project about Progression System.
 */

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROGRESSIONSYSTEMRUNTIME_API UProgressionSystemComponent final : public UActorComponent
{
	GENERATED_BODY()

public:
	/** Sets default values for this component's properties. */
	UProgressionSystemComponent();

	/** Returns the Progression System data asset. */
	UFUNCTION(BlueprintPure, Category = "C++")
	static const UProgressionSystemDataAsset* GetProgressionSystemDataAsset() {return &UProgressionSystemDataAsset::Get(); }

	/** Returns current saved progression. */
	UFUNCTION(BlueprintCallable, Category="C++")
	FORCEINLINE FProgressionRowData GetSavedProgressionRowData() const { return SavedProgressionRowDataInternal; }

	/** Save the progression depends on EEndGameState. */
	UFUNCTION(BlueprintCallable, Category="C++")
	void SavePoints(ELevelType Map, FPlayerTag Character, EEndGameState EndGameState);

	/** Returns the endgame reward. */
	UFUNCTION(BlueprintCallable, Category="C++")
	int32 GetProgressionReward(ELevelType Map, FPlayerTag Character, EEndGameState EndGameState);

	/** Saves the current progression. */
	UFUNCTION(BlueprintCallable, Category="C++")
	void SaveCurrentGameProgression();

	UFUNCTION()
	void SaveDataAsync();

	/*********************************************************************************************
	* Protected properties
	********************************************************************************************* */
protected:
	/** Progression System data asset */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BlueprintProtected, DisplayName = "Progression System Data Asset"))
	TObjectPtr<UProgressionSystemDataAsset> ProgressionSystemDataAssetInternal;

	/** Created Main Menu widget. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Transient,  Category = "C++", meta = (BlueprintProtected, DisplayName = "Progression Menu Widget"))
	TObjectPtr<class UProgressionMenuWidget> ProgressionMenuWidgetInternal = nullptr;

	/** Created Save points widget. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Transient, Category = "C++", meta = (BlueprintProtected, DisplayName = "Progression Save Widget"))
	TObjectPtr<class UProgressionSaveWidget> ProgressionSaveWidgetInternal = nullptr;

	/** Store the save game instance */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "C++", meta = (BlueprintProtected, DisplayName = "Save Game Instance"))
	TObjectPtr<class UPSCSaveGame> SaveGameInstanceInternal = nullptr;

	/** The current Saved Progression of a player. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "C++", meta = (BlueprintProtected, DisplayName = "Saved Progression Row Data"))
	FProgressionRowData SavedProgressionRowDataInternal;

	/** The current selected player */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "C++", meta = (BlueprintProtected, DisplayName = "Current Player Tag"))
	FPlayerTag CurrentPlayerTagInternal = FPlayerTag::None;

	/** The Progression Data Table that is responsible for progression configuration. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Transient, Category = "C++", meta = (BlueprintProtected, DisplayName = "Progression Data Table"))
	TObjectPtr<UDataTable> ProgressionDataTableInternal = nullptr;

	/*********************************************************************************************
	* Protected functions
	********************************************************************************************* */
protected:
	/** Called when the game starts. */
	virtual void BeginPlay() override;

	/** Returns a current progression row name */
	UFUNCTION(BlueprintPure, Category="C++")
	FName GetProgressionRowName(ELevelType Map, FPlayerTag Character);

	/** */
	UFUNCTION(BlueprintCallable, Category="C++")
	void NextLevelProgressionRowData();
	
	/** Called when the current game state was changed. */
	UFUNCTION(BlueprintCallable, Category = "C++", meta = (BlueprintProtected))
	void OnGameStateChanged(ECurrentGameState CurrentGameState);

	/** Called when the end game state was changed. */
	UFUNCTION(BlueprintCallable, Category = "C++", meta = (BlueprintProtected))
	void OnEndGameStateChanged(EEndGameState EndGameState);
	
	/** Is called to prepare the widget for Menu game state. */
	UFUNCTION(BlueprintCallable, Category = "C++", meta = (BlueprintProtected))
	void HandleGameState(class AMyGameStateBase* MyGameState);

	/** Is called to prepare the widget for handling end game state. */
	UFUNCTION(BlueprintCallable, Category = "C++", meta = (BlueprintProtected))
	void HandleEndGameState(class AMyPlayerState* MyPlayerState);

	/** Is called to handle character possession event */
	UFUNCTION(BlueprintCallable, Category = "C++", meta = (BlueprintProtected))
	void OnCharacterPossessed(class APawn* MyPawn);
	
	/** Is called when a player has been changed */
	UFUNCTION(BlueprintCallable, Category= "C++", meta = (BlueprintProtected))
	void OnPlayerTypeChanged(FPlayerTag PlayerTag);

	/** Updates the progression menu widget when player changed */
	UFUNCTION(BlueprintCallable, Category= "C++", meta = (BlueprintProtected))
	void UpdateProgressionWidgetForPlayer();

	/** Show locked level ui overlay */
	UFUNCTION(BlueprintCallable, Category= "C++", meta = (BlueprintProtected))
	void DisplayLevelUIOverlay(bool IsLevelLocked);

	/** Load game from save */
	UFUNCTION(BlueprintCallable, Category= "C++", meta = (BlueprintProtected))
	void LoadGameFromSave();
};
