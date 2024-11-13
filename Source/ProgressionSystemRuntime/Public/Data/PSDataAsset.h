// Copyright (c) Valerii Rotermel & Yevhenii Selivanov

#pragma once
#include "Engine/DataAsset.h"
#include "Data/SettingTag.h"
#include "PSDataAsset.generated.h"

/**
 * Contains all progression assets used in the module 
 */
UCLASS()
class PROGRESSIONSYSTEMRUNTIME_API UPSDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	/** Returns the settings data asset. */
	static const UPSDataAsset& Get();

	/** Returns the Progression Data Table
	 * @see UProgressionSystemDataAsset::ProgressionDataTableInternal */
	UFUNCTION(BlueprintPure, Category = "C++")
	const FORCEINLINE class UDataTable* GetProgressionDataTable() const { return ProgressionDataTableInternal; }

	/** Returns a progression menu widget to be displayed in the main menu*/
	UFUNCTION(BlueprintPure, Category = "C++")
	FORCEINLINE TSubclassOf<class UPSMenuWidget> GetProgressionMenuWidget() const { return ProgressionMenuWidgetInternal; }

	/** Returns a progression overlay widget to be displayed in the main menu for locked levels */
	UFUNCTION(BlueprintPure, Category = "C++")
	FORCEINLINE TSubclassOf<class UPSOverlayWidget> GetProgressionOverlayWidget() const { return ProgressionOverlayWidgetInternal; }

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

	/** Returns Material applied for locked progression material (star is empty) */
	UFUNCTION(BlueprintPure, Category = "C++")
	FORCEINLINE class UMaterialInterface* GetLockedProgressionMaterial() const { return LockedProgressionMaterialInternal; }

	/** Returns Material applied for unlocked progression material (star is fully filled) */
	UFUNCTION(BlueprintPure, Category = "C++")
	FORCEINLINE class UMaterialInterface* GetUnlockedProgressionMaterial() const { return UnlockedProgressionMaterialInternal; }

	/** Returns Material applied for dynamic progression material (star is filled partially depends on progression) */
	UFUNCTION(BlueprintPure, Category = "C++")
	FORCEINLINE class UMaterialInterface* GetDynamicProgressionMaterial() const { return DynamicProgressionMaterialInternal; }

	/** Returns progression difficulty multiplier */
	UFUNCTION(BlueprintPure, Category = "C++")
	FORCEINLINE TMap<EGameDifficulty, float> GetProgressionDifficultyMultiplier() const { return ProgressionDifficultyMultiplierInternal; }

	/** Returns the duration of fade-in/fade-out overlay animation in the main menu when cinematic started */
	UFUNCTION(BlueprintPure, Category = "C++")
	FORCEINLINE float GetOverlayFadeDuration() const { return FadeDurationInternal; }

	/** Star Material Slot name to change the dynamic fill-in based on the progression */
	UFUNCTION(BlueprintPure, Category = "C++")
	FORCEINLINE FName GetStarMaterialSlotName() const { return StarMaterialSlotNameInternal; }

	/** Returns temp value to tweak the stars with bad UV  to look as expected. Could not be 0 */
	UFUNCTION(BlueprintPure, Category = "C++")
	FORCEINLINE float GetStarMaterialFractionalDivisor() const { return StarMaterialFractionalDivisorInternal; }

	/** Returns Instant Character Switch Tag. When Instant character switch setting enabled fade animation will not be played */
	UFUNCTION(BlueprintPure, Category = "C++")
	FORCEINLINE FSettingTag GetInstantCharacterSwitchTag() const { return InstantCharacterSwitchTagInternal; }
protected:
	/** The Progression Data Table that is responsible for progression configuration. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BlueprintProtected, DisplayName = "Progression Data Table", ShowOnlyInnerProperties))
	TObjectPtr<UDataTable> ProgressionDataTableInternal = nullptr;

	/** Main progression widget */
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UPSMenuWidget> ProgressionMenuWidgetInternal = nullptr;

	/** Main Menu overlay widget */
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UPSOverlayWidget> ProgressionOverlayWidgetInternal = nullptr;

	/** Star icon widget */
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UPSStarWidget> StarWidgetInternal = nullptr;

	/** Star icon widget */
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AActor> StarActorClassInternal = nullptr;

	/** Image for locked progression */
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UTexture2D> LockedProgressionIconInternal = nullptr;

	/** Image for unlocked progression */
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UTexture2D> UnlockedProgressionIconInternal = nullptr;

	/** Material applied for locked progression material (star is empty) */
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UMaterialInterface> LockedProgressionMaterialInternal = nullptr;

	/** Material applied for unlocked progression material (star is fully filled) */
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UMaterialInterface> UnlockedProgressionMaterialInternal = nullptr;

	/** Material applied for dynamic progression material (star is filled partially depends on progression) */
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UMaterialInterface> DynamicProgressionMaterialInternal = nullptr;

	/** The Progression difficulty multiplier. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BlueprintProtected, DisplayName = "Progression Multiplier", ShowOnlyInnerProperties))
	TMap<EGameDifficulty, float> ProgressionDifficultyMultiplierInternal;

	/** Stores the duration of fade-in/fade-out overlay animation in the main menu when cinematic started */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "C++", meta = (BlueprintProtected, DisplayName = "Fade duration"))
	float FadeDurationInternal = 1.0;

	/** Star Material Slot name to change the dynamic fill-in based on the progression */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "C++", meta = (BlueprintProtected, DisplayName = "Star Material Slot Name"))
	FName StarMaterialSlotNameInternal = NAME_None;

	/** Temporary used to tweak the stars with bad UV  to look as expected
	 * Since it's a divisor couldn't be 0 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "C++", meta = (BlueprintProtected, DisplayName = "Star Material Fractional Divisor Temporary"))
	float StarMaterialFractionalDivisorInternal = 1.f;

	/** When Instant character switch setting enabled fade animation will not be played */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "C++", AdvancedDisplay, meta = (BlueprintProtected, DisplayName = "Instant Character Switch Tag"))
	FSettingTag InstantCharacterSwitchTagInternal = FSettingTag::EmptySettingTag;
};
