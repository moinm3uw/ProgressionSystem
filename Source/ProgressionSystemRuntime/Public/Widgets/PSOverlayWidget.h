// Copyright (c) Yevhenii Selivanov

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PSOverlayWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROGRESSIONSYSTEMRUNTIME_API UPSOverlayWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	
	/**
	* Sets the visibility of the background overlay and lock icon.
	* @param VisibilitySlate The visibility state (e.g., Visible, Collapsed) to apply to the overlay and icon.
	*/
	UFUNCTION(BlueprintCallable, Category= "C++", meta = (BlueprintProtected))
	void SetOverlayVisibility(ESlateVisibility VisibilitySlate);
	
protected:
	/** Background overlay for tint effect */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Transient, Category = "C++", meta = (BlueprintProtected, BindWidget))
	TObjectPtr<class UImage> PSCBackgroundOverlay = nullptr;

	/** Background overlay lock icon */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Transient, Category = "C++", meta = (BlueprintProtected, BindWidget))
	TObjectPtr<class UImage> PSCBackgroundIconLock = nullptr;
};
