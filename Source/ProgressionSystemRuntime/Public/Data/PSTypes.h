// Copyright (c) Valerii Rotermel & Yevhenii Selivanov

#pragma once

#include "Bomber.h"
#include "Structures/PlayerTag.h"
#include "Engine/DataTable.h"
#include "PSTypes.generated.h"

/**
 * Basic structure for all progression settings data
 * Initial load performed once based on the data in the DT Table and never changed later
 */
USTRUCT(BlueprintType)
struct FPSRowData : public FTableRowBase
{
	GENERATED_BODY()

	static const FPSRowData EmptyData;

	/** Default constructor. */
	FPSRowData() = default;

	/** Stores the value of the map for progression system component */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="C++")
	ELevelType Map = ELevelType::None;

	/** Contains the character player tag used in the save/load progression system */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="C++")
	FPlayerTag Character = FPlayerTag::None;

	/** Transform of Stars above the character on a level */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="C++")
	FTransform StarActorTransform = FTransform::Identity;

	/** Offset between stars for stars above the character on a level */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="C++")
	FVector OffsetBetweenStarActors = FVector::ZeroVector;

	/** Required about of points to unlock level  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="C++")
	float PointsToUnlock = 0.f;

	/** Base scores per end-game result before applying difficulty multiplier. E.g. the number of stars a player receives upon winning the game. 
	* If End-Game state is not matching with game result, 0 score will be granted by default. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="C++", meta = (DisplayName = "Progression End Game States"))
	TMap<EEndGameState, float> ProgressionEndGameValues;

	/** Defines the star animations for each character called when in-game cinematic played */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="C++")
	TObjectPtr<class UCurveTable> HideStarsAnimation = nullptr;

	/** Defines the star animations for each character called when in-game cinematic played */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="C++")
	TObjectPtr<class UCurveTable> MenuStarsAnimation = nullptr;
};

/**
 *  Basic structure for all save data information regarding the progression
 *  The data can be modified in run-time and saved to the disk
 *  Same structure will be reflected in the save file. 
 */
USTRUCT(BlueprintType)
struct FPSSaveToDiskData
{
	GENERATED_BODY()

	static const FPSSaveToDiskData EmptyData;

	/** Default constructor. */
	FPSSaveToDiskData() = default;

	/** Current progression for each level  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="C++")
	float CurrentLevelProgression = 0.f;

	/** Defines if level is locked or not */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="C++")
	bool IsLevelLocked = true;
};


/**
 * Represents the state of the overlay widget fade animation played in the menu.
 */
UENUM(BlueprintType, DisplayName = "Overlay Widget Fade Animation State")
enum class EPSOverlayWidgetFadeState : uint8
{
	///< Is not in the Menu
	None,
	///< Fade-it animation
	FadeIn,
	///< Fade-out animation 
	FadeOut,
};

/**
 * Represents the state of the stars states displayed in the main menu
 */
UENUM(BlueprintType, DisplayName = "Overlay Widget Fade Animation State")
enum class EPSStarActorState : uint8
{
	///< Is not defined
	None,
	///< Star is locked
	Locked,
	///< Star is unlocked 
	Unlocked,
};