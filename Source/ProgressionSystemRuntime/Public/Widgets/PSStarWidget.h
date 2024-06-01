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

protected:
	// Storing star image information for lock/unlocked icon
	UPROPERTY(EditDefaultsOnly, Transient, Category = "C++", meta = (BlueprintProtected, BindWidget))
	TObjectPtr<class UImage> StarImageInternal = nullptr;

	virtual void NativeConstruct() override;
};
