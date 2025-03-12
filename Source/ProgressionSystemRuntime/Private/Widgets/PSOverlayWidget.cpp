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
void UPSOverlayWidget::SetOverlayVisibility(EPSOverlayWidgetFadeState NewState)
{
	//if new visibility is same as previous animation is not required
	const bool bIsCurrentFadeInFinished = CurrentOverlayWidgetFadeStateInternal == EPSOverlayWidgetFadeState::FadeInFinished
		&& NewState == EPSOverlayWidgetFadeState::FadeInActive;

	const bool bIsCurrentFadeOutFinished = CurrentOverlayWidgetFadeStateInternal == EPSOverlayWidgetFadeState::FadeOutFinished
		&& NewState == EPSOverlayWidgetFadeState::FadeOutActive;

	if (NewState == CurrentOverlayWidgetFadeStateInternal
		|| bIsCurrentFadeInFinished || bIsCurrentFadeOutFinished)
	{
		return;
	}

	CurrentOverlayWidgetFadeStateInternal = NewState;

	if (NewState == EPSOverlayWidgetFadeState::FadeInFinished || NewState == EPSOverlayWidgetFadeState::FadeOutFinished)
	{
		if (!ensureMsgf(PSCOverlay, TEXT("ASSERT: [%i] %hs:\n'PSCOverlay' is not valid!"), __LINE__, __FUNCTION__))
		{
			return;
		}
		const ESlateVisibility desiredWidgetVisibility = NewState == EPSOverlayWidgetFadeState::FadeInFinished ? ESlateVisibility::Visible : ESlateVisibility::Collapsed;
		const float desiredOpacity = NewState == EPSOverlayWidgetFadeState::FadeInFinished ? 1.0f : 0.0f;
		PSCOverlay->SetRenderOpacity(desiredOpacity);
		StartTimeFadeAnimationInternal = 0.0f;
		SetVisibility(desiredWidgetVisibility);
		return;
	}


	if (NewState == EPSOverlayWidgetFadeState::FadeInActive)
	{
		SetVisibility(ESlateVisibility::Visible);
	}

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
	if (CurrentOverlayWidgetFadeStateInternal == EPSOverlayWidgetFadeState::None
		|| CurrentOverlayWidgetFadeStateInternal == EPSOverlayWidgetFadeState::FadeInFinished
		|| CurrentOverlayWidgetFadeStateInternal == EPSOverlayWidgetFadeState::FadeOutFinished)
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

	const bool bIsFadeOutAnimation = CurrentOverlayWidgetFadeStateInternal == EPSOverlayWidgetFadeState::FadeOutActive;
	const float SecondsSinceStart = GetWorld()->GetTimeSeconds() - StartTimeFadeAnimationInternal;
	const float NormalizedTime = FMath::Clamp(SecondsSinceStart / FadeDuration, 0.0f, 1.0f);
	const float OpacityValue = bIsFadeOutAnimation ? 1.0f - NormalizedTime : NormalizedTime;

	if (SecondsSinceStart >= FadeDuration)
	{
		auto FadeOutActive = CurrentOverlayWidgetFadeStateInternal == EPSOverlayWidgetFadeState::FadeOutActive;
		EPSOverlayWidgetFadeState newState = FadeOutActive ? EPSOverlayWidgetFadeState::FadeOutFinished : EPSOverlayWidgetFadeState::FadeInFinished;
		SetOverlayVisibility(newState);
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

		EPSOverlayWidgetFadeState RequestSate = OverlayVisibility == ESlateVisibility::Visible ? EPSOverlayWidgetFadeState::FadeInActive : EPSOverlayWidgetFadeState::FadeOutActive;

		if (!bShouldPlayFadeAnimation)
		{
			auto FadeOutActive = RequestSate == EPSOverlayWidgetFadeState::FadeOutActive;
			RequestSate = FadeOutActive ? EPSOverlayWidgetFadeState::FadeOutFinished : EPSOverlayWidgetFadeState::FadeInFinished;
		}

		// request 2 states: type and direction. 
		SetOverlayVisibility(RequestSate);
	}
}
