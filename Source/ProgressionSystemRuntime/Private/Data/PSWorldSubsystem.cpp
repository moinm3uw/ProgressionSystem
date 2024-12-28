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
#include "GameFramework/MyGameStateBase.h"
#include "LevelActors/PSStarActor.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "MyUtilsLibraries/GameplayUtilsLibrary.h"
#include "Subsystems/GameDifficultySubsystem.h"
#include "Subsystems/GlobalEventsSubsystem.h"
#include "Materials/MaterialInstanceDynamic.h"
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
	return UMyPrimaryDataAsset::GetOrLoadOnce(PSDataAssetInternal);
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
}

// Called when progression module ready
void UPSWorldSubsystem::OnInitialized_Implementation()
{
	StarDynamicProgressMaterial = UMaterialInstanceDynamic::Create(UPSDataAsset::Get().GetDynamicProgressionMaterial(), this);
	if (!ensureMsgf(StarDynamicProgressMaterial, TEXT("ASSERT: [%i] %hs:\n'StarDynamicProgressMaterial' is null!"), __LINE__, __FUNCTION__))
	{
		return;
	}

	// Subscribe events on player type changed and Character spawned
	BIND_ON_LOCAL_CHARACTER_READY(this, ThisClass::OnLocalCharacterReady);

	// Listen to handle input for each game state
	BIND_ON_GAME_STATE_CHANGED(this, ThisClass::OnGameStateChanged);
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
	FAsyncLoadGameFromSlotDelegate AsyncLoadGameFromSlotDelegate;
	AsyncLoadGameFromSlotDelegate.BindUObject(this, &ThisClass::OnAsyncLoadGameFromSlotCompleted);
	UGameplayStatics::AsyncLoadGameFromSlot(UPSSaveGameData::GetSaveSlotName(), UPSSaveGameData::GetSaveSlotIndex(), AsyncLoadGameFromSlotDelegate);
}

// Is called when a player character is ready
void UPSWorldSubsystem::OnLocalCharacterReady_Implementation(APlayerCharacter* PlayerCharacter, int32 CharacterID)
{
	if (!ensureMsgf(PlayerCharacter, TEXT("ASSERT: [%i] %s:\n'PlayerCharacter' is not valid!"), __LINE__, *FString(__FUNCTION__)))
	{
		return;
	}
	PlayerCharacter->OnPlayerTypeChanged.AddUniqueDynamic(this, &ThisClass::OnPlayerTypeChanged);
}

// Is called when a player has been changed
void UPSWorldSubsystem::OnPlayerTypeChanged_Implementation(FPlayerTag PlayerTag)
{
	// todo refactor: in SetCurrentRowByTag function on broadcast OnCurrentRowDataChanged create a function which will
	// perform all logic after this function call
	SetCurrentRowByTag(PlayerTag);

	for (UPSSpotComponent* SpotComponent : PSSpotComponentArrayInternal)
	{
		if (SpotComponent->GetMeshChecked().GetPlayerTag() == PlayerTag)
		{
			PSCurrentSpotComponentInternal = SpotComponent;
			UpdateProgressionStarActors();
		}
	}
}

// Called when the current game state was changed
void UPSWorldSubsystem::OnGameStateChanged_Implementation(ECurrentGameState CurrentGameState)
{
	switch (CurrentGameState)
	{
	case ECurrentGameState::Menu:
		// refresh 3D Stars actors
		UpdateProgressionStarActors();
		break;
	case ECurrentGameState::GameStarting:
		// Show Progression Menu widget in Main Menu
		break;
	default:
		break;
	}
}

// Always set first levels as unlocked on begin play
void UPSWorldSubsystem::SetFirstElementAsCurrent()
{
	FName FirstSaveToDiskRow = GetFirstSaveToDiskRowName();
	
	// early return if first element is not valid
	if (!ensureMsgf(!FirstSaveToDiskRow.IsNone(), TEXT("ASSERT: [%i] %s:\n'FirstSaveToDiskRow' is not valid!"), __LINE__, *FString(__FUNCTION__)))
	{
		return;
	}
	if (!ensureMsgf(SaveGameDataInternal, TEXT("ASSERT: [%i] %s:\n'SaveGameDataInternal' is not valid!"), __LINE__, *FString(__FUNCTION__)))
	{
		return;
	}

	CurrentRowNameInternal = FirstSaveToDiskRow;
	SaveGameDataInternal->UnlockLevelByName(CurrentRowNameInternal);
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
			SpawnedActor.UpdateStarActorMeshMaterial(StarDynamicProgressMaterial, StarAmount, EPSStarActorState::Unlocked);
		}
		else
		{
			SpawnedActor.UpdateStarActorMeshMaterial(StarDynamicProgressMaterial, 1, EPSStarActorState::Locked);
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

// Triggers when a spot is loaded
void UPSWorldSubsystem::OnSpotComponentLoad_Implementation(UPSSpotComponent* SpotComponent)
{
	if (!ensureMsgf(SpotComponent, TEXT("ASSERT: [%i] %s:\n'SpotComponent' is not valid!"), __LINE__, *FString(__FUNCTION__)))
	{
		return;
	}

	PSCurrentSpotComponentInternal = SpotComponent;
}

// Is called from AsyncLoadGameFromSlot once Save Game is loaded, or null if it failed to load.
void UPSWorldSubsystem::OnAsyncLoadGameFromSlotCompleted_Implementation(const FString& SlotName, int32 UserIndex, USaveGame* SaveGame)
{
	// load from data table
	const UDataTable* ProgressionDataTable = UPSDataAsset::Get().GetProgressionDataTable();
	if (!ensureMsgf(ProgressionDataTable, TEXT("ASSERT: [%i] %s:\n'ProgressionDataTable' is not valid!"), __LINE__, *FString(__FUNCTION__)))
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
	PSCurrentSpotComponentInternal = nullptr;

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

	UGameplayStatics::AsyncSaveGameToSlot(SaveGameDataInternal, UPSSaveGameData::GetSaveSlotName(), SaveGameDataInternal->GetSaveSlotIndex());
}

// Removes all saved data of the Progression system and creates a new empty data
void UPSWorldSubsystem::ResetSaveGameData()
{
	const FString& SlotName = UPSSaveGameData::GetSaveSlotName();
	const int32 UserIndex = UPSSaveGameData::GetSaveSlotIndex();

	SaveGameDataInternal = Cast<UPSSaveGameData>(UGameplayUtilsLibrary::ResetSaveGameData(SaveGameDataInternal, SlotName, UserIndex));
	checkf(SaveGameDataInternal, TEXT("ERROR: [%i] %hs:\n'SaveGameDataInternal' is null!"), __LINE__, __FUNCTION__);

	// load from data table
	const UDataTable* ProgressionDataTable = UPSDataAsset::Get().GetProgressionDataTable();
	if (!ensureMsgf(ProgressionDataTable, TEXT("ASSERT: [%i] %s:\n'ProgressionDataTable' is not valid!"), __LINE__, *FString(__FUNCTION__)))
	{
		return;
	}
	UMyDataTable::GetRows(*ProgressionDataTable, ProgressionSettingsDataInternal);

	for (const TTuple<FName, FPSRowData>& Row : ProgressionSettingsDataInternal)
	{
		SaveGameDataInternal->SetProgressionMap(Row.Key, FPSSaveToDiskData::EmptyData);
	}

	// Re-load save game object. Load game from save file or if there is no such creates a new one
	SetFirstElementAsCurrent();
	UpdateProgressionUI();

	const APlayerCharacter* LocalCharacter = UMyBlueprintFunctionLibrary::GetLocalPlayerCharacter();
	if (!ensureMsgf(LocalCharacter, TEXT("ASSERT: [%i] %hs:\n'SpotComponent' is null!"), __LINE__, __FUNCTION__))
	{
		return;
	}
	SetCurrentRowByTag(LocalCharacter->GetPlayerTag());
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
	UpdateProgressionUI();
}

// Returns difficultyMultiplier
float UPSWorldSubsystem::GetDifficultyMultiplier() const
{
	const TMap<EGameDifficulty, float>& DifficultyMap = UPSDataAsset::Get().GetProgressionDifficultyMultiplier();
	constexpr float DefaultDifficulty = 0.f;
	if (!ensureMsgf(!DifficultyMap.IsEmpty(), TEXT("ASSERT: [%i] %s:\n'DifficultyMap' is empty!"), __LINE__, *FString(__FUNCTION__)))
	{
		return DefaultDifficulty;
	}
	const float* FoundDifficulty = DifficultyMap.Find(UGameDifficultySubsystem::Get().GetDifficultyType());
	if (!FoundDifficulty)
	{
		// No difficulty found, try to apply Any scenario
		FoundDifficulty = DifficultyMap.Find(EGameDifficulty::Any);
	}

	return FoundDifficulty ? *FoundDifficulty : DefaultDifficulty;
}

void UPSWorldSubsystem::UpdateProgressionUI()
{
	UPSHUDComponent* PSHUDComponent = GetProgressionSystemHUDComponent();
	if (!ensureMsgf(PSHUDComponent, TEXT("ASSERT: [%i] %hs:\n'PSHUDComponent' is null!"), __LINE__, __FUNCTION__))
	{
		return;
	}

	UPSSpotComponent* SpotComponent = GetCurrentSpot();
	if (!ensureMsgf(SpotComponent, TEXT("ASSERT: [%i] %hs:\n'SpotComponent' is null!"), __LINE__, __FUNCTION__))
	{
		return;
	}

	SpotComponent->ChangeSpotVisibilityStatus();
	PSHUDComponent->UpdateProgressionWidgetForPlayer();
	UpdateProgressionStarActors();
}
