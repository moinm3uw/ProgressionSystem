// Copyright (c) Valerii Rotermel & Yevhenii Selivanov

#pragma once

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
	const FORCEINLINE UDataTable* GetProgressionDataTable() const { return ProgressionDataTableInternal; }

	/** Returns a progression menu widget to be displayed in the main menu*/
	UFUNCTION(BlueprintPure, Category = "C++")
	FORCEINLINE TSubclassOf<class UPSMenuWidget> GetProgressionMenuWidget() const { return ProgressionMenuWidgetInternal; }

	/** Returns a locked progression icon reference */
	UFUNCTION(BlueprintPure, Category = "C++")
	FORCEINLINE UTexture2D* GetLockedProgressionIcon() const { return LockedProgressionIconInternal; }

	/** Returns a unlocked progression icon reference */
	UFUNCTION(BlueprintPure, Category = "C++")
	FORCEINLINE UTexture2D* GetUnlockedProgressionIcon() const { return UnlockedProgressionIconInternal; }

protected:
	/** The Progression Data Table that is responsible for progression configuration. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BlueprintProtected, DisplayName = "Progression Data Table", ShowOnlyInnerProperties))
	TObjectPtr<UDataTable> ProgressionDataTableInternal = nullptr;

	/** Main progression widget */
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UPSMenuWidget> ProgressionMenuWidgetInternal = nullptr;

	/** Image for locked progression */
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UTexture2D> LockedProgressionIconInternal = nullptr;

	/** Image for unlocked progression */
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UTexture2D> UnlockedProgressionIconInternal = nullptr;
};
