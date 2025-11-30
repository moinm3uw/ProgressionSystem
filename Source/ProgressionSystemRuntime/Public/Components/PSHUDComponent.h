// Copyright (c) Valerii Rotermel & Yevhenii Selivanov

#pragma once

#include "Bomber.h"
#include "Components/ActorComponent.h"
#include "GameFramework/BmrPlayerState.h"
#include "Structures/BmrPlayerTag.h"
//---
#include "PSHUDComponent.generated.h"

/**
 * Implements the core logic on project about Progression System.
 */

UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROGRESSIONSYSTEMRUNTIME_API UPSHUDComponent final : public UActorComponent
{
	GENERATED_BODY()

public:
	/** Sets default values for this component's properties. */
	UPSHUDComponent();

	/** Returns the Progression End Game widget. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "C++")
	class UPSEndGameWidget* GetProgressionEndGameWidget() const;

	/** Returns the Progression Menu overlay widget. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "C++")
	class UPSOverlayWidget* GetProgressionMenuOverlayWidget() const;

	/*********************************************************************************************
	 * Protected functions
	 ********************************************************************************************* */
protected:
	/** Called when progression module ready
	 * Once the save file is loaded it activates the functionality of this class */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "C++", meta = (BlueprintProtected))
	void OnInitialized();

	/** Called when the game starts. */
	virtual void BeginPlay() override;

	/** Clears all transient data created by this component. */
	virtual void OnUnregister() override;

	/** Is called when local player character is ready to guarantee that they player controller is initialized for the Widget SubSystem */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "C++", meta = (BlueprintProtected))
	void OnLocalPawnReady(class ABmrPawn* Character, int32 CharacterID);
};
