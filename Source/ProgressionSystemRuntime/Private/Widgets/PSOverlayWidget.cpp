// Copyright (c) Yevhenii Selivanov


#include "Widgets/PSOverlayWidget.h"

#include "Components/Image.h"

// Sets the visibility of the background overlay and lock icon.
void UPSOverlayWidget::SetOverlayVisibility(ESlateVisibility VisibilitySlate)
{
	// Level is unlocked hide the blocking overlay
	PSCBackgroundOverlay->SetVisibility(VisibilitySlate);
	PSCBackgroundIconLock->SetVisibility(VisibilitySlate);
}
