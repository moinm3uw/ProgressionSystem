// Copyright (c) Valerii Rotermel & Yevhenii Selivanov

#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "PSWorldSubsystem.generated.h"

/**
 * Implements the world subsystem to access different components in the module 
 */
UCLASS()
class PROGRESSIONSYSTEMRUNTIME_API UPSWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	/** Returns this Subsystem, is checked and wil crash if can't be obtained.*/
	static UPSWorldSubsystem& Get();
	static UPSWorldSubsystem& Get(const UObject& WorldContextObject);

	/** Returns a progression System component reference */
	UFUNCTION(BlueprintPure, Category = "C++")
	FORCEINLINE class UPSHUDComponent* GetProgressionSystemComponent() const { return ProgressionSystemComponentInternal;	}

	UFUNCTION(BlueprintCallable, Category = "C++")
	void SetProgressionSystemComponent(UPSHUDComponent* MyProgressionSystemComponent);

protected:
	/** Progression System component reference*/
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Transient, Category = "C++", meta = (BlueprintProtected, DisplayName = "Progression System Component"))
	TObjectPtr<class UPSHUDComponent> ProgressionSystemComponentInternal = nullptr;
};
