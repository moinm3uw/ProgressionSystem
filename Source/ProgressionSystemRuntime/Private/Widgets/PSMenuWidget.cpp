// Copyright (c) Valerii Rotermel & Yevhenii Selivanov

#include "Widgets/PSMenuWidget.h"
//---
#include "Data/PSDataAsset.h"
#include "Blueprint/WidgetTree.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Image.h"
#include "Widgets/PSStarWidget.h"
//---

#include "PoolManagerSubsystem.h"
#include "PoolManagerTypes.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PSMenuWidget)

// Dynamically populates a Horizontal Box with images representing unlocked and locked progression icons.
void UPSMenuWidget::AddImagesToHorizontalBox(float AmountOfUnlockedPoints, float AmountOfLockedPoints)
{
	checkf(HorizontalBox, TEXT("ERROR: 'HorizontalBox' is null"));

	// Map component was not found, it could be not spawned, but in spawn request in queue
	UPoolManagerSubsystem::Get().ReturnToPoolArray(PoolWidgetHandlers);
	
	PoolWidgetHandlers.Empty();
	checkf(PoolWidgetHandlers.IsEmpty(), TEXT("ERROR: [%i] %s:\n'PoolWidgetHandlers' is not empty after removing all!"), __LINE__, *FString(__FUNCTION__));
	
	// --- Prepare spawn request
	const TWeakObjectPtr<ThisClass> WeakThis = this;
	const FOnSpawnAllCallback OnTakeFromPoolCompleted = [WeakThis, AmountOfUnlockedPoints, AmountOfLockedPoints](const TArray<FPoolObjectData>& CreatedObjects)
	{
		if (UPSMenuWidget* This = WeakThis.Get())
		{
			This->HorizontalBox->ClearChildren();
			int32 CurrentAmountOfUnlocked = AmountOfUnlockedPoints;
			int32 CurrentAmountOfLocked = AmountOfLockedPoints;
			// Setup spawned widget
			for (const FPoolObjectData& CreatedObject : CreatedObjects)
			{
				if (CurrentAmountOfUnlocked > 0)
				{
					// #1 Create MyFunction
					This->UpdateStarImages(CreatedObject, 1, 0);
					CurrentAmountOfUnlocked--;
					continue;
				}

				if (CurrentAmountOfLocked > 0)
				{
					This->UpdateStarImages(CreatedObject, 0, 1);
					CurrentAmountOfLocked--;
				}
			}
		}
	};
	
	// --- Spawn widgets
	const int32 TotalRequests = AmountOfLockedPoints + AmountOfUnlockedPoints;
	UPoolManagerSubsystem::Get().TakeFromPoolArray(PoolWidgetHandlers, UPSDataAsset::Get().GetStarWidget(), TotalRequests, OnTakeFromPoolCompleted);
}

void UPSMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Hide this widget by default
	SetVisibility(ESlateVisibility::Collapsed);
}

void UPSMenuWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

// Sets the visibility of the background overlay and lock icon.
void UPSMenuWidget::SetOverlayVisibility(ESlateVisibility VisibilitySlate)
{
	// Level is unlocked hide the blocking overlay
	PSCBackgroundOverlay->SetVisibility(VisibilitySlate);
	PSCBackgroundIconLock->SetVisibility(VisibilitySlate);
}
// Updates star images icon to locked/unlocked according to input amounnt
void UPSMenuWidget::UpdateStarImages(const FPoolObjectData& CreatedData, float AmountOfUnlockedStars, float AmountOfLockedStars)
{
	UPSStarWidget& SpawnedWidget = CreatedData.GetChecked<UPSStarWidget>();
	checkf(&SpawnedWidget, TEXT("ERROR: 'ProgressionMenuWidgetInternal' is null"));
	if (AmountOfUnlockedStars > 0)
	{
		SpawnedWidget.SetStarImage(UPSDataAsset::Get().GetUnlockedProgressionIcon());
	}

	if (AmountOfLockedStars > 0)
	{
		SpawnedWidget.SetStarImage(UPSDataAsset::Get().GetLockedProgressionIcon());
	}
	// Load and set the image texture here using ImagePath or other methods
	if (!HorizontalBox->HasChild(&SpawnedWidget))
	{
		HorizontalBox->AddChildToHorizontalBox(&SpawnedWidget);
	}
}
