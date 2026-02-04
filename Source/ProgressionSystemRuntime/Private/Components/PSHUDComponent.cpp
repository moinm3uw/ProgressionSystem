// Copyright (c) Valerii Rotermel & Yevhenii Selivanov

#include "Components/PSHUDComponent.h"

// PS
#include "Data/PSDataAsset.h"
#include "Data/PSWorldSubsystem.h"
#include "PsGameplayTags.h"
#include "Widgets/PSEndGameWidget.h"
#include "Widgets/PSOverlayWidget.h"

// Bomber
#include "Actors/BmrPawn.h"
#include "Structures/BmrGameplayTags.h"
#include "Subsystems/BmrGameplayMessageSubsystem.h"
#include "Subsystems/BmrWidgetsSubsystem.h"
#include "UtilityLibraries/BmrBlueprintFunctionLibrary.h"

// UE
#include "Abilities/GameplayAbilityTypes.h"
#include "Blueprint/WidgetTree.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PSHUDComponent)

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
	const UBmrWidgetsSubsystem* WidgetsSubsystem = UBmrWidgetsSubsystem::GetWidgetsSubsystem();
	return WidgetsSubsystem ? WidgetsSubsystem->GetWidgetByTag<UPSEndGameWidget>(PsGameplayTags::UI::Widget_EndGame) : nullptr;
}

// Returns the Progression Menu overlay widget
UPSOverlayWidget* UPSHUDComponent::GetProgressionMenuOverlayWidget() const
{
	const UBmrWidgetsSubsystem* WidgetsSubsystem = UBmrWidgetsSubsystem::GetWidgetsSubsystem();
	return WidgetsSubsystem ? WidgetsSubsystem->GetWidgetByTag<UPSOverlayWidget>(PsGameplayTags::UI::Widget_MenuOverlay) : nullptr;
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
	BIND_ON_LOCAL_PAWN_READY(this, ThisClass::OnLocalPawnReady);
}

// Called when the component is unregistered, used to clean up resources
void UPSHUDComponent::OnUnregister()
{
	Super::OnUnregister();

	UPSWorldSubsystem::Get().PerformCleanUp();

	if (UBmrWidgetsSubsystem* WidgetsSubsystem = UBmrWidgetsSubsystem::GetWidgetsSubsystem())
	{
		WidgetsSubsystem->DestroyManageableWidgetByTag(PsGameplayTags::UI::Widget_EndGame);
		WidgetsSubsystem->DestroyManageableWidgetByTag(PsGameplayTags::UI::Widget_MenuOverlay);
	}
}

// Is called when local player character is ready to guarantee that they player controller is initialized for the Widget SubSystem
void UPSHUDComponent::OnLocalPawnReady_Implementation(const FGameplayEventData& Payload)
{
	// Create widgets now as fast as possible, later we will register them in Widgets Subsystem
	UBmrWidgetsSubsystem::Get().CreateManageableWidgetChecked(UPSDataAsset::Get().GetProgressionEndGameWidget());
	UBmrWidgetsSubsystem::Get().CreateManageableWidgetChecked(UPSDataAsset::Get().GetProgressionOverlayWidget());

	UPSWorldSubsystem& WorldSubsystem = UPSWorldSubsystem::Get();
	WorldSubsystem.OnInitialize.AddUniqueDynamic(this, &ThisClass::OnInitialized);
	WorldSubsystem.OnWorldSubSystemInitialize();
}
