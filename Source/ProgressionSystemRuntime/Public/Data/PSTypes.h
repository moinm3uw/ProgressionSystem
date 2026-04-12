// Copyright (c) Valerii Rotermel & Yevhenii Selivanov

#pragma once

#include "Structures/BmrPlayerTag.h"
#include "Engine/DataTable.h"
#include "PSTypes.generated.h"

/**
 * Basic structure for all progression settings data
 * Initial load performed once based on the data in the DT Table and never changed later
 */
USTRUCT(BlueprintType)
struct FPSSettingsRow : public FTableRowBase
{
	GENERATED_BODY()

	static const FPSSettingsRow EmptyData;

	/** Default constructor. */
	FPSSettingsRow() = default;

	/** Contains the character player tag used in the save/load progression system */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="C++")
	FBmrPlayerTag Character = FBmrPlayerTag::None;

	/** Transform of Stars above the character on a level */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="C++")
	FTransform StarActorTransform = FTransform::Identity;

	/** Offset between stars for stars above the character on a level */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="C++")
	FVector OffsetBetweenStarActors = FVector::ZeroVector;

	/** Required about of points to unlock level  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="C++")
	float PointsToUnlock = 0.f;

	/** Defines the star animations for each character called when in-game cinematic played */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="C++")
	TObjectPtr<class UCurveTable> HideStarsAnimation = nullptr;

	/** Defines the star animations for each character called when in-game cinematic played */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="C++")
	TObjectPtr<class UCurveTable> MenuStarsAnimation = nullptr;

	/** Returns true is this does not contain any data. */
	bool FORCEINLINE IsValid() const { return Character.IsValid() && PointsToUnlock > 0.f; }
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

	/* Stores the amount of total unlocked skins index */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="C++")
	int32 UnlockedSkinsAmount = 0;
};

/**
 * Represents animations of the overlay widget animations played in the menu
 */
UENUM(BlueprintType, DisplayName = "Overlay Widget Animation Type")
enum class EPSOverlayWidgetAnimationName : uint8
{
	///< Default fade no animation required
	None,
	///< Fade-it animation
	FadeIn,
	///< Fade-out animation 
	FadeOut,
};

/**
 * Represents type of the overlay widget animation played in the menu.
 */
UENUM(BlueprintType, DisplayName = "Overlay Widget Animation Type")
enum class EPSOverlayWidgetAnimationType : uint8
{
	///< no type to be applied
	None,
	///< Fade type animation
	Fade,
	///< Instant (no animation) type 
	Instant,
};

/**
 * Represents the state of the stars states displayed in the main menu
 */
UENUM(BlueprintType, DisplayName = "Progression Stars State")
enum class EPSStarActorState : uint8
{
	///< Is not defined
	None,
	///< Star is locked
	Locked,
	///< Star is unlocked 
	Unlocked,
	///< Star is partially unlocked
	Partial
};
