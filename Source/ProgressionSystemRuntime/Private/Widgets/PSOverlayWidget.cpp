// Copyright (c) Yevhenii Selivanov


#include "Widgets/PSOverlayWidget.h"

#include "Components/Image.h"
#include "Components/MySkeletalMeshComponent.h"
#include "Curves/CurveFloat.h"
#include "Data/PSDataAsset.h"
#include "Components/Overlay.h"
#include "Components/PSSpotComponent.h"
#include "Data/PSWorldSubsystem.h"
#include "UI/SettingsWidget.h"
#include "UtilityLibraries/MyBlueprintFunctionLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PSOverlayWidget)

// Sets the visibility of the overlay elements and playing fade animation if needed
// Example: FadeIn, Active
// void UPSOverlayWidget::SetOverlayVisibility(EPSOverlayWidgetFadeStateDirection NewState)
void UPSOverlayWidget::SetOverlayVisibility(EPSOverlayWidgetFadeAnimationType NewFadeAnimationType, EPSOverlayWidgetFadeAnimationState NewFadeAnimationState)
{
	if (NewFadeAnimationState == EPSOverlayWidgetFadeAnimationState::Finished)
	{
		const ESlateVisibility desiredWidgetVisibility = NewFadeAnimationType == EPSOverlayWidgetFadeAnimationType::FadeIn ? ESlateVisibility::Visible : ESlateVisibility::Collapsed;
		const float desiredOpacity = NewFadeAnimationType == EPSOverlayWidgetFadeAnimationType::FadeIn ? 1.0f : 0.0f;
		PSCOverlay->SetRenderOpacity(desiredOpacity);
		StartTimeFadeAnimationInternal = 0.0f;
		SetVisibility(desiredWidgetVisibility);
		CurrentOverlayWidgetFadeAnimationStateInternal = NewFadeAnimationState;
		return;
	}

	if (NewFadeAnimationType == EPSOverlayWidgetFadeAnimationType::FadeIn && CurrentOverlayWidgetFadeAnimationTypeInternal == EPSOverlayWidgetFadeAnimationType::FadeIn)
	{
		return;
	}

	if (NewFadeAnimationType == EPSOverlayWidgetFadeAnimationType::FadeIn)
	{
		SetVisibility(ESlateVisibility::Visible);
	}

	CurrentOverlayWidgetFadeAnimationTypeInternal = NewFadeAnimationType;
	CurrentOverlayWidgetFadeAnimationStateInternal = NewFadeAnimationState;

	const UWorld* World = GetWorld();
	if (!ensureMsgf(World, TEXT("ASSERT: [%i] %hs:\n'World' is not valid!"), __LINE__, __FUNCTION__))
	{
		return;
	}

	StartTimeFadeAnimationInternal = World->GetTimeSeconds();
}

// overrides NativeTick to make the user widget tickable
void UPSOverlayWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (CurrentOverlayWidgetFadeAnimationStateInternal == EPSOverlayWidgetFadeAnimationState::None
		|| CurrentOverlayWidgetFadeAnimationStateInternal == EPSOverlayWidgetFadeAnimationState::Finished)
	{
		return;
	}
	TickPlayFadeOverlayAnimation();
}

// Event to execute when widget is ready
void UPSOverlayWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SetVisibility(ESlateVisibility::Collapsed);

	// Subscribe to the event notifying changes in player type
	UPSWorldSubsystem::Get().OnCurrentActiveSaveRowChanged.AddDynamic(this, &ThisClass::OnCurrentRowDataChanged);
}

// Play the overlay elements fade-in/fade-out animation. Uses the internal FadeCurveFloatInternal initialized in NativeConstruct
void UPSOverlayWidget::TickPlayFadeOverlayAnimation()
{
	const UWorld* World = GetWorld();
	const float FadeDuration = UPSDataAsset::Get().GetOverlayFadeDuration();

	if (!World
		|| !ensureMsgf(FadeDuration > 0.0f, TEXT("ASSERT: [%i] %hs:\n'FadeDuration' must be greater than 0"), __LINE__, __FUNCTION__))
	{
		return;
	}

	const bool bIsFadeOutAnimation = CurrentOverlayWidgetFadeAnimationTypeInternal == EPSOverlayWidgetFadeAnimationType::FadeOut;
	const float SecondsSinceStart = GetWorld()->GetTimeSeconds() - StartTimeFadeAnimationInternal;
	const float NormalizedTime = FMath::Clamp(SecondsSinceStart / FadeDuration, 0.0f, 1.0f);
	const float OpacityValue = bIsFadeOutAnimation ? 1.0f - NormalizedTime : NormalizedTime;

	if (SecondsSinceStart >= FadeDuration)
	{
		SetOverlayVisibility(CurrentOverlayWidgetFadeAnimationTypeInternal, EPSOverlayWidgetFadeAnimationState::Finished);
		return;
	}

	if (PSCOverlay)
	{
		PSCOverlay->SetRenderOpacity(OpacityValue);
	}
}

// When a character has been changed current active progression row also changes
void UPSOverlayWidget::OnCurrentRowDataChanged_Implementation(FPlayerTag PlayerTag)
{
	DisplayLevelUIOverlay();
}

// Show or hide the LevelUIOverlay depends on the level lock state for current level
// by default overlay is always displayed 
void UPSOverlayWidget::DisplayLevelUIOverlay()
{
	const FPSSaveToDiskData& CurrenSaveToDiskDataRow = UPSWorldSubsystem::Get().GetCurrentSaveToDiskRowByName();
	const bool IsLevelLocked = CurrenSaveToDiskDataRow.IsLevelLocked;

	if (const USettingsWidget* SettingsWidget = UMyBlueprintFunctionLibrary::GetSettingsWidget())
	{
		const bool bShouldPlayFadeAnimation = !SettingsWidget->GetCheckboxValue(UPSDataAsset::Get().GetInstantCharacterSwitchTag());
		ESlateVisibility OverlayVisibility = IsLevelLocked ? ESlateVisibility::Visible : ESlateVisibility::Collapsed;

		if (!IsLevelLocked)
		{
			const UPSSpotComponent* CurrentSpot = UPSWorldSubsystem::Get().GetCurrentSpot();
			if (CurrentSpot)
			{
				const UMySkeletalMeshComponent& MeshComp = CurrentSpot->GetMeshChecked();
				const int32 CurrentSkinIndex = MeshComp.GetAppliedSkinIndex();
				const bool isCurrentSkinAvailable = MeshComp.IsSkinAvailable(CurrentSkinIndex);
				OverlayVisibility = isCurrentSkinAvailable ? ESlateVisibility::Collapsed : ESlateVisibility::Visible;
			}
		}

		EPSOverlayWidgetFadeAnimationType FadeAnimationType = OverlayVisibility == ESlateVisibility::Visible ? EPSOverlayWidgetFadeAnimationType::FadeIn : EPSOverlayWidgetFadeAnimationType::FadeOut;
		EPSOverlayWidgetFadeAnimationState FadeAnimationState = bShouldPlayFadeAnimation ? EPSOverlayWidgetFadeAnimationState::Active : EPSOverlayWidgetFadeAnimationState::Finished;
		SetOverlayVisibility(FadeAnimationType, FadeAnimationState);
	}
}
