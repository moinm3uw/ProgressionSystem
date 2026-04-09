// Copyright (c) Valerii Rotermel & Yevhenii Selivanov

#pragma once

#include "DalPrimaryDataAsset.h"

// BMR
#include "Data/SettingTag.h"

#include "PSDataAsset.generated.h"

/**
 * Contains all progression assets used in the module
 */
UCLASS(Blueprintable, BlueprintType)
class PROGRESSIONSYSTEMRUNTIME_API UPSDataAsset : public UDalPrimaryDataAsset
{
	GENERATED_BODY()

public:
	/** Returns the progression data asset or crash if can not be obtained. */
	static const UPSDataAsset& Get();

	/** Returns the Progression Data Table
	 * @see UProgressionSystemDataAsset::ProgressionDataTableInternal */
	UFUNCTION(BlueprintPure, Category = "C++")
	const FORCEINLINE class UDataTable* GetProgressionDataTable() const { return ProgressionDataTableInternal; }

	/** Returns a locked progression icon reference */
	UFUNCTION(BlueprintPure, Category = "C++")
	FORCEINLINE class UTexture2D* GetLockedProgressionIcon() const { return LockedProgressionIconInternal; }

	/** Returns a unlocked progression icon reference */
	UFUNCTION(BlueprintPure, Category = "C++")
	FORCEINLINE class UTexture2D* GetUnlockedProgressionIcon() const { return UnlockedProgressionIconInternal; }

	/** Returns a star widget  */
	UFUNCTION(BlueprintPure, Category = "C++")
	FORCEINLINE TSubclassOf<class UPSStarWidget> GetStarWidgetClass() const { return StarWidgetInternal; }

	/** Returns a star widget  */
	UFUNCTION(BlueprintPure, Category = "C++")
	FORCEINLINE TSubclassOf<class AActor> GetStarActorClass() const { return StarActorClassInternal; }

	/** Returns Material overlay applied as dynamic progression (character's bomb is filled partially depends on progression) in the main menu */
	UFUNCTION(BlueprintPure, Category = "C++")
	FORCEINLINE class UMaterialInterface* GetBombDynamicProgressionOverlayMaterial() const { return DynamicProgressionBombOverlayMaterialInternal; }

	/** Returns progression difficulty multiplier */
	UFUNCTION(BlueprintPure, Category = "C++")
	const FORCEINLINE class UCurveTable* GetProgressionDifficultyMultiplierCurveTable() const { return ProgressionDifficultyMultiplierCurveTableInternal; }

	/** Returns the duration of fade-in/fade-out overlay animation in the main menu when cinematic started */
	UFUNCTION(BlueprintPure, Category = "C++")
	FORCEINLINE float GetOverlayFadeDuration() const { return FadeDurationInternal; }

	/** Star Material Slot name to change the dynamic fill-in based on the progression */
	UFUNCTION(BlueprintPure, Category = "C++")
	FORCEINLINE FName GetStarMaterialSlotName() const { return StarPercentSlotNameInternal; }

	/** Returns Instant Character Switch Tag. When Instant character switch setting enabled fade animation will not be played */
	UFUNCTION(BlueprintPure, Category = "C++")
	FORCEINLINE FSettingTag GetInstantCharacterSwitchTag() const { return InstantCharacterSwitchTagInternal; }

	/** Returns Skin Unlock Interval. Amount of stars required to unlock a skin */
	UFUNCTION(BlueprintPure, Category = "C++")
	FORCEINLINE int32 GetSkinUnlockInterval() const { return SkinUnlockIntervalInternal; }

protected:
	/** The Progression Data Table that is responsible for progression configuration. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BlueprintProtected, DisplayName = "Progression Data Table", ShowOnlyInnerProperties))
	TObjectPtr<UDataTable> ProgressionDataTableInternal = nullptr;

	/** Star icon widget */
	UPROPERTY(EditAnywhere, Category = "UI", meta = (BlueprintProtected, DisplayName = "Star Widget"))
	TSubclassOf<class UPSStarWidget> StarWidgetInternal = nullptr;

	/** Star icon widget */
	UPROPERTY(EditAnywhere, Category = "UI", meta = (BlueprintProtected, DisplayName = "Star Actor Class"))
	TSubclassOf<class AActor> StarActorClassInternal = nullptr;

	/** Image for locked progression */
	UPROPERTY(EditAnywhere, Category = "UI|Material", meta = (BlueprintProtected, DisplayName = "Locked Progression Icon"))
	TObjectPtr<class UTexture2D> LockedProgressionIconInternal = nullptr;

	/** Image for unlocked progression */
	UPROPERTY(EditAnywhere, Category = "UI|Material", meta = (BlueprintProtected, DisplayName = "Unlocked Progression Icon"))
	TObjectPtr<class UTexture2D> UnlockedProgressionIconInternal = nullptr;

	/** Material applied as dynamic progression overlay material (character's bomb is filled partially depends on progression) in the main menu*/
	UPROPERTY(EditAnywhere, Category = "UI|Material", meta = (BlueprintProtected, DisplayName = "Dynamic Progression Bomb Overlay Material"))
	TObjectPtr<class UMaterialInterface> DynamicProgressionBombOverlayMaterialInternal = nullptr;

	/** The Progression difficulty multiplier. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BlueprintProtected, DisplayName = "Progression Multiplier Curve Table"))
	TObjectPtr<UCurveTable> ProgressionDifficultyMultiplierCurveTableInternal = nullptr;

	/** Star Material Slot name to change the dynamic fill-in based on the progression */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Material", meta = (BlueprintProtected, DisplayName = "Star Percent Slot Name"))
	FName StarPercentSlotNameInternal = NAME_None;

	/** When Instant character switch setting enabled fade animation will not be played */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "C++", AdvancedDisplay, meta = (BlueprintProtected, DisplayName = "Instant Character Switch Tag"))
	FSettingTag InstantCharacterSwitchTagInternal = FSettingTag::EmptySettingTag;

	/** Stores the duration of fade-in/fade-out overlay animation in the main menu when cinematic started */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "C++", meta = (BlueprintProtected, DisplayName = "Fade duration"))
	float FadeDurationInternal = 1.f;

	/** Interval which used of amount stars required to unlock a skin
	 * Can not be 0 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "C++", meta = (BlueprintProtected, DisplayName = "Skin Unlock Interval", ClampMin = "0"))
	int32 SkinUnlockIntervalInternal = 1;
};
