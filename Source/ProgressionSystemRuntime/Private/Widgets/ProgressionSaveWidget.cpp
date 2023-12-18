// Copyright (c) Yevhenii Selivanov


#include "Widgets/ProgressionSaveWidget.h"
//---

//---
#include <string>

#include "ProgressionSystemDataAsset.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UProgressionSaveWidget::ConfigureWidgetText(FText endGameState, FText pointsGained, FText totalScore)
{
	if (EndGameStateTextWidget && PointsGainTextWidget && TotalScoreTextWidget)
	{
		EndGameStateTextWidget->SetText(endGameState);
		PointsGainTextWidget->SetText(pointsGained);
		TotalScoreTextWidget->SetText(totalScore);
	}
}

void UProgressionSaveWidget::NativeConstruct()
{
	Super::NativeConstruct();
	// Hide this widget by default
	SetVisibility(ESlateVisibility::Collapsed);

	if (ConfirmButton)
	{
		ConfirmButton->SetClickMethod(EButtonClickMethod::PreciseClick);
		ConfirmButton->OnClicked.AddUniqueDynamic(this, &ThisClass::OnConfirmationButtonPressed);
	}

	ProgressionSystemComponentInternal = ProgressionSystemDataAssetInternal->GetProgressionSystemComponent();
}

void UProgressionSaveWidget::OnConfirmationButtonPressed()
{
	SetVisibility(ESlateVisibility::Collapsed);
}
