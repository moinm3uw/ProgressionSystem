// Copyright (c) Yevhenii Selivanov

#pragma once

#include "CoreMinimal.h"
#include "Factories/PoolFactory_UObject.h"
#include "UPoolFactory_Widget.generated.h"

/**
 * 
 */
UCLASS()
class PROGRESSIONSYSTEMRUNTIME_API UUPoolFactory_Widget : public UPoolFactory_UObject
{
	GENERATED_BODY()

public:
	/** Is overridden to handle Actors-inherited classes. */
	virtual const UClass* GetObjectClass_Implementation() const override;

	/*********************************************************************************************
	 * Creation
	 ********************************************************************************************* */
public:
	/** Is overridden to spawn actors using its engine's Spawn Actor method. */
	virtual UObject* SpawnNow_Implementation(const FSpawnRequest& Request) override;

	/*********************************************************************************************
	 * Destruction
	 ********************************************************************************************* */
public:
	/*********************************************************************************************
	 * Pool
	 ********************************************************************************************* */
public:
	/** Is overridden to set transform to the actor before taking the object from its pool. */
	virtual void OnTakeFromPool_Implementation(UObject* Object, const FTransform& Transform) override;

	/** Is overridden to reset transform to the actor before returning the object to its pool. */
	virtual void OnReturnToPool_Implementation(UObject* Object) override;
};
