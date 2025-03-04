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
	// Sets default values for this component's properties
	UPSSpotComponent();

	/** Returns the Skeletal Mesh of the Bomber character. */
	UFUNCTION(BlueprintPure, Category = "C++")
	class UMySkeletalMeshComponent* GetMySkeletalMeshComponent() const;
	class UMySkeletalMeshComponent& GetMeshChecked() const;

	/** Changes the player spot depends on current level state  */
	UFUNCTION(BlueprintCallable, Category= "C++")
	void ChangeSpotVisibilityStatus(UMySkeletalMeshComponent* Mesh);

	/** Refresh Amount Of Unlocked skins for the character (level) */
	UFUNCTION(BlueprintCallable, Category= "C++")
	void RefreshAmountOfUnlockedSkins(bool bApplySkin);

protected:
	/** Called when progression module ready
	 * Once the save file is loaded it activates the functionality of this class */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "C++", meta = (BlueprintProtected))
	void OnInitialized();

	/** Once the save file is reset the spot component needs to reset skins
	* Before progression loaded, the game has all skins available by default.
	* But if Progression System plugin is enabled, we are changing the default state only when the first skin unlocked.
	* This should happen right after the once the progression spot loaded and the reset cheat activated */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "C++", meta = (BlueprintProtected))
	void OnReset();

	/** Listen game states to switch character skin. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "C++", meta = (BlueprintProtected))
	void OnGameStateChanged(ECurrentGameState CurrentGameState);
	
	// Called when the game starts
	virtual void BeginPlay() override;

	/** Clears all transient data created by this component. */
	virtual void OnUnregister() override;

	/** Updates the progression menu widget when player changed */
	UFUNCTION(BlueprintNativeEvent, Category= "C++", meta = (BlueprintProtected))
	void OnCurrentActiveSaveRowChanged(const FPlayerTag PlayerTag);

	/** Updates the progression unlocked skins when score changes */
	UFUNCTION(BlueprintNativeEvent, Category= "C++", meta = (BlueprintProtected))
	void OnCurrentScoreChanged(const FPSSaveToDiskData& CurrentSaveToDiskDataRow, const FPSRowData& CurrentProgressionSettingsRow);
};
