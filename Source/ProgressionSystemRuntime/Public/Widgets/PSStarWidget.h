// Copyright (c) Yevhenii Selivanov

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PSStarWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROGRESSIONSYSTEMRUNTIME_API UPSStarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/**
	* Updates the image used for the star display.
	* @param Image The new image to set for the star.
	*/
	UFUNCTION()
	void SetStarImage(UTexture2D* Image);

	/**
	* Updates the star image progress bar to fill
	* @param NewProgressValue new progress bar value to set.
	*/
	UFUNCTION()
	void UpdateProgressionBarPercentage(float NewProgressValue);

protected:
	// Storing star image information for lock/unlocked icon
	UPROPERTY(EditDefaultsOnly, Transient, Category = "C++", meta = (BlueprintProtected, BindWidget))
	TObjectPtr<class UImage> StarImageInternal = nullptr;

	// Storing star progress bar icon to partially or fully fill progression 
	UPROPERTY(EditDefaultsOnly, Transient, Category = "C++", meta = (BlueprintProtected, BindWidget))
	TObjectPtr<class UProgressBar> StarProgressBarInternal = nullptr;
};
