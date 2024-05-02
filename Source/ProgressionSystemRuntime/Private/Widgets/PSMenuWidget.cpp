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

	// Iterate it by handles to cancel spawning even if the widget is not spawned yet
	for (const FPoolObjectHandle& Handle : PoolWidgetHandlers)
	{
		if (!ensureMsgf(Handle.IsValid(), TEXT("ASSERT: [%i] %s:\n'PoolObjectHandle' is not valid!"), __LINE__, *FString(__FUNCTION__)))
		{
			return;
		}

		// Map component was not found, it could be not spawned, but in spawn request in queue
		UPoolManagerSubsystem::Get().ReturnToPool(Handle);
	}
	
	PoolWidgetHandlers.Empty();
	checkf(PoolWidgetHandlers.IsEmpty(), TEXT("ERROR: [%i] %s:\n'PoolWidgetHandlers' is not empty after removing all!"), __LINE__, *FString(__FUNCTION__));

	TArray<FSpawnRequest> InOutRequests;
	InOutRequests.Empty();
	checkf(InOutRequests.IsEmpty(), TEXT("ERROR: [%i] %s:\n'InOutRequests' is not empty after removing all!"), __LINE__, *FString(__FUNCTION__));

	float TotalRequests = AmountOfLockedPoints + AmountOfUnlockedPoints;
	// Loop to create and add images to the Horizontal Box for unlocked stars
	for (float i = 0; i < TotalRequests; i++)
	{
		FSpawnRequest& NewRequestRef = InOutRequests.AddDefaulted_GetRef();
		TSubclassOf<UPSStarWidget> StarWidgetClass = UPSDataAsset::Get().GetStarWidget(); 
		NewRequestRef.Class = StarWidgetClass;
	}

	// --- Prepare spawn request
	const TWeakObjectPtr<ThisClass> WeakThis = this;
	const FOnSpawnAllCallback OnTakeFromPoolCompleted = [WeakThis, AmountOfUnlockedPoints, AmountOfLockedPoints](const TArray<FPoolObjectData>& CreatedObjects)
	{
		UPSMenuWidget* This = WeakThis.Get();
		if (This)
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

	if (!InOutRequests.IsEmpty())
	{
		// --- Spawn widgets
		UPoolManagerSubsystem::Get().TakeFromPool(InOutRequests, OnTakeFromPoolCompleted);
	}

	// --- Add handles if requested spawning, so they can be canceled if regenerate before spawning finished
	for (const FSpawnRequest& It : InOutRequests)
	{
		checkf(It.Handle.IsValid(), TEXT("ERROR: [%i] %s:\n'Handle' is not valid!"), __LINE__, *FString(__FUNCTION__));
		PoolWidgetHandlers.AddUnique(It.Handle);
	}
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
