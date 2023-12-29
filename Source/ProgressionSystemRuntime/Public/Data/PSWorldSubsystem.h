// Copyright (c) Valerii Rotermel & Yevhenii Selivanov

#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "PSWorldSubsystem.generated.h"

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

	/** Returns the data asset that contains all the assets of Progression System game feature.
	 * @see UPSWorldSubsystem::PSDataAssetInternal. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "C++")
	const class UPSDataAsset* GetPSDataAsset() const;
	
	/** Returns a progression System component reference */
	UFUNCTION(BlueprintPure, Category = "C++")
	FORCEINLINE class UPSHUDComponent* GetProgressionSystemComponent() const { return ProgressionSystemComponentInternal;	}

	UFUNCTION(BlueprintCallable, Category = "C++")
	void SetProgressionSystemComponent(UPSHUDComponent* MyProgressionSystemComponent);

protected:
	/** Contains all the assets and tweaks of New Main Menu game feature.
	 * Note: Since Subsystem is code-only, there is config property set in BasePSDataAssetInternal.ini.
	 * Property is put to subsystem because its instance is created before any other object.
	 * It can't be put to DevelopSettings class because it does work properly for MGF-modules. */
	UPROPERTY(Config, VisibleInstanceOnly, BlueprintReadWrite, Category = "C++", meta = (BlueprintProtected, DisplayName = "Progression System Data Asset"))
	TSoftObjectPtr<const class UPSDataAsset> PSDataAssetInternal;
	
	/** Progression System component reference*/
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Transient, Category = "C++", meta = (BlueprintProtected, DisplayName = "Progression System Component"))
	TObjectPtr<class UPSHUDComponent> ProgressionSystemComponentInternal = nullptr;
};
