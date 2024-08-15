// Copyright (c) Yevhenii Selivanov


#include "Widgets/PSStarWidget.h"
//--
#include "Components/Image.h"
//--

#include "Components/ProgressBar.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PSStarWidget)

// Updates the image used for the star display
void UPSStarWidget::SetStarImage(UTexture2D* Image)
{
	if (ensureMsgf(Image, TEXT("ASSERT: [%i] %s:\n'Image' is not valid!"), __LINE__, *FString(__FUNCTION__)))
	{
		StarImageInternal->SetBrushFromTexture(Image);
	}
}

void UPSStarWidget::UpdateProgressionBarPercentage(float NewProgressValue)
{
	StarProgressBarInternal->SetPercent(NewProgressValue);
}
