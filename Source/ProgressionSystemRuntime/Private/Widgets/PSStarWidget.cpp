// Copyright (c) Yevhenii Selivanov


#include "Widgets/PSStarWidget.h"
//--
#include "Blueprint/WidgetTree.h"
#include "Components/Image.h"
//--

// Updates the image used for the star display
void UPSStarWidget::SetStarImage(UTexture2D* Image)
{
	checkf(Image, TEXT("ERROR: 'Image' is null"));

	StarImageInternal->SetBrushFromTexture(Image);
}

void UPSStarWidget::NativeConstruct()
{
	Super::NativeConstruct();
	StarImageInternal = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
}
