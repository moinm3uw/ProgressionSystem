// Copyright (c) Yevhenii Selivanov

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "PSCWorldSubsystem.generated.h"

/**
 * 
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
	FORCEINLINE TObjectPtr<class UProgressionSystemComponent> GetProgressionSystemComponent() const { return ProgressionSystemComponentInternal; }

	UFUNCTION(BlueprintCallable, Category = "C++")
	void SetProgressionSystemComponent(UProgressionSystemComponent* MyProgressionSystemComponent);

protected:
	
	/** Progression System component reference*/
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UProgressionSystemComponent> ProgressionSystemComponentInternal = nullptr;
	
};
