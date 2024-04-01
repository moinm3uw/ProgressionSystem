// Copyright (c) Valerii Rotermel & Yevhenii Selivanov

#pragma once

#include "Blueprint/UserWidget.h"
#include "Structures/MapComponentsContainer.h"
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
	void AddImagesToHorizontalBox(int32 AmountOfUnlockedPoints, int32 AmountOfLockedPoints);

	/** Removes all images from horizontal box */
	UFUNCTION(BlueprintCallable, Category= "C++", meta = (BlueprintProtected, BindWidget))
	void ClearImagesFromHorizontalBox();

	/**
	* Sets the visibility of the background overlay and lock icon.
	* @param VisibilitySlate The visibility state (e.g., Visible, Collapsed) to apply to the overlay and icon.
	*/
	UFUNCTION(BlueprintCallable, Category= "C++", meta = (BlueprintProtected, BindWidget))
	void SetOverlayVisibility(ESlateVisibility VisibilitySlate);

	/*********************************************************************************************
	 * Protected functions
	 ********************************************************************************************* */
protected:
	// Horizontal Box widget for storing stars
	UPROPERTY(EditDefaultsOnly, Category = "C++", meta = (BlueprintProtected, BindWidget))
	TObjectPtr<class UHorizontalBox> HorizontalBox = nullptr;

	// Background overlay for tint effect
	UPROPERTY(EditDefaultsOnly, Category = "C++", meta = (BlueprintProtected, BindWidget))
	TObjectPtr<class UImage> PSCBackgroundOverlay = nullptr;

	// Background overlay lock icon
	UPROPERTY(EditDefaultsOnly, Category = "C++", meta = (BlueprintProtected, BindWidget))
	TObjectPtr<class UImage> PSCBackgroundIconLock = nullptr;

	/** Array of pool handlers which should be released */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Transient, Category = "C++", meta = (BlueprintProtected, DisplayName = "Progression Components"))
	TArray<FPoolObjectHandle> PoolWidgetHandlers;


	/** Called after the underlying slate widget is constructed.
	 * May be called multiple times due to adding and removing from the hierarchy. */
	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;
};
