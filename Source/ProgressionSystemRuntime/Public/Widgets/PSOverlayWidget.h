// Copyright (c) Yevhenii Selivanov

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/PSTypes.h"
#include "PSOverlayWidget.generated.h"

/**
 * 
 * Overlay widget which is displayed for the locked/unlocked levels in the main menu
 * If level is locked overlay is displayed. Is unlocked - no overlay.
 * For transition between locked and unlocked widget has fade-in, fade-out or instant appearing effect
 */
UCLASS()
class PROGRESSIONSYSTEMRUNTIME_API UPSOverlayWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/**
	* Applies animation and requested animation style
	* @param NewAnimation defines animation to be applied: e.g. FadeIn, FadeOut
	* @param NewAnimationType Defines the type of animation to be played: Fade or Instant style
	*/
	UFUNCTION(BlueprintCallable, Category= "C++")
	void ApplyOverlayAnimation(EPSOverlayWidgetFadeAnimation NewAnimation, EPSOverlayWidgetFadeAnimationType NewAnimationType);

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

	/** When a character has been changed current active progression row also changes */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "C++", meta = (BlueprintProtected))
	void OnCurrentRowDataChanged(FPlayerTag PlayerTag);

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
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Transient, AdvancedDisplay, Category = "C++", meta = (BlueprintProtected, DisplayName = "Current Overlay Widget Fade State"))
	EPSOverlayWidgetFadeAnimationType CurrentAnimationStyleInternal = EPSOverlayWidgetFadeAnimationType::None;

	/** Current overlay widget fade type. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Transient, AdvancedDisplay, Category = "C++", meta = (BlueprintProtected, DisplayName = "Current Overlay Widget Fade Type"))
	EPSOverlayWidgetFadeAnimation CurrentAnimationInternal = EPSOverlayWidgetFadeAnimation::None;

	/** Show locked level ui overlay */
	UFUNCTION(BlueprintCallable, Category= "C++", meta = (BlueprintProtected))
	void DisplayLevelUIOverlay();
};
