// Copyright (c) Valerii Rotermel & Yevhenii Selivanov

#include "Widgets/PSMenuWidget.h"
//---
#include "Data/PSDataAsset.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "Widgets/PSStarWidget.h"
//---

#include "PoolManagerSubsystem.h"
#include "PoolManagerTypes.h"
#include "Components/StaticMeshComponent.h"
#include "UtilityLibraries/MyBlueprintFunctionLibrary.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(PSMenuWidget)

// Dynamically populates a Horizontal Box with images representing unlocked and locked progression icons.
void UPSMenuWidget::AddImagesToHorizontalBox(float AmountOfUnlockedPoints, float AmountOfLockedPoints, float MaxLevelPoints)
{
	//Return to Pool Manager the list of handles which is not needed (if there are any) 
	UPoolManagerSubsystem::Get().ReturnToPoolArray(PoolWidgetHandlersInternal);
	
	// --- Prepare spawn request
	const TWeakObjectPtr<ThisClass> WeakThis = this;
	const FOnSpawnAllCallback OnTakeFromPoolCompleted = [WeakThis, AmountOfUnlockedPoints, AmountOfLockedPoints, MaxLevelPoints](const TArray<FPoolObjectData>& CreatedObjects)
	{
		if (UPSMenuWidget* This = WeakThis.Get())
		{
			This->OnTakeFromPoolCompleted(CreatedObjects, AmountOfUnlockedPoints, AmountOfLockedPoints, MaxLevelPoints);
		}
	};
	
	// --- Spawn widgets
	const int32 TotalRequests = AmountOfLockedPoints + AmountOfUnlockedPoints;
	UPoolManagerSubsystem::Get().TakeFromPoolArray(PoolWidgetHandlersInternal, UPSDataAsset::Get().GetStarWidgetClass(), TotalRequests, OnTakeFromPoolCompleted);
}

// Dynamically populates a Horizontal Box with images representing unlocked and locked progression icons
void UPSMenuWidget::OnTakeFromPoolCompleted(const TArray<FPoolObjectData>& CreatedObjects, float AmountOfUnlockedPoints, float AmountOfLockedPoints, float MaxLevelPoints)
{
	checkf(HorizontalBox, TEXT("ERROR: 'HorizontalBox' is null"));
	HorizontalBox->ClearChildren();
	float CurrentAmountOfUnlocked = AmountOfUnlockedPoints;
	float CurrentAmountOfLocked = AmountOfLockedPoints;
	// Setup spawned widget
	for (const FPoolObjectData& CreatedObject : CreatedObjects)
	{
		if (CurrentAmountOfUnlocked > 0)
		{
			// #1 Create MyFunction
			UpdateStarImages(CreatedObject, 1, 0);
			// more than 0 means that it's not an integer
			if ((MaxLevelPoints - CurrentAmountOfUnlocked)  > 0)
			{
				UpdateStarProgressBarValue(CreatedObject, CurrentAmountOfUnlocked);
			}
			CurrentAmountOfUnlocked--;
			continue;
		}

		if (CurrentAmountOfLocked > 0)
		{
			UpdateStarImages(CreatedObject, 0, 1);
			CurrentAmountOfLocked--;
		}
	}
}

// Called after the underlying slate widget is constructed.
void UPSMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Hide this widget by default
	SetVisibility(ESlateVisibility::Collapsed);
}
// Updates star images icon to locked/unlocked according to input amounnt
void UPSMenuWidget::UpdateStarImages(const FPoolObjectData& CreatedData, float AmountOfUnlockedStars, float AmountOfLockedStars)
{
	UPSStarWidget& SpawnedWidget = CreatedData.GetChecked<UPSStarWidget>();
	
	if (AmountOfUnlockedStars > 0)
	{
		SpawnedWidget.SetStarImage(UPSDataAsset::Get().GetUnlockedProgressionIcon());
		SpawnedWidget.UpdateProgressionBarPercentage(AmountOfUnlockedStars);
	}

	if (AmountOfLockedStars > 0)
	{
		SpawnedWidget.SetStarImage(UPSDataAsset::Get().GetLockedProgressionIcon());
	}
	// Load and set the image texture here using ImagePath or other methods
	if (!HorizontalBox->HasChild(&SpawnedWidget))
	{
		HorizontalBox->AddChildToHorizontalBox(&SpawnedWidget);
		SpawnedWidget.UpdateProgressionBarPercentage(AmountOfUnlockedStars);
	}
}

// Updates Progress bar icon for unlocked icons 
void UPSMenuWidget::UpdateStarProgressBarValue(const FPoolObjectData& CreatedData, float NewProgressBarValue)
{
	UPSStarWidget& SpawnedWidget = CreatedData.GetChecked<UPSStarWidget>();
	SpawnedWidget.UpdateProgressionBarPercentage(NewProgressBarValue);
}
