// Copyright (c) Valerii Rotermel & Yevhenii Selivanov

#include "Widgets/PSSaveWidget.h"
//---

//---
#include "Components/Button.h"
#include "Components/TextBlock.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PSSaveWidget)

void UPSSaveWidget::ConfigureWidgetText(FText endGameState, FText pointsGained, FText totalScore)
{
	if (EndGameStateTextWidget && PointsGainTextWidget && TotalScoreTextWidget)
	{
		EndGameStateTextWidget->SetText(endGameState);
		PointsGainTextWidget->SetText(pointsGained);
		TotalScoreTextWidget->SetText(totalScore);
	}
}

void UPSSaveWidget::NativeConstruct()
{
	Super::NativeConstruct();
	// Hide this widget by default
	SetVisibility(ESlateVisibility::Collapsed);

	if (ConfirmButton)
	{
		ConfirmButton->SetClickMethod(EButtonClickMethod::PreciseClick);
		ConfirmButton->OnClicked.AddUniqueDynamic(this, &ThisClass::OnConfirmationButtonPressed);
	}
}

void UPSSaveWidget::OnConfirmationButtonPressed()
{
	SetVisibility(ESlateVisibility::Collapsed);
}
