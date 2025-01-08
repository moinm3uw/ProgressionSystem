// Copyright (c) Valerii Rotermel & Yevhenii Selivanov

#include "Components/PSHUDComponent.h"
//---

#include "Bomber.h"
#include "UtilityLibraries/MyBlueprintFunctionLibrary.h"
#include "Data/PSDataAsset.h"
#include "Blueprint/WidgetTree.h"
#include "Widgets/PSEndGameWidget.h"
#include "Data/PSTypes.h"
#include "Data/PSSaveGameData.h"
#include "Data/PSWorldSubsystem.h"
#include "GameFramework/MyPlayerState.h"
#include "MyUtilsLibraries/WidgetUtilsLibrary.h"
#include "Subsystems/GlobalEventsSubsystem.h"
#include "Subsystems/WidgetsSubsystem.h"
#include "UI/SettingsWidget.h"
#include "Widgets/PSOverlayWidget.h"
#include "LevelActors/PlayerCharacter.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PSHUDComponent)

// Sets default values for this component's properties
UPSHUDComponent::UPSHUDComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

// Called when main save game file is loaded
void UPSHUDComponent::OnInitialized_Implementation()
{
	// Binds the local player state ready event to the handler
	BIND_ON_LOCAL_PLAYER_STATE_READY(this, ThisClass::OnLocalPlayerStateReady);

	// Save reference of this component to the world subsystem
	UPSWorldSubsystem::Get().SetHUDComponent(this);
}

// Subscribes to the end game state change notification on the player state.
void UPSHUDComponent::OnLocalPlayerStateReady_Implementation(AMyPlayerState* PlayerState, int32 CharacterID)
{
	// Ensure that PlayerState is not null before subscribing to the event
	if (!ensureMsgf(PlayerState, TEXT("ASSERT: [%i] %hs:\n'PlayerState' is null!"), __LINE__, __FUNCTION__))
	{
		return;
	}
	PlayerState->OnEndGameStateChanged.AddUniqueDynamic(this, &ThisClass::OnEndGameStateChanged);
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

	if (ProgressionEndGameWidgetInternal)
	{
		FWidgetUtilsLibrary::DestroyWidget(*ProgressionEndGameWidgetInternal);
		ProgressionEndGameWidgetInternal = nullptr;
	}
	if (ProgressionMenuOverlayWidgetInternal)
	{
		FWidgetUtilsLibrary::DestroyWidget(*ProgressionMenuOverlayWidgetInternal);
		ProgressionMenuOverlayWidgetInternal = nullptr;
	}
	if (UGlobalEventsSubsystem* EventSubsystem = UGlobalEventsSubsystem::GetGlobalEventsSubsystem())
	{
		EventSubsystem->BP_OnLocalCharacterReady.RemoveAll(this);
	}
}

// Save the progression depends on EEndGameState
void UPSHUDComponent::SavePoints(EEndGameState EndGameState)
{
	// @h4rdmol to move to Subsystem instead of hud
	const UPSSaveGameData* SaveGameInstance = UPSWorldSubsystem::Get().GetCurrentSaveGameData();
	if (!ensureMsgf(SaveGameInstance, TEXT("ASSERT: [%i] %hs:\n'SaveGameInstance' is null!"), __LINE__, __FUNCTION__))
	{
		return;
	}
	UPSSaveGameData* SaveGameData = UPSWorldSubsystem::Get().GetCurrentSaveGameData();
	SaveGameData->SavePoints(EndGameState);
}

// Listening end game states changes events (win, lose, draw) 
void UPSHUDComponent::OnEndGameStateChanged_Implementation(EEndGameState EndGameState)
{
	if (EndGameState != EEndGameState::None)
	{
		SavePoints(EndGameState);
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
	UWidgetsSubsystem& WidgetsSubsystem = UWidgetsSubsystem::Get();
	ProgressionEndGameWidgetInternal = WidgetsSubsystem.CreateManageableWidgetChecked<UPSEndGameWidget>(UPSDataAsset::Get().GetProgressionEndGameWidget());
	ProgressionMenuOverlayWidgetInternal = WidgetsSubsystem.CreateManageableWidgetChecked<UPSOverlayWidget>(UPSDataAsset::Get().GetProgressionOverlayWidget());

	UPSWorldSubsystem& WorldSubsystem = UPSWorldSubsystem::Get();
	WorldSubsystem.OnInitialize.AddUniqueDynamic(this, &ThisClass::OnInitialized);
	WorldSubsystem.OnWorldSubSystemInitialize();
}
