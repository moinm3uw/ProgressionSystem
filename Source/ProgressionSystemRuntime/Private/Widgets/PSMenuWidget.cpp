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
void UPSMenuWidget::AddImagesToHorizontalBox(int32 AmountOfUnlockedPoints, int32 AmountOfLockedPoints)
{
	checkf(HorizontalBox, TEXT("ERROR: 'HorizontalBox' is null"));

	// Iterate it by handles to cancel spawning even if the widget is not spawned yet
	for (const FPoolObjectHandle& Handle : PoolWidgetHandlers)
	{
		if (!ensureMsgf(Handle.IsValid(), TEXT("ASSERT: [%i] %s:\n'PoolObjectHandle' is not valid!"), __LINE__, *FString(__FUNCTION__)))
		{
			return;
		}

		FPoolObjectData ObjData = UPoolManagerSubsystem::Get().FindPoolObjectByHandle(Handle);
		if (!ensureMsgf(ObjData.IsValid(), TEXT("ASSERT: [%i] %s:\n'ObjData' is not valid!"), __LINE__, *FString(__FUNCTION__)))
		{
		}

		// Map component was not found, it could be not spawned, but in spawn request in queue
		UPoolManagerSubsystem::Get().ReturnToPool(Handle);
	}

	TArray<FSpawnRequest> InOutRequestsUnlockedProgression;
	TArray<FSpawnRequest> InOutRequestsLockedProgression;
	
	InOutRequestsUnlockedProgression.Empty();
	checkf(InOutRequestsUnlockedProgression.IsEmpty(), TEXT("ERROR: [%i] %s:\n'InOutRequestsUnlockedProgression' is not empty after removing all!"), __LINE__, *FString(__FUNCTION__));
	
	InOutRequestsLockedProgression.Empty();
	checkf(InOutRequestsLockedProgression.IsEmpty(), TEXT("ERROR: [%i] %s:\n'InOutRequestsLockedProgression' is not empty after removing all!"), __LINE__, *FString(__FUNCTION__));

	PoolWidgetHandlers.Empty();
	checkf(PoolWidgetHandlers.IsEmpty(), TEXT("ERROR: [%i] %s:\n'PoolWidgetHandlers' is not empty after removing all!"), __LINE__, *FString(__FUNCTION__));
	// Loop to create and add images to the Horizontal Box for unlocked stars
	for (int32 i = 0; i < AmountOfUnlockedPoints; i++)
	{
		FSpawnRequest& NewRequestRef = InOutRequestsUnlockedProgression.AddDefaulted_GetRef();
		NewRequestRef.Class = UPSDataAsset::Get().GetStarWidget();
	}

	// --- Prepare spawn request
	const FOnSpawnAllCallback OnCompletedUnlockedProgression = [this,AmountOfUnlockedPoints](const TArray<FPoolObjectData>& CreatedObjects)
	{
		// Setup spawned widget
		for (const FPoolObjectData& CreatedObject : CreatedObjects)
		{
			UPSStarWidget& SpawnedWidget = CreatedObject.GetChecked<UPSStarWidget>();
			checkf(&SpawnedWidget, TEXT("ERROR: 'ProgressionMenuWidgetInternal' is null"));

			SpawnedWidget.SetVisibility(ESlateVisibility::Visible);
			SpawnedWidget.SetStarImage(UPSDataAsset::Get().GetUnlockedProgressionIcon());

			// Load and set the image texture here using ImagePath or other methods
			if (!HorizontalBox->HasChild(&SpawnedWidget))
			{
				HorizontalBox->AddChildToHorizontalBox(&SpawnedWidget);
			}
		}
	};

	// --- Spawn widgets
	if (!InOutRequestsUnlockedProgression.IsEmpty())
	{
		UPoolManagerSubsystem::Get().TakeFromPool(InOutRequestsUnlockedProgression, OnCompletedUnlockedProgression);
	}

	// --- Add handles if requested spawning, so they can be canceled if regenerate before spawning finished
	for (const FSpawnRequest& It : InOutRequestsUnlockedProgression)
	{
		checkf(It.Handle.IsValid(), TEXT("ERROR: [%i] %s:\n'Handle' is not valid!"), __LINE__, *FString(__FUNCTION__));
		PoolWidgetHandlers.AddUnique(It.Handle);
	}

	// Loop to create and add images to the Horizontal Box for unlocked stars
	for (int32 i = 0; i < AmountOfLockedPoints; i++)
	{
		FSpawnRequest& NewRequestRef = InOutRequestsLockedProgression.AddDefaulted_GetRef();
		NewRequestRef.Class = UPSDataAsset::Get().GetStarWidget();
	}

	// --- Prepare spawn request
	const TWeakObjectPtr<ThisClass> WeakThis = this;
	const FOnSpawnAllCallback OnCompletedLockedProgression = [WeakThis, AmountOfLockedPoints](const TArray<FPoolObjectData>& CreatedObjects)
	{
		UPSMenuWidget* This = WeakThis.Get();
		// @todo: 
		//if (This)
		//{
		// #1 Create MyFunction
		//This->MYFunction(CreatedObject.GetChecked<UPSStarWidget>(), a, b);
		// void MYFunction(UPSTextWidget& SpawnedTXTWidget);

		// #2 Merge two lambdas

		// #3 remove requiests as class member: make it as local varaible
		
		// Setup spawned widget
		for (const FPoolObjectData& CreatedObject : CreatedObjects)
		{
			UPSStarWidget& SpawnedWidget = CreatedObject.GetChecked<UPSStarWidget>();
			checkf(&SpawnedWidget, TEXT("ERROR: 'ProgressionMenuWidgetInternal' is null"));

			SpawnedWidget.SetStarImage(UPSDataAsset::Get().GetLockedProgressionIcon());

			// Load and set the image texture here using ImagePath or other methods
			if (!This->HorizontalBox->HasChild(&SpawnedWidget))
			{
				This->HorizontalBox->AddChildToHorizontalBox(&SpawnedWidget);
			}
		}
	};

	if (!InOutRequestsLockedProgression.IsEmpty())
	{
		// --- Spawn widgets
		UPoolManagerSubsystem::Get().TakeFromPool(InOutRequestsLockedProgression, OnCompletedLockedProgression);
	}

	// --- Add handles if requested spawning, so they can be canceled if regenerate before spawning finished
	for (const FSpawnRequest& It : InOutRequestsLockedProgression)
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
