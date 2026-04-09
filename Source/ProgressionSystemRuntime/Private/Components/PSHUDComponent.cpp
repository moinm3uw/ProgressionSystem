// Copyright (c) Valerii Rotermel & Yevhenii Selivanov

#include "Components/PSHUDComponent.h"

// PS
#include "Data/PSDataAsset.h"
#include "Data/PSWorldSubsystem.h"
#include "PsGameplayTags.h"
#include "Widgets/PSEndGameWidget.h"
#include "Widgets/PSOverlayWidget.h"

// Bomber
#include "DalSubsystem.h"
#include "Structures/BmrGameplayTags.h"
#include "Subsystems/BmrWidgetsSubsystem.h"
#include "Subsystems/GlobalMessageSubsystem.h"

// UE
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

	UDalSubsystem::Get().ListenForDataAsset<UPSDataAsset>(this, &ThisClass::OnDataAssetLoaded);
}

// Called when the PS data asset is loaded and available
void UPSHUDComponent::OnDataAssetLoaded_Implementation(const UPSDataAsset* DataAsset)
{
	// Binds to local character ready to guarantee that the player controller is initialized
	// so we can safely use Widget's Subsystem
	UGlobalMessageSubsystem::CallOrStartListeningForGlobalMessage(BmrGameplayTags::Event::Player_LocalPawnReady, this, &ThisClass::OnLocalPawnReady);
}

// Called when the component is unregistered, used to clean up resources
void UPSHUDComponent::OnUnregister()
{
	Super::OnUnregister();

	UGlobalMessageSubsystem::StopListeningForAllGlobalMessages(this);
}

// Is called when local player character is ready to guarantee that they player controller is initialized for the Widget SubSystem
void UPSHUDComponent::OnLocalPawnReady_Implementation(const FGameplayEventData& Payload)
{
	UPSWorldSubsystem& WorldSubsystem = UPSWorldSubsystem::Get();
	WorldSubsystem.OnInitialize.AddUniqueDynamic(this, &ThisClass::OnInitialized);
	WorldSubsystem.OnWorldSubSystemInitialize();
}
