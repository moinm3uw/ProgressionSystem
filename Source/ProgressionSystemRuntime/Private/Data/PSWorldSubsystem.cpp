// Copyright (c) Valerii Rotermel & Yevhenii Selivanov

#include "Data/PSWorldSubsystem.h"

#include "PoolManagerSubsystem.h"
#include "Components/MySkeletalMeshComponent.h"
#include "Components/PSHUDComponent.h"
#include "Components/PSSpotComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Data/PSDataAsset.h"
#include "Data/PSSaveGameData.h"
#include "Kismet/GameplayStatics.h"
#include "LevelActors/PlayerCharacter.h"
#include "MyDataTable/MyDataTable.h"
#include "MyUtilsLibraries/UtilsLibrary.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "LevelActors/PSStarActor.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "MyUtilsLibraries/SaveUtilsLibrary.h"
#include "Subsystems/GameDifficultySubsystem.h"
#include "Subsystems/GlobalEventsSubsystem.h"
#include "UtilityLibraries/MyBlueprintFunctionLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PSWorldSubsystem)

// Returns this Subsystem, is checked and will crash if it can't be obtained
UPSWorldSubsystem& UPSWorldSubsystem::Get()
{
	const UWorld* World = UUtilsLibrary::GetPlayWorld();
	checkf(World, TEXT("%s: 'World' is null"), *FString(__FUNCTION__));
	UPSWorldSubsystem* ThisSubsystem = World->GetSubsystem<ThisClass>();
	checkf(ThisSubsystem, TEXT("%s: 'ProgressionSubsystem' is null"), *FString(__FUNCTION__));
	return *ThisSubsystem;
}

// Returns this Subsystem, is checked and will crash if it can't be obtained
UPSWorldSubsystem& UPSWorldSubsystem::Get(const UObject& WorldContextObject)
{
	const UWorld* World = GEngine->GetWorldFromContextObjectChecked(&WorldContextObject);
	checkf(World, TEXT("%s: 'World' is null"), *FString(__FUNCTION__));
	UPSWorldSubsystem* ThisSubsystem = World->GetSubsystem<ThisClass>();
	checkf(ThisSubsystem, TEXT("%s: 'ProgressionSubsystem' is null"), *FString(__FUNCTION__));
	return *ThisSubsystem;
}

// Set current row of progression system by tag
void UPSWorldSubsystem::SetCurrentRowByTag(FPlayerTag NewRowPlayerTag)
{
	for (const TTuple<FName, FPSRowData>& KeyValue : ProgressionSettingsDataInternal)
	{
		const FPSRowData& RowData = KeyValue.Value;

		if (RowData.Character == NewRowPlayerTag)
		{
			CurrentRowNameInternal = KeyValue.Key;
			OnCurrentActiveSaveRowChanged.Broadcast(NewRowPlayerTag);
			UpdateProgressionStarActors();
			return; // Exit immediately after finding the match
		}
	}
}

// Returns the data asset that contains all the assets of Progression System game feature
const UPSDataAsset* UPSWorldSubsystem::GetPSDataAsset() const
{
	return UMyPrimaryDataAsset::GetOrLoadOnce(PSDataAssetInternal);
}

//  Returns a current save to disk row name
FName UPSWorldSubsystem::GetFirstSaveToDiskRowName() const
{
	if (!ensureMsgf(SaveGameDataInternal, TEXT("ASSERT: [%i] %hs:\n'SaveGameDataInternal' is empty!"), __LINE__, __FUNCTION__))
	{
		return NAME_None;
	}
	return SaveGameDataInternal->GetSavedProgressionRowByIndex(0);
}

//  Returns a current save to disk row by name
const FPSSaveToDiskData& UPSWorldSubsystem::GetCurrentSaveToDiskRowByName() const
{
	if (!ensureMsgf(SaveGameDataInternal, TEXT("ASSERT: [%i] %hs:\n'SaveGameDataInternal' is empty!"), __LINE__, __FUNCTION__))
	{
		return FPSSaveToDiskData::EmptyData;
	}
	return SaveGameDataInternal->GetSaveToDiskDataByName(CurrentRowNameInternal);
}

// Returns a current progression row settings data row by name
const FPSRowData& UPSWorldSubsystem::GetCurrentProgressionSettingsRowByName() const
{
	if (const FPSRowData* FoundRow = ProgressionSettingsDataInternal.Find(CurrentRowNameInternal))
	{
		return *FoundRow;
	}

	return FPSRowData::EmptyData;
}

// Returns the current row data by name.
const FPSRowData& UPSWorldSubsystem::GetRowDataByName(FName CurrentRowName) const
{
	if (const FPSRowData* FoundRow = ProgressionSettingsDataInternal.Find(CurrentRowName))
	{
		return *FoundRow;
	}

	return FPSRowData::EmptyData;
}

// Set the progression system component
void UPSWorldSubsystem::SetHUDComponent(UPSHUDComponent* MyHUDComponent)
{
	if (!ensureMsgf(MyHUDComponent, TEXT("ASSERT: [%i] %hs:\n'MyHUDComponent' is null!"), __LINE__, __FUNCTION__))
	{
		return;
	}
	PSHUDComponentInternal = MyHUDComponent;
}

// Set the progression system spot component
void UPSWorldSubsystem::RegisterSpotComponent(UPSSpotComponent* MySpotComponent)
{
	if (!ensureMsgf(MySpotComponent, TEXT("ASSERT: [%i] %hs:\n'MyHUDComponent' is null!"), __LINE__, __FUNCTION__))
	{
		return;
	}
	PSSpotComponentArrayInternal.AddUnique(MySpotComponent);
}

// Called when progression module ready
void UPSWorldSubsystem::OnInitialized_Implementation()
{
	UMaterialInterface* StarMaterial = UPSDataAsset::Get().GetDynamicProgressionMaterial();

	if (!ensureMsgf(StarMaterial, TEXT("ASSERT: [%i] %hs:\n'StarMaterial' is null!"), __LINE__, __FUNCTION__))
	{
		return;
	}

	StarDynamicProgressMaterial = UMaterialInstanceDynamic::Create(StarMaterial, this);
	StarLockedProgressMaterial = UMaterialInstanceDynamic::Create(StarMaterial, this);
	StarUnLockedProgressMaterial = UMaterialInstanceDynamic::Create(StarMaterial, this);

	// Subscribe events on player type changed and Character spawned
	BIND_ON_LOCAL_CHARACTER_READY(this, ThisClass::OnLocalCharacterReady);

	// Binds the local player state ready event to the handler
	BIND_ON_LOCAL_PLAYER_STATE_READY(this, ThisClass::OnLocalPlayerStateReady);
}

// Called when world is ready to start gameplay before the game mode transitions to the correct state and call BeginPlay on all actors 
void UPSWorldSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
}

// Clears all transient data created by this subsystem
void UPSWorldSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

// Is called to initialize the world subsystem. It's a BeginPlay logic for the PS module
void UPSWorldSubsystem::OnWorldSubSystemInitialize_Implementation()
{
	// Load save game data of the Progression system
	FAsyncLoadGameFromSlot AsyncLoadGameFromSlotDelegate;
	AsyncLoadGameFromSlotDelegate.BindUObject(this, &ThisClass::OnAsyncLoadGameFromSlotCompleted);
	USaveUtilsLibrary::AsyncLoadGameFromSlot(this, UPSSaveGameData::GetSaveSlotName(SaveFileVersionExtensionInternal), UPSSaveGameData::GetSaveSlotIndex(), AsyncLoadGameFromSlotDelegate);
}

// Is called when a player character is ready
void UPSWorldSubsystem::OnLocalCharacterReady_Implementation(APlayerCharacter* PlayerCharacter, int32 CharacterID)
{
	if (!ensureMsgf(PlayerCharacter, TEXT("ASSERT: [%i] %hs:\n'PlayerCharacter' is not valid!"), __LINE__, __FUNCTION__))
	{
		return;
	}
	PlayerCharacter->OnPlayerTypeChanged.AddUniqueDynamic(this, &ThisClass::OnPlayerTypeChanged);
}

// Subscribes to the end game state change notification on the player state. 
void UPSWorldSubsystem::OnLocalPlayerStateReady_Implementation(AMyPlayerState* PlayerState, int32 CharacterID)
{
	checkf(PlayerState, TEXT("ERROR: [%i] %hs:\n'PlayerState' is null!"), __LINE__, __FUNCTION__);
	PlayerState->OnEndGameStateChanged.AddUniqueDynamic(this, &ThisClass::OnEndGameStateChanged);
}

// Is called when a player has been changed
void UPSWorldSubsystem::OnPlayerTypeChanged_Implementation(FPlayerTag PlayerTag)
{
	SetCurrentRowByTag(PlayerTag);
}

// Called when the end game state was changed to recalculate progression according to endgame (win, loss etc.) 
void UPSWorldSubsystem::OnEndGameStateChanged_Implementation(EEndGameState EndGameState)
{
	if (EndGameState != EEndGameState::None)
	{
		SavePoints(EndGameState);
	}
}

// Save the progression depends on EEndGameState. 
void UPSWorldSubsystem::SavePoints(EEndGameState EndGameState)
{
	if (!ensureMsgf(SaveGameDataInternal, TEXT("ASSERT: [%i] %hs:\n'SaveGameDataInternal' is null!"), __LINE__, __FUNCTION__))
	{
		return;
	}
	SaveGameDataInternal->SavePoints(EndGameState);
}

// Always set first levels as unlocked on begin play
void UPSWorldSubsystem::SetFirstElementAsCurrent()
{
	FName FirstSaveToDiskRow = GetFirstSaveToDiskRowName();

	// early return if first element is not valid
	if (!ensureMsgf(!FirstSaveToDiskRow.IsNone(), TEXT("ASSERT: [%i] %hs:\n'FirstSaveToDiskRow' is not valid!"), __LINE__, __FUNCTION__))
	{
		return;
	}
	if (!ensureMsgf(SaveGameDataInternal, TEXT("ASSERT: [%i] %hs:\n'SaveGameDataInternal' is not valid!"), __LINE__, __FUNCTION__))
	{
		return;
	}

	CurrentRowNameInternal = FirstSaveToDiskRow;
	SaveGameDataInternal->UnlockLevelByName(CurrentRowNameInternal);
	APlayerCharacter* PlayerCharacter = UMyBlueprintFunctionLibrary::GetLocalPlayerCharacter();

	if (!ensureMsgf(PlayerCharacter, TEXT("ASSERT: [%i] %hs:\n'PlayerCharacter' is not valid!"), __LINE__, __FUNCTION__))
	{
		return;
	}
	const FPlayerTag& PlayerTag = PlayerCharacter->GetPlayerTag();
	SetCurrentRowByTag(PlayerTag);
	SaveDataAsync();
}

// Spawn/add the stars actors for a spot
void UPSWorldSubsystem::UpdateProgressionStarActors()
{
	//Return to Pool Manager the list of handles which is not needed (if there are any) 

	if (!PoolActorHandlersInternal.IsEmpty())
	{
		UPoolManagerSubsystem::Get().ReturnToPoolArray(PoolActorHandlersInternal);
		PoolActorHandlersInternal.Empty();
	}
	// --- Prepare spawn request
	const TWeakObjectPtr<ThisClass> WeakThis = this;
	const FOnSpawnAllCallback OnTakeActorsFromPoolCompleted = [WeakThis](const TArray<FPoolObjectData>& CreatedObjects)
	{
		if (UPSWorldSubsystem* This = WeakThis.Get())
		{
			This->OnTakeActorsFromPoolCompleted(CreatedObjects);
		}
	};

	// --- Spawn actors
	const FPSRowData& CurrentSettingsRowData = GetCurrentProgressionSettingsRowByName();
	if (CurrentSettingsRowData.PointsToUnlock)
	{
		UPoolManagerSubsystem::Get().TakeFromPoolArray(PoolActorHandlersInternal, UPSDataAsset::Get().GetStarActorClass(), CurrentSettingsRowData.PointsToUnlock, OnTakeActorsFromPoolCompleted, ESpawnRequestPriority::High);
	}
}

// Dynamically adds Star actors which representing unlocked and locked progression above the character
void UPSWorldSubsystem::OnTakeActorsFromPoolCompleted(const TArray<FPoolObjectData>& CreatedObjects)
{
	const FPSRowData& CurrentSettingsRowData = GetCurrentProgressionSettingsRowByName();
	const FPSSaveToDiskData& CurrentSaveToDiskRowData = GetCurrentSaveToDiskRowByName();

	float CurrentAmountOfUnlocked = CurrentSaveToDiskRowData.CurrentLevelProgression;

	FVector PreviousActorLocation = FVector::Zero();

	// Setup spawned widget
	for (const FPoolObjectData& CreatedObject : CreatedObjects)
	{
		APSStarActor& SpawnedActor = CreatedObject.GetChecked<APSStarActor>();

		float StarAmount = FMath::Clamp(CurrentAmountOfUnlocked, 0.0f, 1.0f);
		if (CurrentAmountOfUnlocked > 0)
		{
			SpawnedActor.UpdateStarActorProgressMeshMaterial(StarAmount, EPSStarActorState::Unlocked);
		}
		else
		{
			SpawnedActor.UpdateStarActorProgressMeshMaterial(1, EPSStarActorState::Locked);
		}

		CurrentAmountOfUnlocked -= StarAmount;

		SpawnedActor.OnInitialized(PreviousActorLocation);
		PreviousActorLocation = SpawnedActor.GetActorLocation();
	}
}

// Returns current spot component returns null if spot is not found
UPSSpotComponent* UPSWorldSubsystem::GetCurrentSpot() const
{
	APlayerCharacter* PlayerCharacter = UMyBlueprintFunctionLibrary::GetLocalPlayerCharacter();
	if (!PlayerCharacter)
	{
		return nullptr;
	}

	const FPlayerTag& PlayerTag = PlayerCharacter->GetPlayerTag();
	if (PSSpotComponentArrayInternal.IsEmpty() || !PlayerTag.IsValid())
	{
		return nullptr;
	}

	for (UPSSpotComponent* SpotComponent : PSSpotComponentArrayInternal)
	{
		if (SpotComponent && SpotComponent->GetMeshChecked().GetPlayerTag() == PlayerTag)
		{
			return SpotComponent;
		}
	}
	return nullptr;
}

// Returns Progression Star Dynamic Material by state
UMaterialInstanceDynamic* UPSWorldSubsystem::GetStarProgressionDynamicMaterial(EPSStarActorState StarState)
{
	switch (StarState)
	{
	case EPSStarActorState::Locked:
		return StarLockedProgressMaterial;
	case EPSStarActorState::Unlocked:
		return StarUnLockedProgressMaterial;
	case EPSStarActorState::Partial:
		return StarDynamicProgressMaterial;
	default:
		return nullptr;
	}
}

// Is called from AsyncLoadGameFromSlot once Save Game is loaded, or null if it failed to load.
void UPSWorldSubsystem::OnAsyncLoadGameFromSlotCompleted_Implementation(USaveGame* SaveGame)
{
	// load from data table
	const UDataTable* ProgressionDataTable = UPSDataAsset::Get().GetProgressionDataTable();
	if (!ensureMsgf(ProgressionDataTable, TEXT("ASSERT: [%i] %hs:\n'ProgressionDataTable' is not valid!"), __LINE__, __FUNCTION__))
	{
		return;
	}
	UMyDataTable::GetRows(*ProgressionDataTable, ProgressionSettingsDataInternal);

	SaveGameDataInternal = Cast<UPSSaveGameData>(SaveGame);

	if (!SaveGameDataInternal)
	{
		//  there is no save game file, or it is corrupted, create a new one
		SaveGameDataInternal = Cast<UPSSaveGameData>(UGameplayStatics::CreateSaveGameObject(UPSSaveGameData::StaticClass()));

		if (SaveGameDataInternal)
		{
			for (const TTuple<FName, FPSRowData>& Row : ProgressionSettingsDataInternal)
			{
				SaveGameDataInternal->SetProgressionMap(Row.Key, FPSSaveToDiskData::EmptyData);
			}
		}
	}

	SetFirstElementAsCurrent();
	OnInitialized();
	OnInitialize.Broadcast();
}

// Destroy all star actors that should not be available by other objects anymore.
void UPSWorldSubsystem::PerformCleanUp()
{
	// Destroying Star Actors
	if (!PoolActorHandlersInternal.IsEmpty())
	{
		UPoolManagerSubsystem::Get().ReturnToPoolArray(PoolActorHandlersInternal);
		PoolActorHandlersInternal.Empty();
		UPoolManagerSubsystem::Get().EmptyPool(UPSDataAsset::Get().GetStarActorClass());
	}

	ProgressionSettingsDataInternal.Empty();
	StarDynamicProgressMaterial = nullptr;

	// Subsystem clean up  
	UMyPrimaryDataAsset::ResetDataAsset(PSDataAssetInternal);
	PSHUDComponentInternal = nullptr;
	PSSpotComponentArrayInternal.Empty();

	// Saves clean up 
	if (SaveGameDataInternal)
	{
		SaveGameDataInternal->ConditionalBeginDestroy();
		SaveGameDataInternal = nullptr;
	}
}

// Saves the progression to the local files
void UPSWorldSubsystem::SaveDataAsync()
{
	if (!ensureMsgf(SaveGameDataInternal, TEXT("ASSERT: [%i] %hs:\n'SaveGameDataInternal' is null!"), __LINE__, __FUNCTION__))
	{
		return;
	}
	const FPSSaveToDiskData& CurrenSaveToDiskDataRow = GetCurrentSaveToDiskRowByName();
	const FPSRowData& CurrenProgressionSettingsRow = GetCurrentProgressionSettingsRowByName();

	UpdateProgressionStarActors();
	OnCurrentScoreChanged.Broadcast(CurrenSaveToDiskDataRow, CurrenProgressionSettingsRow);

	UGameplayStatics::AsyncSaveGameToSlot(SaveGameDataInternal, UPSSaveGameData::GetSaveSlotName(SaveFileVersionExtensionInternal), SaveGameDataInternal->GetSaveSlotIndex());
}

// Removes all saved data of the Progression system and creates a new empty data
void UPSWorldSubsystem::ResetSaveGameData()
{
	const FString& SlotName = UPSSaveGameData::GetSaveSlotName(SaveFileVersionExtensionInternal);
	const int32 UserIndex = UPSSaveGameData::GetSaveSlotIndex();

	SaveGameDataInternal = USaveUtilsLibrary::ResetSaveGameData<UPSSaveGameData>(SaveGameDataInternal, SlotName, UserIndex);
	checkf(SaveGameDataInternal, TEXT("ERROR: [%i] %hs:\n'SaveGameDataInternal' is null!"), __LINE__, __FUNCTION__);

	// load from data table
	const UDataTable* ProgressionDataTable = UPSDataAsset::Get().GetProgressionDataTable();
	if (!ensureMsgf(ProgressionDataTable, TEXT("ASSERT: [%i] %hs:\n'ProgressionDataTable' is not valid!"), __LINE__, __FUNCTION__))
	{
		return;
	}
	UMyDataTable::GetRows(*ProgressionDataTable, ProgressionSettingsDataInternal);

	for (const TTuple<FName, FPSRowData>& Row : ProgressionSettingsDataInternal)
	{
		SaveGameDataInternal->SetProgressionMap(Row.Key, FPSSaveToDiskData::EmptyData);
	}

	OnReset.Broadcast();
	// Re-load save game object. Load game from save file or if there is no such creates a new one
	SetFirstElementAsCurrent();
}

// Unlocks all levels of the Progression System
void UPSWorldSubsystem::UnlockAllLevels()
{
	if (!ensureMsgf(SaveGameDataInternal, TEXT("ASSERT: [%i] %hs:\n'SaveGameDataInternal' is not valid!"), __LINE__, __FUNCTION__))
	{
		return;
	}
	SaveGameDataInternal->UnlockAllLevels();

	APlayerCharacter* PlayerCharacter = UMyBlueprintFunctionLibrary::GetLocalPlayerCharacter();

	if (!ensureMsgf(PlayerCharacter, TEXT("ASSERT: [%i] %hs:\n'PlayerCharacter' is not valid!"), __LINE__, __FUNCTION__))
	{
		return;
	}
	const FPlayerTag& PlayerTag = PlayerCharacter->GetPlayerTag();
	SetCurrentRowByTag(PlayerTag);
	SaveDataAsync();
}
