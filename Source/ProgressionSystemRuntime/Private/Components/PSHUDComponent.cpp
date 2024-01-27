// Copyright (c) Valerii Rotermel & Yevhenii Selivanov

#include "Components/PSHUDComponent.h"
//---
#include "Bomber.h"
#include "Controllers/MyPlayerController.h"
#include "UtilityLibraries/MyBlueprintFunctionLibrary.h"
#include "Data/PSDataAsset.h"
#include "ProgressionSystemRuntimeModule.h"
#include "Blueprint/WidgetTree.h"
#include "UI/MyHUD.h"
#include "Widgets/PSMenuWidget.h"
#include "Data/PSTypes.h"
#include "Data/PSSaveGameData.h"
#include "Data/PSWorldSubsystem.h"
#include "Components/Image.h"
#include "GameFramework/MyGameStateBase.h"
#include "GameFramework/MyPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "MyDataTable/MyDataTable.h"
#include "MyUtilsLibraries/WidgetUtilsLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PSHUDComponent)

// Sets default values for this component's properties
UPSHUDComponent::UPSHUDComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

// Called when the game starts
void UPSHUDComponent::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogProgressionSystem, Warning, TEXT("--- I'm running ---"));

	AMyHUD* MyHUD = Cast<AMyHUD>(GetOwner());
	const AMyHUD& HUD = *MyHUD;
	ProgressionMenuWidgetInternal = HUD.CreateWidgetByClass<UPSMenuWidget>(UPSWorldSubsystem::Get().GetPSDataAsset()->GetProgressionMenuWidget(), true, 1);
	checkf(ProgressionMenuWidgetInternal, TEXT("ERROR: 'ProgressionMenuWidgetInternal' is null"));

	// Listen states to spawn widgets
	if (AMyGameStateBase* MyGameState = UMyBlueprintFunctionLibrary::GetMyGameState())
	{
		HandleGameState(MyGameState);
	}
	else if (AMyPlayerController* MyPC = UMyBlueprintFunctionLibrary::GetLocalPlayerController())
	{
		MyPC->OnGameStateCreated.AddUniqueDynamic(this, &ThisClass::HandleGameState);
	}
	if (AMyPlayerState* MyPlayerState = UMyBlueprintFunctionLibrary::GetLocalPlayerState())
	{
		HandleEndGameState(MyPlayerState);
	}

	UPSWorldSubsystem::Get().OnCurrentRowDataChanged.AddDynamic(this, &ThisClass::OnPlayerTypeChanged);
	// Save reference of this component to the world subsystem
	UPSWorldSubsystem::Get().SetProgressionSystemComponent(this);
	SaveGameInstanceInternal = UPSWorldSubsystem::Get().GetCurrentSaveGameData();
	checkf(SaveGameInstanceInternal, TEXT("ERROR: 'SaveGameInstanceInternal' is null"));
	SavedProgressionRowDataInternal = UPSWorldSubsystem::Get().GetCurrentRowData();

	UpdateProgressionWidgetForPlayer();
}

void UPSHUDComponent::OnUnregister()
{
	Super::OnUnregister();
	if (ProgressionMenuWidgetInternal)
	{
		FWidgetUtilsLibrary::DestroyWidget(*ProgressionMenuWidgetInternal);
		ProgressionMenuWidgetInternal = nullptr;
	}

	ProgressionMenuWidgetInternal = nullptr;
}

// Save the progression depends on EEndGameState
void UPSHUDComponent::SavePoints(ELevelType Map, FPlayerTag Character, EEndGameState EndGameState)
{
	SavedProgressionRowDataInternal.CurrentLevelProgression += UPSWorldSubsystem::Get().GetProgressionReward(EndGameState);

	if (SavedProgressionRowDataInternal.CurrentLevelProgression >= SavedProgressionRowDataInternal.PointsToUnlock)
	{
		NextLevelProgressionRowData();
	}

	SaveCurrentGameProgression();
}

// Finds current game progression and save to save file
void UPSHUDComponent::SaveCurrentGameProgression()
{
	if (SaveGameInstanceInternal)
	{
		SaveGameInstanceInternal->SavedProgressionRowDataInternal = SavedProgressionRowDataInternal;

		//find in Save current FProgressionRow struct and save
		for (const auto& KeyValue : SaveGameInstanceInternal->SavedProgressionRows)
		{
			FName Key = KeyValue.Key;
			FPSRowData RowData = KeyValue.Value;

			if (RowData.Map == UMyBlueprintFunctionLibrary::GetLevelType() && RowData.Character == SavedProgressionRowDataInternal.Character)
			{
				SaveGameInstanceInternal->SavedProgressionRows[Key] = SavedProgressionRowDataInternal;
			}
		}
	}
	UPSWorldSubsystem::Get().SaveDataAsync();
}

// Find next item in the level from current. TMap represents levels in savefile 
void UPSHUDComponent::NextLevelProgressionRowData()
{
	FName Key;

	if (SaveGameInstanceInternal)
	{
		for (const auto& KeyValue : SaveGameInstanceInternal->SavedProgressionRows)
		{
			if (SavedProgressionRowDataInternal.Map == KeyValue.Value.Map && SavedProgressionRowDataInternal.Character == KeyValue.Value.Character)
			{
				Key = KeyValue.Key;
			}
		}

		checkf(SaveGameInstanceInternal, TEXT("ERROR: 'SaveGameInstanceInternal' is null"));
		// Find the iterator for the current key
		TMap<FName, FPSRowData>::TIterator CurrentIterator = SaveGameInstanceInternal->SavedProgressionRows.CreateIterator();

		// Iterate through the map until the specified key is found
		while (CurrentIterator && CurrentIterator.Key() != Key)
		{
			++CurrentIterator;
		}

		// Check if the iterator is pointing to a valid position
		if (CurrentIterator)
		{
			// Move to the next element
			++CurrentIterator;

			// Check if the iterator is pointing to a valid position (not at the end)
			if (CurrentIterator)
			{
				// Access the key and value using the iterator
				FName NextProgressionName = CurrentIterator.Key();
				SaveGameInstanceInternal->SavedProgressionRows[NextProgressionName].IsLevelLocked = false;

				UPSWorldSubsystem::Get().SaveDataAsync();
			}
		}
	}
}

// Listening game states changes events 
void UPSHUDComponent::OnGameStateChanged(ECurrentGameState CurrentGameState)
{
	checkf(ProgressionMenuWidgetInternal, TEXT("ERROR: 'ProgressionMenuWidgetInternal' is null"));
	// Show Progression Menu widget in Main Menu
	ProgressionMenuWidgetInternal->SetVisibility(CurrentGameState == ECurrentGameState::Menu ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	if (CurrentGameState == ECurrentGameState::Menu)
	{
		UpdateProgressionWidgetForPlayer();
	}
}

// Listening end game states changes events (win, lose, draw) 
void UPSHUDComponent::OnEndGameStateChanged(EEndGameState EndGameState)
{
	if (EndGameState != EEndGameState::None)
	{
		SavePoints(UMyBlueprintFunctionLibrary::GetLevelType(), SavedProgressionRowDataInternal.Character, EndGameState);
	}
}

// Listen game state changes events 
void UPSHUDComponent::HandleGameState(AMyGameStateBase* MyGameState)
{
	checkf(MyGameState, TEXT("ERROR: 'MyGameState' is null!"));

	// Listen states to handle this widget behavior
	MyGameState->OnGameStateChanged.AddUniqueDynamic(this, &ThisClass::OnGameStateChanged);

	if (MyGameState->GetCurrentGameState() == ECurrentGameState::Menu)
	{
		// Handle current game state initialized with delay
		OnGameStateChanged(ECurrentGameState::Menu);
	}
	else
	{
		// Enter the game in Menu game state
		MyGameState->ServerSetGameState(ECurrentGameState::Menu);
	}
}

void UPSHUDComponent::HandleEndGameState(AMyPlayerState* MyPlayerState)
{
	checkf(MyPlayerState, TEXT("ERROR: 'MyGameState' is null!"));
	MyPlayerState->OnEndGameStateChanged.AddUniqueDynamic(this, &ThisClass::OnEndGameStateChanged);
}

// Handle events when player type changes
void UPSHUDComponent::OnPlayerTypeChanged(const FPSRowData& RowData)
{
	CurrentPlayerTagInternal = RowData.Character;
	SavedProgressionRowDataInternal = RowData;
	UpdateProgressionWidgetForPlayer();
}

// Refresh the main menu progression widget player 
void UPSHUDComponent::UpdateProgressionWidgetForPlayer()
{
	if (SaveGameInstanceInternal)
	{
		checkf(ProgressionMenuWidgetInternal, TEXT("ERROR: 'ProgressionMenuWidgetInternal' is null"));
		// reset amount of start for a level
		ProgressionMenuWidgetInternal->ClearImagesFromHorizontalBox();

		//set updated amount of stars
		if (SavedProgressionRowDataInternal.CurrentLevelProgression >= SavedProgressionRowDataInternal.PointsToUnlock)
		{
			// set required points (stars)  to achieve for a level  
			ProgressionMenuWidgetInternal->AddImagesToHorizontalBox(SavedProgressionRowDataInternal.PointsToUnlock, 0);
		}
		else
		{
			// Calculate the unlocked against locked points (stars) 
			ProgressionMenuWidgetInternal->AddImagesToHorizontalBox(SavedProgressionRowDataInternal.CurrentLevelProgression, SavedProgressionRowDataInternal.PointsToUnlock - SavedProgressionRowDataInternal.CurrentLevelProgression); // Listen game state changes events 
		}
		DisplayLevelUIOverlay(SavedProgressionRowDataInternal.IsLevelLocked);
	}
}

// Show or hide the LevelUIOverlay depends on the level lock state for current level
// by default overlay is always displayed 
void UPSHUDComponent::DisplayLevelUIOverlay(bool IsLevelLocked)
{
	checkf(ProgressionMenuWidgetInternal, TEXT("ERROR: 'ProgressionMenuWidgetInternal' is null"));
	if (IsLevelLocked)
	{
		// Level is locked show the blocking overlay
		ProgressionMenuWidgetInternal->PSCBackgroundOverlay->SetVisibility(ESlateVisibility::Visible);
		ProgressionMenuWidgetInternal->PSCBackgroundIconLock->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		// Level is unlocked hide the blocking overlay
		ProgressionMenuWidgetInternal->PSCBackgroundOverlay->SetVisibility(ESlateVisibility::Collapsed);
		ProgressionMenuWidgetInternal->PSCBackgroundIconLock->SetVisibility(ESlateVisibility::Collapsed);
	}
}
