// Copyright (c) Valerii Rotermel & Yevhenii Selivanov

#include "Components/PSHUDComponent.h"
//---

#include "UtilityLibraries/MyBlueprintFunctionLibrary.h"
#include "Data/PSDataAsset.h"
#include "Blueprint/WidgetTree.h"
#include "Widgets/PSEndGameWidget.h"
#include "Data/PSWorldSubsystem.h"
#include "MyUtilsLibraries/WidgetUtilsLibrary.h"
#include "Subsystems/GlobalEventsSubsystem.h"
#include "Subsystems/WidgetsSubsystem.h"
#include "Widgets/PSOverlayWidget.h"
#include "LevelActors/PlayerCharacter.h"
#include "NativeGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PSHUDComponent)

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_UI_WIDGET_PROGRESSIONSYSTEM_ENDGAME, TEXT("UI.Widget.ProgressionSystem.EndGame"));
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_UI_WIDGET_PROGRESSIONSYSTEM_MENUOVERLAY, TEXT("UI.Widget.ProgressionSystem.MenuOverlay"));

// Sets default values for this component's properties
UPSHUDComponent::UPSHUDComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

// Returns the Progression End Game widget
UPSEndGameWidget* UPSHUDComponent::GetProgressionEndGameWidget() const
{
	const UWidgetsSubsystem* WidgetsSubsystem = UWidgetsSubsystem::GetWidgetsSubsystem();
	return WidgetsSubsystem ? WidgetsSubsystem->GetWidgetByTag<UPSEndGameWidget>(TAG_UI_WIDGET_PROGRESSIONSYSTEM_ENDGAME) : nullptr;
}

// Returns the Progression Menu overlay widget
UPSOverlayWidget* UPSHUDComponent::GetProgressionMenuOverlayWidget() const
{
	const UWidgetsSubsystem* WidgetsSubsystem = UWidgetsSubsystem::GetWidgetsSubsystem();
	return WidgetsSubsystem ? WidgetsSubsystem->GetWidgetByTag<UPSOverlayWidget>(TAG_UI_WIDGET_PROGRESSIONSYSTEM_MENUOVERLAY) : nullptr;
}

// Called when main save game file is loaded
void UPSHUDComponent::OnInitialized_Implementation()
{
	// Save reference of this component to the world subsystem
	UPSWorldSubsystem::Get().SetHUDComponent(this);
}

// Called when the game starts
void UPSHUDComponent::BeginPlay()
{
	Super::BeginPlay();
	// Binds to local character ready to guarantee that the player controller is initialized
	// so we can safely use Widget's Subsystem
	BIND_ON_LOCAL_CHARACTER_READY(this, ThisClass::OnLocalCharacterReady);
}

// Called when the component is unregistered, used to clean up resources
void UPSHUDComponent::OnUnregister()
{
	Super::OnUnregister();

	UPSWorldSubsystem::Get().PerformCleanUp();

	if (UWidgetsSubsystem* WidgetsSubsystem = UWidgetsSubsystem::GetWidgetsSubsystem())
	{
		WidgetsSubsystem->DestroyManageableWidgetByTag(TAG_UI_WIDGET_PROGRESSIONSYSTEM_ENDGAME);
		WidgetsSubsystem->DestroyManageableWidgetByTag(TAG_UI_WIDGET_PROGRESSIONSYSTEM_MENUOVERLAY);
	}
}

//Is called when local player character is ready to guarantee that they player controller is initialized for the Widget SubSystem
void UPSHUDComponent::OnLocalCharacterReady_Implementation(APlayerCharacter* Character, int32 CharacterID)
{
	if (!Character || !Character->IsLocallyControlled())
	{
		return;
	}

	// Create widgets now as fast as possible, later we will register them in Widgets Subsystem
	UWidgetsSubsystem::Get().CreateManageableWidgetChecked(UPSDataAsset::Get().GetProgressionEndGameWidget());
	UWidgetsSubsystem::Get().CreateManageableWidgetChecked(UPSDataAsset::Get().GetProgressionOverlayWidget());

	UPSWorldSubsystem& WorldSubsystem = UPSWorldSubsystem::Get();
	WorldSubsystem.OnInitialize.AddUniqueDynamic(this, &ThisClass::OnInitialized);
	WorldSubsystem.OnWorldSubSystemInitialize();
}
