// Copyright (c) Valerii Rotermel & Yevhenii Selivanov

#include "Widgets/PSMenuWidget.h"
//---

//---
#include "Data/PSDataAsset.h"
#include "Blueprint/WidgetTree.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Image.h"
#include "Data/PSWorldSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PSMenuWidget)

void UPSMenuWidget::AddImagesToHorizontalBox(int32 AmountOfUnlockedPoints, int32 AmountOfLockedPoints)
{
	checkf(HorizontalBox, TEXT("ERROR: 'HorizontalBox' is null"));
	if (HorizontalBox)
	{
		// Loop to create and add images to the Horizontal Box for unlocked stars
		for (int32 i = 0; i < AmountOfUnlockedPoints; i++)
		{
			UImage* ImageWidget = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
			ImageWidget->SetBrushFromTexture(UPSDataAsset::Get().GetUnlockedProgressionIcon());

			// Load and set the image texture here using ImagePath or other methods
			HorizontalBox->AddChildToHorizontalBox(ImageWidget);

			// Create the UHorizontalBoxSlot and assign it to the ImageWidget
			UHorizontalBoxSlot* HorizontalBoxSlot = Cast<UHorizontalBoxSlot>(ImageWidget->Slot);
			if (HorizontalBoxSlot)
			{
				HorizontalBoxSlot->SetPadding(FMargin(20.0f, 10.0f, 20.0f, 10.0f));
				HorizontalBoxSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
				HorizontalBoxSlot->SetHorizontalAlignment(HAlign_Fill);
				HorizontalBoxSlot->SetVerticalAlignment(VAlign_Fill);
			}
		}

		// Loop to create and add images to the Horizontal Box for unlocked stars
		for (int32 i = 0; i < AmountOfLockedPoints; i++)
		{
			UImage* ImageWidget = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
			ImageWidget->SetBrushFromTexture(UPSDataAsset::Get().GetLockedProgressionIcon());
			// Load and set the image texture here using ImagePath or other methods
			HorizontalBox->AddChildToHorizontalBox(ImageWidget);
			// Create the UHorizontalBoxSlot and assign it to the ImageWidget
			UHorizontalBoxSlot* HorizontalBoxSlot = Cast<UHorizontalBoxSlot>(ImageWidget->Slot);
			if (HorizontalBoxSlot)
			{
				HorizontalBoxSlot->SetPadding(FMargin(20.0f, 10.0f, 20.0f, 10.0f));
				HorizontalBoxSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
				HorizontalBoxSlot->SetHorizontalAlignment(HAlign_Fill);
				HorizontalBoxSlot->SetVerticalAlignment(VAlign_Fill);
			}
		}
	}
}

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
