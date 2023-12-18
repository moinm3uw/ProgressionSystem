// Copyright (c) Yevhenii Selivanov

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ProgressionSaveWidget.generated.h"

class UButton;
class UTextBlock;
class UImage;
/**
 * 
 */
UCLASS()
class PROGRESSIONSYSTEMRUNTIME_API UProgressionSaveWidget : public UUserWidget
{
	GENERATED_BODY()
	public:

	/** Main text to display the progression text widget */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "C++", meta = (BlueprintProtected, BindWidget))
	TObjectPtr<UTextBlock> EndGameStateTextWidget = nullptr;

	/** Text to display the amount of points earned after game */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "C++", meta = (BlueprintProtected, BindWidget))
	TObjectPtr<UTextBlock> PointsGainTextWidget = nullptr;

	/** Text to display the total amount of player points after game */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "C++", meta = (BlueprintProtected, BindWidget))
	TObjectPtr<UTextBlock> TotalScoreTextWidget = nullptr;
	
	/** The button to hide save progression widget. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "C++", meta = (BlueprintProtected, BindWidget))
	TObjectPtr<UButton> ConfirmButton = nullptr;

	UFUNCTION()
	void ConfigureWidgetText(FText endGameState, FText pointsGained, FText totalScore);
	
	/*********************************************************************************************
	 * Protected functions
	 ********************************************************************************************* */

protected:
	/** Called after the underlying slate widget is constructed.
	 * May be called multiple times due to adding and removing from the hierarchy. */
	virtual void NativeConstruct() override;
	
	/** Is called when player pressed the button to show the player progression details. */
	UFUNCTION(BlueprintCallable, Category = "C++")
	void OnConfirmationButtonPressed();

	/** Progression System data asset */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BlueprintProtected, DisplayName = "Progression System Data Asset"))
	TObjectPtr<class UProgressionSystemDataAsset> ProgressionSystemDataAssetInternal;
	
	/** Progression System component reference */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BlueprintProtected, DisplayName = "Progression System Component"))
	TObjectPtr<class UProgressionSystemComponent> ProgressionSystemComponentInternal = nullptr; 
};
