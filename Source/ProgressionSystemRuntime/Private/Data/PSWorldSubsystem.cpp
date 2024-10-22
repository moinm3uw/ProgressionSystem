// Copyright (c) Valerii Rotermel & Yevhenii Selivanov

#include "Data/PSWorldSubsystem.h"

#include "PoolManagerSubsystem.h"
#include "Components/MySkeletalMeshComponent.h"
#include "Components/PSHUDComponent.h"
#include "Components/PSSpotComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Data/PSDataAsset.h"
#include "Data/PSSaveGameData.h"
#include "DataAssets/GameStateDataAsset.h"
#include "Kismet/GameplayStatics.h"
#include "LevelActors/PlayerCharacter.h"
#include "MyDataTable/MyDataTable.h"
#include "MyUtilsLibraries/UtilsLibrary.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/MyGameStateBase.h"
#include "LevelActors/PSStarActor.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "MyUtilsLibraries/GameplayUtilsLibrary.h"
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
			OnCurrentRowDataChanged.Broadcast(NewRowPlayerTag);
			return; // Exit immediately after finding the match
		}
	}
}


// Returns the data asset that contains all the assets of Progression System game feature
const UPSDataAsset* UPSWorldSubsystem::GetPSDataAsset() const
{
	if (!ensureMsgf(!PSDataAssetInternal.IsNull(), TEXT("ASSERT: [%i] %s:\n'PSDataAssetInternal' is empty!"), __LINE__, *FString(__FUNCTION__)))
	{
		return nullptr;
	}
	return PSDataAssetInternal.LoadSynchronous();
}

//  Returns a current save to disk row name
FName UPSWorldSubsystem::GetFirstSaveToDiskRowName() const
{
	if (!ensureMsgf(SaveGameDataInternal, TEXT("ASSERT: [%i] %s:\n'SaveGameDataInternal' is empty!"), __LINE__, *FString(__FUNCTION__)))
	{
		return NAME_None;
	}
	return SaveGameDataInternal->GetSavedProgressionRowByIndex(0);
}

//  Returns a current save to disk row by name
const FPSSaveToDiskData& UPSWorldSubsystem::GetCurrentSaveToDiskRowByName() const
{
	if (!ensureMsgf(SaveGameDataInternal, TEXT("ASSERT: [%i] %s:\n'SaveGameDataInternal' is empty!"), __LINE__, *FString(__FUNCTION__)))
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
void UPSWorldSubsystem::RegisterSpotComponent(UPSSpotComponent* MyHUDComponent)
{
	if (!ensureMsgf(MyHUDComponent, TEXT("ASSERT: [%i] %hs:\n'MyHUDComponent' is null!"), __LINE__, __FUNCTION__))
	{
		return;
	}
	PSSpotComponentArrayInternal.AddUnique(MyHUDComponent);
	MyHUDComponent->OnSpotComponentReady.AddDynamic(this, &UPSWorldSubsystem::OnSpotComponentLoad);
}

void UPSWorldSubsystem::SetCurrentSpotComponent(UPSSpotComponent* MyHUDComponent)
{
	if (!ensureMsgf(MyHUDComponent, TEXT("ASSERT: [%i] %hs:\n'MyHUDComponent' is null!"), __LINE__, __FUNCTION__))
	{
		return;
	}
	PSCurrentSpotComponentInternal = MyHUDComponent;
	UpdateProgressionActorsForSpot();
}

// Called when world is ready to start gameplay before the game mode transitions to the correct state and call BeginPlay on all actors 
void UPSWorldSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	const TArray<FName>& GameFeaturesToEnable = UGameStateDataAsset::Get().GetGameFeaturesToEnable();
	for (const FName GameFeatureName : GameFeaturesToEnable)
	{
		if (GameFeatureName.IsNone())
		{
			continue;
		}

		if (GameFeatureName == "ProgressionSystem")
		{
			// Subscribe events on player type changed and Character spawned
			BIND_ON_LOCAL_CHARACTER_READY(this, ThisClass::OnCharacterReady);

			// Listen to handle input for each game state
			BIND_ON_GAME_STATE_CHANGED(this, ThisClass::OnGameStateChanged);

			LoadGameFromSave();

			StarDynamicProgressMaterial = UMaterialInstanceDynamic::Create(UPSDataAsset::Get().GetDynamicProgressionMaterial(), this);
			if (!ensureMsgf(StarDynamicProgressMaterial, TEXT("ASSERT: [%i] %hs:\n'StarDynamicProgressMaterial' is null!"), __LINE__, __FUNCTION__))
			{
				return;
			}
		}
	}
}

// Clears all transient data created by this subsystem
void UPSWorldSubsystem::Deinitialize()
{
	Super::Deinitialize();
	PSHUDComponentInternal = nullptr;
	PSCurrentSpotComponentInternal = nullptr;
	SaveGameDataInternal = nullptr;
	ProgressionSettingsDataInternal.Empty();
	StarDynamicProgressMaterial = nullptr;
}

// Is called when a player character is ready
void UPSWorldSubsystem::OnCharacterReady(APlayerCharacter* PlayerCharacter, int32 CharacterID)
{
	if (!ensureMsgf(PlayerCharacter, TEXT("ASSERT: [%i] %s:\n'PlayerCharacter' is not valid!"), __LINE__, *FString(__FUNCTION__)))
	{
		return;
	}
	PlayerCharacter->OnPlayerTypeChanged.AddUniqueDynamic(this, &ThisClass::OnPlayerTypeChanged);
}

// Is called when a player has been changed
void UPSWorldSubsystem::OnPlayerTypeChanged(FPlayerTag PlayerTag)
{
	SetCurrentRowByTag(PlayerTag);

	for (UPSSpotComponent* SpotComponent : PSSpotComponentArrayInternal)
	{
		if (SpotComponent->GetMeshChecked().GetPlayerTag() == PlayerTag)
		{
			PSCurrentSpotComponentInternal = SpotComponent;
			UpdateProgressionActorsForSpot();
		}
	}
}

// Called when the current game state was changed
void UPSWorldSubsystem::OnGameStateChanged(ECurrentGameState CurrentGameState)
{
	switch (CurrentGameState)
	{
	case ECurrentGameState::Menu:
		// refresh 3D Stars actors
		UpdateProgressionActorsForSpot();
		break;
	case ECurrentGameState::GameStarting:
		// Show Progression Menu widget in Main Menu
		break;
	default:
		return;
	}
}

// Load game from save file or create a new one (does initial load from data table)
void UPSWorldSubsystem::LoadGameFromSave()
{
	// load from data table
	const UDataTable* ProgressionDataTable = UPSDataAsset::Get().GetProgressionDataTable();
	checkf(ProgressionDataTable, TEXT("ERROR: 'ProgressionDataTableInternal' is null"));
	UMyDataTable::GetRows(*ProgressionDataTable, ProgressionSettingsDataInternal);

	// Check if the save game file exists
	if (UGameplayStatics::DoesSaveGameExist(SaveGameDataInternal->GetSaveSlotName(), SaveGameDataInternal->GetSaveSlotIndex()))
	{
		SaveGameDataInternal = Cast<UPSSaveGameData>(UGameplayStatics::LoadGameFromSlot(SaveGameDataInternal->GetSaveSlotName(), SaveGameDataInternal->GetSaveSlotIndex()));
	}
	else
	{
		// Save file does not exist
		// do initial load from data table

		SaveGameDataInternal = Cast<UPSSaveGameData>(UGameplayStatics::CreateSaveGameObject(UPSSaveGameData::StaticClass()));

		if (SaveGameDataInternal)
		{
			for (TTuple<FName, FPSRowData> Row : ProgressionSettingsDataInternal)
			{
				SaveGameDataInternal->SetProgressionMap(Row.Key, FPSSaveToDiskData::EmptyData);
			}
		}
	}
	SetFirstElementAsCurrent();
}

// Always set first levels as unlocked on begin play
void UPSWorldSubsystem::SetFirstElementAsCurrent()
{
	FName FirstSaveToDiskRow = GetFirstSaveToDiskRowName();
	if (!FirstSaveToDiskRow.IsNone())
	{
		CurrentRowNameInternal = FirstSaveToDiskRow;
		SaveGameDataInternal->UnlockLevelByName(CurrentRowNameInternal);
	}
	SaveDataAsync();
}

// Updates the stars actors for a spot
void UPSWorldSubsystem::UpdateProgressionActorsForSpot()
{
	AddProgressionStarActors();
}

// Spawn/add the stars actors for a spot
void UPSWorldSubsystem::AddProgressionStarActors()
{
	const FPSRowData& CurrentSettingsRowData = GetCurrentProgressionSettingsRowByName();
	//Return to Pool Manager the list of handles which is not needed (if there are any) 
	UPoolManagerSubsystem::Get().ReturnToPoolArray(PoolActorHandlersInternal);
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
	UPoolManagerSubsystem::Get().TakeFromPoolArray(PoolActorHandlersInternal, UPSDataAsset::Get().GetStarActorClass(), CurrentSettingsRowData.PointsToUnlock, OnTakeActorsFromPoolCompleted, ESpawnRequestPriority::High);
}

// Dynamically adds Star actors which representing unlocked and locked progression above the character
void UPSWorldSubsystem::OnTakeActorsFromPoolCompleted(const TArray<FPoolObjectData>& CreatedObjects)
{
	const FPSRowData& CurrentSettingsRowData = GetCurrentProgressionSettingsRowByName();
	const FPSSaveToDiskData& CurrentSaveToDiskRowData = GetCurrentSaveToDiskRowByName();

	float CurrentAmountOfUnlocked = CurrentSaveToDiskRowData.CurrentLevelProgression;

	FVector PreviousActorLocation;

	// Setup spawned widget
	for (const FPoolObjectData& CreatedObject : CreatedObjects)
	{
		APSStarActor& SpawnedActor = CreatedObject.GetChecked<APSStarActor>();

		float StarAmount = FMath::Clamp(CurrentAmountOfUnlocked, 0.0f, 1.0f);
		if (CurrentAmountOfUnlocked > 0)
		{
			constexpr bool bIsStarLocked = false;
			SpawnedActor.UpdateStarActorMeshMaterial(StarDynamicProgressMaterial, StarAmount, bIsStarLocked);
		}
		else
		{
			constexpr bool bIsStarLocked = true;
			SpawnedActor.UpdateStarActorMeshMaterial(StarDynamicProgressMaterial, 1, bIsStarLocked);
		}
		CurrentAmountOfUnlocked -= StarAmount;

		SpawnedActor.OnInitialized(PreviousActorLocation);
		PreviousActorLocation = SpawnedActor.GetActorLocation();
	}
}

// Triggers when a spot is loaded
void UPSWorldSubsystem::OnSpotComponentLoad(UPSSpotComponent* SpotComponent)
{
	if (!ensureMsgf(SpotComponent, TEXT("ASSERT: [%i] %s:\n'SpotComponent' is not valid!"), __LINE__, *FString(__FUNCTION__)))
	{
		return;
	}

	PSCurrentSpotComponentInternal = SpotComponent;
}

// Saves the progression to the local files
void UPSWorldSubsystem::SaveDataAsync() const
{
	if (!ensureMsgf(SaveGameDataInternal, TEXT("ASSERT: [%i] %hs:\n'SaveGameDataInternal' is null!"), __LINE__, __FUNCTION__))
	{
		return;
	}
	UGameplayStatics::AsyncSaveGameToSlot(SaveGameDataInternal, SaveGameDataInternal->GetSaveSlotName(), SaveGameDataInternal->GetSaveSlotIndex());
}

// Removes all saved data of the Progression system and creates a new empty data
void UPSWorldSubsystem::ResetSaveGameData()
{
	const FString& SlotName = SaveGameDataInternal->GetSaveSlotName();
	const int32 UserIndex = SaveGameDataInternal->GetSaveSlotIndex();

	UGameplayUtilsLibrary::ResetSaveGameData(SaveGameDataInternal, SlotName, UserIndex);

	// Re-load a new save game object. Load game from save creates a save file if there is no such
	LoadGameFromSave();
	UPSHUDComponent* PSHUDComponent = GetProgressionSystemHUDComponent();
	if (!ensureMsgf(PSHUDComponent, TEXT("ASSERT: [%i] %hs:\n'PSHUDComponent' is null!"), __LINE__, __FUNCTION__))
	{
		return;
	}
	PSHUDComponent->UpdateProgressionWidgetForPlayer();
	UpdateProgressionActorsForSpot();
}

// Unlocks all levels of the Progression System
void UPSWorldSubsystem::UnlockAllLevels()
{
	if (!ensureMsgf(SaveGameDataInternal, TEXT("ASSERT: [%i] %s:\n'SaveGameDataInternal' is not valid!"), __LINE__, *FString(__FUNCTION__)))
	{
		return;
	}
	SaveGameDataInternal->UnlockAllLevels();
	SaveDataAsync();
	UPSHUDComponent* PSHUDComponent = GetProgressionSystemHUDComponent();
	if (!ensureMsgf(PSHUDComponent, TEXT("ASSERT: [%i] %hs:\n'PSHUDComponent' is null!"), __LINE__, __FUNCTION__))
	{
		return;
	}
	PSHUDComponent->UpdateProgressionWidgetForPlayer();
	UpdateProgressionActorsForSpot();
}

// Returns difficultyMultiplier
float UPSWorldSubsystem::GetDifficultyMultiplier()
{
	TMap<EGameDifficulty, float> DifficultyMap = UPSDataAsset::Get().GetProgressionDifficultyMultiplier();
	if (!ensureMsgf(DifficultyMap.IsEmpty(), TEXT("ASSERT: [%i] %s:\n'DifficultyMap' is empty!"), __LINE__, *FString(__FUNCTION__)))
	{
		return 1.0f;
	}

	switch (UGameDifficultySubsystem::GetGameDifficultySubsystem()->GetDifficultyType())
	{
	case EGameDifficulty::Easy:
		return *DifficultyMap.Find(EGameDifficulty::Easy);
	case EGameDifficulty::Normal:
		return *DifficultyMap.Find(EGameDifficulty::Normal);
	case EGameDifficulty::Hard:
		return *DifficultyMap.Find(EGameDifficulty::Hard);
	case EGameDifficulty::Vanilla:
		return 1.0f;
	default:
		return 1.0f;
	}
}
