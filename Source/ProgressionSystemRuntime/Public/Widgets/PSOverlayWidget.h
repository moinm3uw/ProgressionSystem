// Copyright (c) Yevhenii Selivanov

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/PSTypes.h"
#include "PSOverlayWidget.generated.h"

/**
 * 
 * Overlay widget which is displayed for the locked/unlocked levels in the main menu
 * If level is locked overlay is displayed. Is unlocked - no overlay 
 */
UCLASS()
class PROGRESSIONSYSTEMRUNTIME_API UPSOverlayWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/**
	* Sets the visibility of the overlay elements and playing fade animation if needed 
	* @param VisibilitySlate The visibility state (e.g., Visible, Collapsed) to apply to the overlay and icon.
	* @param bShouldPlayFadeAnimation Defines if the fade animation should be played after or before widget is visible
	*/
	UFUNCTION(BlueprintCallable, Category= "C++")
	void SetOverlayVisibility(ESlateVisibility VisibilitySlate, bool bShouldPlayFadeAnimation = false);

protected:
	/** overrides NativeTick to make the user widget tickable **/
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	/** Event to execute when widget is ready */
	virtual void NativeConstruct() override;

	/**
	* Play the overlay elements fade-in/fade-out animation.
	* Uses the internal FadeCurveFloatInternal initialized in NativeConstruct
	*/
	UFUNCTION(BlueprintCallable, Category="C++", meta=(BlueprintProtected))
	void TickPlayFadeOverlayAnimation();

	/**
	* Sets the visibility of the background overlay and lock icon.
	* @param VisibilitySlate The visibility state (e.g., Visible, Collapsed) to apply to the overlay and icon.
	*/
	UFUNCTION(BlueprintCallable, Category= "C++", meta = (BlueprintProtected))
	void SetOverlayItemsVisibility(ESlateVisibility VisibilitySlate);

	/** Overlay widget which is a root for all fade-in/out overlay elements */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Transient, Category = "C++", meta = (BlueprintProtected, BindWidget))
	TObjectPtr<class UOverlay> PSCOverlay = nullptr;

	/** Stores the starting time to fade-in/fade-out overlay in the main menu when cinematic started */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Transient, Category = "C++", meta = (BlueprintProtected, DisplayName = "Starting time to fade overlay"))
	float StartTimeFadeAnimationInternal = 0.0f;

	/** if the fade-in/fade-out overlay animation in the main menu when cinematic started should be played */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Transient, Category = "C++", meta = (BlueprintProtected, DisplayName = "Should fade animation to be played"))
	bool bShouldPlayFadeAnimationInternal = false;

	/** Current overlay widget fade state. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Transient, AdvancedDisplay, Category = "C++", meta = (BlueprintProtected, DisplayName = "Overlay Widget Fade State"))
	EPSOverlayWidgetFadeState OverlayWidgetFadeStateInternal = EPSOverlayWidgetFadeState::None;
};
