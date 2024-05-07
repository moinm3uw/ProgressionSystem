// Copyright (c) Valerii Rotermel & Yevhenii Selivanov

#include "Data/PSWorldSubsystem.h"

#include "Components/PSHUDComponent.h"
#include "Data/PSDataAsset.h"
#include "Data/PSSaveGameData.h"
#include "Kismet/GameplayStatics.h"
#include "LevelActors/PlayerCharacter.h"
#include "MyDataTable/MyDataTable.h"
#include "MyUtilsLibraries/UtilsLibrary.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "MyUtilsLibraries/GameplayUtilsLibrary.h"
#include "Subsystems/GlobalEventsSubsystem.h"
#include "UtilityLibraries/MyBlueprintFunctionLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PSWorldSubsystem)

// Returns this Subsystem, is checked and wil crash if can't be obtained
UPSWorldSubsystem& UPSWorldSubsystem::Get()
{
	const UWorld* World = UUtilsLibrary::GetPlayWorld();
	checkf(World, TEXT("%s: 'World' is null"), *FString(__FUNCTION__));
	UPSWorldSubsystem* ThisSubsystem = World->GetSubsystem<ThisClass>();
	checkf(ThisSubsystem, TEXT("%s: 'ProgressionSubsystem' is null"), *FString(__FUNCTION__));
	return *ThisSubsystem;
}

// Returns this Subsystem, is checked and wil crash if can't be obtained
UPSWorldSubsystem& UPSWorldSubsystem::Get(const UObject& WorldContextObject)
{
	const UWorld* World = GEngine->GetWorldFromContextObjectChecked(&WorldContextObject);
	checkf(World, TEXT("%s: 'World' is null"), *FString(__FUNCTION__));
	UPSWorldSubsystem* ThisSubsystem = World->GetSubsystem<ThisClass>();
	checkf(ThisSubsystem, TEXT("%s: 'ProgressionSubsystem' is null"), *FString(__FUNCTION__));
	return *ThisSubsystem;
}
// Returns current row of progression system
const FPSRowData& UPSWorldSubsystem::GetCurrentRow() const
{
	const UPSSaveGameData* SaveGameInstance = GetCurrentSaveGameData();
	checkf(SaveGameInstance, TEXT("ERROR: 'SaveGameInstanceInternal' is null"));

	return SaveGameInstance->GetCurrentRow();
}

// Returns the data asset that contains all the assets of Progression System game feature
const UPSDataAsset* UPSWorldSubsystem::GetPSDataAsset() const
{
	return PSDataAssetInternal.LoadSynchronous();
}

// Set the progression system component
void UPSWorldSubsystem::SetHUDComponent(UPSHUDComponent* MyHUDComponent)
{
	checkf(MyHUDComponent, TEXT("%s: My progression system component is null"), *FString(__FUNCTION__));
	PSHUDComponentInternal = MyHUDComponent;
}

// Called when world is ready to start gameplay before the game mode transitions to the correct state and call BeginPlay on all actors 
void UPSWorldSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	// Subscribe events on player type changed and Character spawned
	BIND_ON_LOCAL_CHARACTER_READY(this, ThisClass::OnCharacterReady);

	LoadGameFromSave();
}

// Clears all transient data created by this subsystem
void UPSWorldSubsystem::Deinitialize()
{
	Super::Deinitialize();
	PSHUDComponentInternal = nullptr;
}

// Is called when a player has been changed
void UPSWorldSubsystem::OnPlayerTypeChanged(FPlayerTag PlayerTag)
{
	SaveGameInstanceInternal->SetRowByTag(PlayerTag);
	OnCurrentRowDataChanged.Broadcast(PlayerTag);
}

void UPSWorldSubsystem::OnCharacterReady(APlayerCharacter* PlayerCharacter, int32 CharacterID)
{
	PlayerCharacter->OnPlayerTypeChanged.AddUniqueDynamic(this, &ThisClass::OnPlayerTypeChanged);
}

// Load game from save file or create a new one (does initial load from data table)
void UPSWorldSubsystem::LoadGameFromSave()
{
	// Check if the save game file exists
	if (UGameplayStatics::DoesSaveGameExist(SaveGameInstanceInternal->GetSaveSlotName(), SaveGameInstanceInternal->GetSaveSlotIndex()))
	{
		SaveGameInstanceInternal = Cast<UPSSaveGameData>(UGameplayStatics::LoadGameFromSlot(SaveGameInstanceInternal->GetSaveSlotName(), SaveGameInstanceInternal->GetSaveSlotIndex()));
	}
	else
	{
		// Save file does not exist
		// do initial load from data table
		TMap<FName, FPSRowData> SavedProgressionRows;
		const UDataTable* ProgressionDataTable = UPSDataAsset::Get().GetProgressionDataTable();
		checkf(ProgressionDataTable, TEXT("ERROR: 'ProgressionDataTableInternal' is null"));
		UMyDataTable::GetRows(*ProgressionDataTable, SavedProgressionRows);
		SaveGameInstanceInternal = Cast<UPSSaveGameData>(UGameplayStatics::CreateSaveGameObject(UPSSaveGameData::StaticClass()));

		if (SaveGameInstanceInternal)
		{
			SaveGameInstanceInternal->SetProgressionMap(SavedProgressionRows);
		}
	}
	SetFirstElemetAsCurrent();
}

// Always set first levels as unlocked on begin play
void UPSWorldSubsystem::SetFirstElemetAsCurrent()
{
	if (SaveGameInstanceInternal)
	{
		constexpr int32 FirstElementIndex = 0;
		SaveGameInstanceInternal->SetCurrentProgressionRowByIndex(FirstElementIndex);
		const FName RowName = SaveGameInstanceInternal->GetCurrentRowName();
		SaveGameInstanceInternal->UnlockLevelByName(RowName);
	}
	SaveDataAsync();
}

// Saves the progression to the local files
void UPSWorldSubsystem::SaveDataAsync()
{
	checkf(SaveGameInstanceInternal, TEXT("ERROR: 'SaveGameInstanceInternal' is null"));
	UGameplayStatics::AsyncSaveGameToSlot(SaveGameInstanceInternal, SaveGameInstanceInternal->GetSaveSlotName(), SaveGameInstanceInternal->GetSaveSlotIndex());
}

// Removes all saved data of the Progression system and creates a new empty data
void UPSWorldSubsystem::ResetSaveGameData()
{
	const FString& SlotName = SaveGameInstanceInternal->GetSaveSlotName();
	const int32 UserIndex = SaveGameInstanceInternal->GetSaveSlotIndex();

	UGameplayUtilsLibrary::ResetSaveGameData(SaveGameInstanceInternal, SlotName, UserIndex);

	// Re-load a new save game object. Load game from save creates a save file if there is no such
	LoadGameFromSave();
	UPSHUDComponent* PSHUDComponent = GetProgressionSystemHUDComponent();
	checkf(PSHUDComponent, TEXT("ERROR: 'PSHUDComponent' is null"));
	PSHUDComponent->UpdateProgressionWidgetForPlayer();
}

// Unlocks all levels of the Progression System
void UPSWorldSubsystem::UnlockAllLevels()
{
	SaveGameInstanceInternal->UnlockAllLevels();
	SaveDataAsync();
	UPSHUDComponent* PSHUDComponent = GetProgressionSystemHUDComponent();
	checkf(PSHUDComponent, TEXT("ERROR: 'PSHUDComponent' is null"));
	PSHUDComponent->UpdateProgressionWidgetForPlayer();
}

// Returns rewards from data table for each type of game endings 
float UPSWorldSubsystem::GetProgressionReward(EEndGameState EndGameState)
{
	switch (EndGameState)
	{
	case EEndGameState::Win:
		return GetCurrentRow().WinReward;
	case EEndGameState::Draw:
		return GetCurrentRow().DrawReward;
	case EEndGameState::Lose:
		return GetCurrentRow().LossReward;
	default:
		return 0.f;
	}
}
