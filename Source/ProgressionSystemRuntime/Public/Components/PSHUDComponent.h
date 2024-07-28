// Copyright (c) Valerii Rotermel & Yevhenii Selivanov

#pragma once

#include "Bomber.h"
#include "Components/ActorComponent.h"
#include "GameFramework/MyPlayerState.h"
#include "Structures/PlayerTag.h"
//---
#include "PSHUDComponent.generated.h"

/**
 * Implements the core logic on project about Progression System.
 */

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROGRESSIONSYSTEMRUNTIME_API UPSHUDComponent final : public UActorComponent
{
	GENERATED_BODY()

public:
	/** Sets default values for this component's properties. */
	UPSHUDComponent();

	/** Save the progression depends on EEndGameState. */
	UFUNCTION(BlueprintCallable, Category="C++")
	void SavePoints(EEndGameState EndGameState);
	
	/** Updates the progression menu widget when player changed */
	UFUNCTION(BlueprintCallable, Category= "C++", meta = (BlueprintProtected))
	void UpdateProgressionWidgetForPlayer();
	
	/*********************************************************************************************
	* Protected properties
	********************************************************************************************* */
protected:
	/** Created Main Menu widget. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Transient, Category = "C++", meta = (BlueprintProtected, DisplayName = "Progression Menu Widget"))
	TObjectPtr<class UPSMenuWidget> ProgressionMenuWidgetInternal = nullptr;

	/** Current game state. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Transient, Category = "C++", meta = (BlueprintProtected, DisplayName = "Current Game state"))
	ECurrentGameState CurrentGameStateInternal = ECurrentGameState::None;
	
	/** Enabled Main Menu widget. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Transient, Category="C++", meta = (BlueprintProtected, DisplayName = "Enable Main Menu Widget"))
	bool PSMenuWidgetEnabledInternal = false;
	
	/*********************************************************************************************
	* Protected functions
	********************************************************************************************* */
protected:
	/** Subscribes to the end game state change notification on the player state. */
	UFUNCTION()
	void OnLocalPlayerStateReady(AMyPlayerState* PlayerState, int32 CharacterID);

	/** Called when the game starts. */
	virtual void BeginPlay() override;

	/** Clears all transient data created by this component. */
	virtual void OnUnregister() override;

	/** Called when the current game state was changed. */
	UFUNCTION(BlueprintCallable, Category = "C++", meta = (BlueprintProtected))
	void OnGameStateChanged(ECurrentGameState CurrentGameState);

	/** Called when the end game state was changed. */
	UFUNCTION(BlueprintCallable, Category = "C++", meta = (BlueprintProtected))
	void OnEndGameStateChanged(EEndGameState EndGameState);

	/** Is called when a player has been changed */
	UFUNCTION(BlueprintCallable, Category= "C++", meta = (BlueprintProtected))
	void OnPlayerTypeChanged(FPlayerTag PlayerTag);

	/** Show locked level ui overlay */
	UFUNCTION(BlueprintCallable, Category= "C++", meta = (BlueprintProtected))
	void DisplayLevelUIOverlay(bool IsLevelLocked);
};
