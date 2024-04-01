// Copyright (c) Yevhenii Selivanov


#include "PoolManager/UPoolFactory_Widget.h"

#include "NavigationSystemTypes.h"
#include "Data/PSDataAsset.h"
#include "UI/MyHUD.h"
#include "UtilityLibraries/MyBlueprintFunctionLibrary.h"
#include "Widgets/PSStarWidget.h"

const UClass* UUPoolFactory_Widget::GetObjectClass_Implementation() const
{
	return UUserWidget::StaticClass();
}

UObject* UUPoolFactory_Widget::SpawnNow_Implementation(const FSpawnRequest& Request)
{
	// Super::SpawnNow_Implementation(Request);
	AMyHUD* MyHUD = CastChecked<AMyHUD>(UMyBlueprintFunctionLibrary::GetMyHUD(this));
	const AMyHUD& HUD = *MyHUD;

	const TSubclassOf<UUserWidget> WidgetClass = const_cast<UClass*>(Request.Class.Get());

	UUserWidget* CreatedWidget = HUD.CreateWidgetByClass<UUserWidget>(WidgetClass, false);
	checkf(CreatedWidget, TEXT("%s: ERROR: %s failed to create"), *FString(__FUNCTION__), *CreatedWidget->GetName());
	CreatedWidget->SetVisibility(ESlateVisibility::Hidden);

	FPoolObjectData PoolObjectData;
	PoolObjectData.bIsActive = true;
	PoolObjectData.PoolObject = CreatedWidget;
	PoolObjectData.Handle = Request.Handle;

	if (Request.Callbacks.OnPreRegistered != nullptr)
	{
		Request.Callbacks.OnPreRegistered(PoolObjectData);
	}

	if (Request.Callbacks.OnPostSpawned != nullptr)
	{
		Request.Callbacks.OnPostSpawned(PoolObjectData);
	}
	return CreatedWidget;
}

void UUPoolFactory_Widget::OnTakeFromPool_Implementation(UObject* Object, const FTransform& Transform)
{
	Super::OnTakeFromPool_Implementation(Object, Transform);
}

void UUPoolFactory_Widget::OnReturnToPool_Implementation(UObject* Object)
{
	Super::OnReturnToPool_Implementation(Object);

	UUserWidget* MyUserWidget = CastChecked<UUserWidget>(Object);
	MyUserWidget->SetVisibility(ESlateVisibility::Hidden);
}
