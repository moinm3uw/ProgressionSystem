// Copyright (c) Yevhenii Selivanov


#include "Widgets/PSOverlayWidget.h"

#include "Components/Image.h"
#include "Curves/CurveFloat.h"
#include "Data/PSDataAsset.h"
#include "Components/Overlay.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PSOverlayWidget)

// Sets the visibility of the overlay elements and playing fade animation if needed
void UPSOverlayWidget::SetOverlayVisibility(ESlateVisibility VisibilitySlate, bool bShouldPlayFadeAnimation/* = false*/)
{
	if (!bShouldPlayFadeAnimation)
	{
		if (PSCOverlay)
		{
			// reset render opacity
			PSCOverlay->SetRenderOpacity(1.0f);
		}
		SetOverlayItemsVisibility(VisibilitySlate);
		return;
	}

	bShouldPlayFadeAnimationInternal = bShouldPlayFadeAnimation;

	if (VisibilitySlate == ESlateVisibility::Visible)
	{
		if (!ensureMsgf(PSCOverlay, TEXT("ASSERT: [%i] %s:\n'PSCOverlay' is not valid!"), __LINE__, *FString(__FUNCTION__)))
		{
			return;
		}
		ESlateVisibility PrevOverlayOpacity = PSCOverlay->GetVisibility();

		//if new visibility is same as previous animation is not required 
		if (PrevOverlayOpacity == VisibilitySlate)
		{
			bShouldPlayFadeAnimationInternal = false;
			OverlayWidgetFadeStateInternal = EPSOverlayWidgetFadeState::None;
		}
		OverlayWidgetFadeStateInternal = EPSOverlayWidgetFadeState::FadeIn;
		SetOverlayItemsVisibility(VisibilitySlate);
	}
	else
	{
		OverlayWidgetFadeStateInternal = EPSOverlayWidgetFadeState::FadeOut;
	}

	const UWorld* World = GetWorld();
	if (!ensureMsgf(World, TEXT("ASSERT: [%i] %s:\n'World' is not valid!"), __LINE__, *FString(__FUNCTION__)))
	{
		return;
	}

	StartTimeFadeAnimationInternal = World->GetTimeSeconds();
}

// overrides NativeTick to make the user widget tickable
void UPSOverlayWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	TickPlayFadeOverlayAnimation();
}

// Event to execute when widget is ready
void UPSOverlayWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

// Play the overlay elements fade-in/fade-out animation. Uses the internal FadeCurveFloatInternal initialized in NativeConstruct
void UPSOverlayWidget::TickPlayFadeOverlayAnimation()
{
	const UWorld* World = GetWorld();
	const float FadeDuration = UPSDataAsset::Get().GetOverlayFadeDuration();

	if (!bShouldPlayFadeAnimationInternal
		|| !World
		|| !ensureMsgf(FadeDuration > 0.0f, TEXT("ASSERT: [%i] %hs:\n'FadeDuration' must be greater than 0"), __LINE__, __FUNCTION__))
	{
		return;
	}

	const bool bIsFadeOutAnimation = OverlayWidgetFadeStateInternal == EPSOverlayWidgetFadeState::FadeOut;
	const float SecondsSinceStart = GetWorld()->GetTimeSeconds() - StartTimeFadeAnimationInternal;
	const float NormalizedTime = FMath::Clamp(SecondsSinceStart / FadeDuration, 0.0f, 1.0f);
	const float OpacityValue = bIsFadeOutAnimation ? 1.0f - NormalizedTime : NormalizedTime;

	if (SecondsSinceStart >= FadeDuration)
	{
		bShouldPlayFadeAnimationInternal = false;
		if (OverlayWidgetFadeStateInternal == EPSOverlayWidgetFadeState::FadeOut)
		{
			PSCOverlay->SetVisibility(ESlateVisibility::Collapsed);
		}
		return;
	}

	if (PSCOverlay)
	{
		PSCOverlay->SetRenderOpacity(OpacityValue);
	}
}

void UPSOverlayWidget::SetOverlayItemsVisibility(ESlateVisibility VisibilitySlate)
{
	// Level is unlocked hide the blocking overlay
	PSCOverlay->SetVisibility(VisibilitySlate);
}
