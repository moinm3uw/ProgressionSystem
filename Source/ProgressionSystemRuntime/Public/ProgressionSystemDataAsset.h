// Copyright (c) Yevhenii Selivanov

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/DataTable.h"
#include "Widgets/ProgressionMenuWidget.h"
#include "ProgressionSystemDataAsset.generated.h"

/**
 * Contains all progression assets used in the module 
 */
UCLASS()
class PROGRESSIONSYSTEMRUNTIME_API UProgressionSystemDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	/** Returns the settings data asset. */
	static const UProgressionSystemDataAsset& Get() { return *GetDefault<ThisClass>(); }

	/** Returns the Progression Data Table
	 * @see UProgressionSystemDataAsset::ProgressionDataTableInternal */
	UFUNCTION(BlueprintPure, Category = "C++")
	FORCEINLINE UDataTable* GetProgressionDataTable() const { return ProgressionDataTableInternal; }

	/** Returns a progression menu widget to be displayed in the main menu*/
	UFUNCTION(BlueprintPure, Category = "C++")
	FORCEINLINE TSubclassOf<class UProgressionMenuWidget> GetProgressionMenuWidget() const { return ProgressionMenuWidgetInternal; }

	/** Returns a progression save widget to be displayed in the end of game */
	UFUNCTION(BlueprintPure, Category = "C++")
	FORCEINLINE TSubclassOf<class UProgressionSaveWidget> GetProgressionSaveWidget() const { return ProgressionSaveWidgetInternal; }

	/** Returns a progression System component reference */
	FORCEINLINE TObjectPtr<class UProgressionSystemComponent> GetProgressionSystemComponent() const { return ProgressionSystemComponentInternal; }

	/** Returns a locked progression icon reference */
	FORCEINLINE TObjectPtr<class UTexture2D> GetLockedProgressionIcon() const { return LockedProgressionIconInternal; }

	/** Returns a unlocked progression icon reference */
	FORCEINLINE TObjectPtr<class UTexture2D> GetUnlockedProgressionIcon() const { return UnlockedProgressionIconInternal; }

protected:
	/** The Progression Data Table that is responsible for progression configuration. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BlueprintProtected, DisplayName = "Progression Data Table", ShowOnlyInnerProperties))
	TObjectPtr<UDataTable> ProgressionDataTableInternal = nullptr;

	/**  Progression acquiring (adding, saving) widget */
	UPROPERTY(EditAnywhere)
	TSubclassOf<UProgressionSaveWidget> ProgressionSaveWidgetInternal = nullptr;

	/** Main progression widget */
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UProgressionMenuWidget> ProgressionMenuWidgetInternal = nullptr;

	/** Progression System component reference*/
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UProgressionSystemComponent> ProgressionSystemComponentInternal = nullptr;

	/** Image for locked progression */
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UTexture2D> LockedProgressionIconInternal = nullptr;

	/** Image for unlocked progression */
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UTexture2D> UnlockedProgressionIconInternal = nullptr;
};
