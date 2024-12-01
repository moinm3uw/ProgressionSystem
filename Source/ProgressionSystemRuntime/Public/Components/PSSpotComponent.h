// Copyright (c) Valerii Rotermel & Yevhenii Selivanov

#pragma once

#include "Data/PSTypes.h"
#include "Components/ActorComponent.h"
#include "PSSpotComponent.generated.h"

/**
 * Represents a spot where a character can be selected in the Main Menu.
 * Is added dynamically to the My Skeletal Mesh actors on the level.
 */
UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROGRESSIONSYSTEMRUNTIME_API UPSSpotComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPSSpotComponent, UPSSpotComponent*, SpotComponent);

	// Sets default values for this component's properties
	UPSSpotComponent();

	/** Returns the Skeletal Mesh of the Bomber character. */
	UFUNCTION(BlueprintPure, Category = "C++")
	class UMySkeletalMeshComponent* GetMySkeletalMeshComponent() const;
	class UMySkeletalMeshComponent& GetMeshChecked() const;

	/** Returns a Player's Spot On level (skeletal mesh component) */
	UFUNCTION(BlueprintPure, Category = "C++")
	FORCEINLINE class UMySkeletalMeshComponent* GetPlayerSpotOnLevel() const { return PlayerSpotOnLevelInternal; }

	/* Delegate for informing about loading spot component */
	UPROPERTY(BlueprintAssignable, Transient, Category = "C++")
	FPSSpotComponent OnSpotComponentReady;

protected:
	/** Called when progression module ready
	 * Once the save file is loaded it activates the functionality of this class */
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable, Category = "C++", meta = (BlueprintProtected))
	void OnInitialized();
	
	// Called when the game starts
	virtual void BeginPlay() override;

	/** Clears all transient data created by this component. */
	virtual void OnUnregister() override;

	/** Is called when a player has been changed */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "C++", meta = (BlueprintProtected))
	void OnPlayerTypeChanged(FPlayerTag PlayerTag);

	/** Is called when a player has been changed */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "C++", meta = (BlueprintProtected))
	void OnCharacterReady(class APlayerCharacter* PlayerCharacter, int32 CharacterID);

	/** Locks the player spot when progression for level achieved */
	UFUNCTION(BlueprintCallable, Category= "C++", meta=(BlueprintProtected))
	void ChangeSpotVisibilityStatus();

	/** A player skeletal mesh actor */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Transient, Category = "C++", meta = (BlueprintProtected, DisplayName = "Player Spot On Level"))
	TObjectPtr<UMySkeletalMeshComponent> PlayerSpotOnLevelInternal = nullptr;
};
