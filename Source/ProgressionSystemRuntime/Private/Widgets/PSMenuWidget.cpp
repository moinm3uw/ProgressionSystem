// Copyright (c) Valerii Rotermel & Yevhenii Selivanov

#include "Widgets/PSMenuWidget.h"
//---
#include "Data/PSDataAsset.h"
#include "Blueprint/WidgetTree.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Image.h"
#include "Widgets/PSStarWidget.h"
#include "Data/PSWorldSubsystem.h"
//---

#include "UI/MyHUD.h"
#include "UtilityLibraries/MyBlueprintFunctionLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PSMenuWidget)

// Dynamically populates a Horizontal Box with images representing unlocked and locked progression icons.
void UPSMenuWidget::AddImagesToHorizontalBox(int32 AmountOfUnlockedPoints, int32 AmountOfLockedPoints)
{
	AMyHUD* MyHUD = CastChecked<AMyHUD>(UMyBlueprintFunctionLibrary::GetMyHUD(this));
	const AMyHUD& HUD = *MyHUD;

	checkf(HorizontalBox, TEXT("ERROR: 'HorizontalBox' is null"));
	// Loop to create and add images to the Horizontal Box for unlocked stars
	for (int32 i = 0; i < AmountOfUnlockedPoints; i++)
	{
		UPSStarWidget* PSStarWidget = HUD.CreateWidgetByClass<UPSStarWidget>(UPSDataAsset::Get().GetStarWidget(), false);
		if (!ensureMsgf(PSStarWidget, TEXT("AttachStarWidget: Failed to create StarWidget")))
		{
			continue;
		}
		PSStarWidget->SetStarImage(UPSDataAsset::Get().GetUnlockedProgressionIcon());

		// Load and set the image texture here using ImagePath or other methods
		HorizontalBox->AddChildToHorizontalBox(PSStarWidget);
	}

	// Loop to create and add images to the Horizontal Box for unlocked stars
	for (int32 i = 0; i < AmountOfLockedPoints; i++)
	{
		UPSStarWidget* PSStarWidget = HUD.CreateWidgetByClass<UPSStarWidget>(UPSDataAsset::Get().GetStarWidget(), false);
		PSStarWidget->SetStarImage(UPSDataAsset::Get().GetLockedProgressionIcon());


		// Load and set the image texture here using ImagePath or other methods
		HorizontalBox->AddChildToHorizontalBox(PSStarWidget);
	}
}

// Removes all images from horizontal box
void UPSMenuWidget::ClearImagesFromHorizontalBox()
{
	if (HorizontalBox)
	{
		HorizontalBox->ClearChildren();
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
