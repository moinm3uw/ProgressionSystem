// Copyright (c) Valerii Rotermel & Yevhenii Selivanov

#pragma once

#include "Blueprint/UserWidget.h"
#include "PoolManagerTypes.h"
#include "PSMenuWidget.generated.h"

/**
 * Widget to display the progression as stars in the main menu.
 */
UCLASS()
class PROGRESSIONSYSTEMRUNTIME_API UPSMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/**
	 * Dynamically populates a Horizontal Box with images representing unlocked and locked progression icons.
	 * @param AmountOfUnlockedPoints The number of images (unlocked-icon as images) to be displayed 
	 * @param AmountOfLockedPoints The number of images (locked-icon as images) to be displayed
	 */
	UFUNCTION(BlueprintCallable, Category= "C++")
	void AddImagesToHorizontalBox(float AmountOfUnlockedPoints, float AmountOfLockedPoints, float MaxLevelPoints);

	/*********************************************************************************************
	 * Protected functions
	 ********************************************************************************************* */
protected:
	// Horizontal Box widget for storing stars
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Transient, Category = "C++", meta = (BlueprintProtected, BindWidget))
	TObjectPtr<class UHorizontalBox> HorizontalBox = nullptr;

	/** Array of pool handlers which should be released */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Transient, Category = "C++", meta = (BlueprintProtected, DisplayName = "Pool Widget Handlers"))
	TArray<FPoolObjectHandle> PoolWidgetHandlersInternal;
	
	/** Called after the underlying slate widget is constructed.
	 * May be called multiple times due to adding and removing from the hierarchy. */
	virtual void NativeConstruct() override;

	/** Called when the end game state was changed to toggle progression widget visibility. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "C++", meta = (BlueprintProtected))
	void OnGameStateChanged(ECurrentGameState CurrentGameState);

	/** Subscribes to the end game state change notification on the player state. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "C++", meta = (BlueprintProtected))
	void OnLocalPlayerStateReady(AMyPlayerState* PlayerState, int32 CharacterID);

	/** Called when the end game state was changed. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "C++", meta = (BlueprintProtected))
	void OnEndGameStateChanged(EEndGameState EndGameState);

	/**
	 * Dynamically populates a Horizontal Box with images representing unlocked and locked progression icons.
	* @param CreatedObjects - Handles of objects from Pool Manager
	* @param AmountOfUnlockedPoints The number of images (unlocked-icon as images) to be displayed 
	 * @param AmountOfLockedPoints The number of images (locked-icon as images) to be displayed
	 */
	UFUNCTION(BlueprintCallable, Category= "C++")
	void OnTakeFromPoolCompleted(const TArray<FPoolObjectData>& CreatedObjects, float AmountOfUnlockedPoints, float AmountOfLockedPoints, float MaxLevelPoints);
	
	/** Updates star images icon to locked/unlocked according to input amounnt
	 * @param CreatedData Object received from Pool Manager which contains the reference to Start Widget 
	 * @param AmountOfUnlockedStars Amount of icons to be switched to Unlocked stars.
	 * @param AmountOfLockedStars Amount of icons to be switched to Locked stars.
	 */
	UFUNCTION(BlueprintCallable, Category= "C++", meta = (BlueprintProtected))
	void UpdateStarImages(const FPoolObjectData& CreatedData, float AmountOfUnlockedStars, float AmountOfLockedStars);

	/** Updates Progress bar icon for unlocked icons
	 * @param CreatedData Object received from Pool Manager which contains the reference to Start Widget 
	 * @param NewProgressBarValue percentage completion
	 */
	UFUNCTION(BlueprintCallable, Category= "C++", meta = (BlueprintProtected))
	void UpdateStarProgressBarValue(const FPoolObjectData& CreatedData, float NewProgressBarValue);
};
