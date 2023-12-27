// Copyright (c) Valerii Rotermel & Yevhenii Selivanov

#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "PSCWorldSubsystem.generated.h"

/**
 * Implements the world subsystem to access different components in the module 
 */
UCLASS()
class PROGRESSIONSYSTEMRUNTIME_API UPSCWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	/** Returns this Subsystem, is checked and wil crash if can't be obtained.*/
	static UPSCWorldSubsystem& Get();
	static UPSCWorldSubsystem& Get(const UObject& WorldContextObject);

	/** Returns a progression System component reference */
	UFUNCTION(BlueprintPure, Category = "C++")
	FORCEINLINE class UProgressionSystemComponent* GetProgressionSystemComponent() const { return ProgressionSystemComponentInternal;	}

	UFUNCTION(BlueprintCallable, Category = "C++")
	void SetProgressionSystemComponent(UProgressionSystemComponent* MyProgressionSystemComponent);

protected:
	/** Progression System component reference*/
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Transient, Category = "C++", meta = (BlueprintProtected, DisplayName = "Progression System Component"))
	TObjectPtr<class UProgressionSystemComponent> ProgressionSystemComponentInternal = nullptr;
};
