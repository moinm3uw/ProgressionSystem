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
#include "Materials/MaterialInstanceDynamic.h"
#include "MyUtilsLibraries/GameplayUtilsLibrary.h"
#include "Subsystems/GameDifficultySubsystem.h"
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

// Set the progression system spot component
void UPSWorldSubsystem::RegisterSpotComponent(UPSSpotComponent* MyHUDComponent)
{
	checkf(MyHUDComponent, TEXT("%s: My progression system component is null"), *FString(__FUNCTION__));
	PSSpotComponentArrayInternal.AddUnique(MyHUDComponent);
	MyHUDComponent->OnSpotComponentReady.AddDynamic(this, &UPSWorldSubsystem::OnSpotComponentLoad);
}

void UPSWorldSubsystem::SetCurrentSpotComponent(UPSSpotComponent* MyHUDComponent)
{
	checkf(MyHUDComponent, TEXT("%s: My progression system component is null"), *FString(__FUNCTION__));
	PSSpotComponentInternal = MyHUDComponent;
	UpdateProgressionActorsForSpot();
}

// Called when world is ready to start gameplay before the game mode transitions to the correct state and call BeginPlay on all actors 
void UPSWorldSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	// Subscribe events on player type changed and Character spawned
	BIND_ON_LOCAL_CHARACTER_READY(this, ThisClass::OnCharacterReady);

	// Listen to handle input for each game state
	BIND_ON_GAME_STATE_CHANGED(this, ThisClass::OnGameStateChanged);

	LoadGameFromSave();

	StarDynamicProgressMaterial = UMaterialInstanceDynamic::Create(UPSDataAsset::Get().GetDynamicProgressionMaterial(), this);
	checkf(StarDynamicProgressMaterial, TEXT("ERROR: 'StarDynamicProgressMaterial' is null"));
}

// Clears all transient data created by this subsystem
void UPSWorldSubsystem::Deinitialize()
{
	Super::Deinitialize();
	PSHUDComponentInternal = nullptr;
	PSSpotComponentInternal = nullptr;
	SaveGameInstanceInternal = nullptr;
	SpawnedStarActorsInternal.Empty();
	StarDynamicProgressMaterial = nullptr;
}

// Is called when a player has been changed
void UPSWorldSubsystem::OnPlayerTypeChanged(FPlayerTag PlayerTag)
{
	SaveGameInstanceInternal->SetRowByTag(PlayerTag);
	OnCurrentRowDataChanged.Broadcast(PlayerTag);

	for (UPSSpotComponent* SpotComponent : PSSpotComponentArrayInternal)
	{
		if (SpotComponent->GetMeshChecked().GetPlayerTag() == PlayerTag)
		{
			PSSpotComponentInternal = SpotComponent;
		}
	}
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

// Updates the stars actors for a spot
void UPSWorldSubsystem::UpdateProgressionActorsForSpot()
{
	const FPSRowData& CurrentRowData = GetCurrentRow();

	//set updated amount of stars
	if (CurrentRowData.CurrentLevelProgression >= CurrentRowData.PointsToUnlock)
	{
		// set required points (stars)  to achieve for a level  
		AddProgressionStarActors(CurrentRowData.PointsToUnlock, 0, CurrentRowData.PointsToUnlock);
	}
	else
	{
		// Calculate the unlocked against locked points (stars) 
		AddProgressionStarActors(CurrentRowData.CurrentLevelProgression, CurrentRowData.PointsToUnlock - CurrentRowData.CurrentLevelProgression, CurrentRowData.PointsToUnlock); // Listen game state changes events 
	}
}

// Spawn/add the stars actors for a spot
void UPSWorldSubsystem::AddProgressionStarActors(float AmountOfUnlockedPoints, float AmountOfLockedPoints, float MaxLevelPoints)
{
	SpawnedStarActorsInternal.Empty();
	//Return to Pool Manager the list of handles which is not needed (if there are any) 
	UPoolManagerSubsystem::Get().ReturnToPoolArray(PoolActorHandlersInternal);
	SpawnedStarActorsInternal.Empty();
	// --- Prepare spawn request
	const TWeakObjectPtr<ThisClass> WeakThis = this;
	const FOnSpawnAllCallback OnTakeActorsFromPoolCompleted = [WeakThis, AmountOfUnlockedPoints, AmountOfLockedPoints, MaxLevelPoints](const TArray<FPoolObjectData>& CreatedObjects)
	{
		if (UPSWorldSubsystem* This = WeakThis.Get())
		{
			This->OnTakeActorsFromPoolCompleted(CreatedObjects, AmountOfUnlockedPoints, AmountOfLockedPoints, MaxLevelPoints);
		}
	};

	// --- Spawn actors
	const int32 TotalRequests = AmountOfLockedPoints + AmountOfUnlockedPoints;
	UPoolManagerSubsystem::Get().TakeFromPoolArray(PoolActorHandlersInternal, UPSDataAsset::Get().GetStarActorClass(), TotalRequests, OnTakeActorsFromPoolCompleted);
}

// Dynamically adds Star actors which representing unlocked and locked progression above the character
void UPSWorldSubsystem::OnTakeActorsFromPoolCompleted(const TArray<FPoolObjectData>& CreatedObjects, float AmountOfUnlockedPoints, float AmountOfLockedPoints, float MaxLevelPoints)
{
	// remove CurrentAmountOfUnlocked, CurrentAmountOfLocked use AmountOfUnlockedPoints, AmountOfLockedPoints from function
	float CurrentAmountOfUnlocked = AmountOfUnlockedPoints;
	float CurrentAmountOfLocked = AmountOfLockedPoints;
	float integerPart;
	float fractionalPart;

	// Setup spawned widget
	for (const FPoolObjectData& CreatedObject : CreatedObjects)
	{
		if (CurrentAmountOfUnlocked > 0)
		{
			fractionalPart = modff(CurrentAmountOfUnlocked, &integerPart);
			// check if it has a fractional part and if there is no more fully achieved stars 
			if (fractionalPart < 1.0f && CurrentAmountOfUnlocked < 1.0f)
			{
				UpdateStarActor(CreatedObject, fractionalPart, 0);
			}
			else
			{
				UpdateStarActor(CreatedObject, 1, 0);
			}

			CurrentAmountOfUnlocked--;
			continue;
		}
		
		if (CurrentAmountOfLocked > 0)
		{
			UpdateStarActor(CreatedObject, 0, 1);
			CurrentAmountOfLocked--;
		}
	}
}

// Updates star actor to locked/unlocked according to input amounnt
void UPSWorldSubsystem::UpdateStarActor(const FPoolObjectData& CreatedData, float AmountOfUnlockedStars, float AmountOfLockedStars)
{
	AActor& SpawnedActor = CreatedData.GetChecked<AActor>();
	UStaticMeshComponent* MeshComponent = SpawnedActor.FindComponentByClass<UStaticMeshComponent>();

	const FPSRowData& CurrentRowData = GetCurrentRow();

	SpawnedStarActorsInternal.Add(&SpawnedActor);

	//SpawnedActor.SetActorScale3D(CurrentRowData.StarActorTransform.GetScale3D());
	SpawnedActor.SetActorTransform(CurrentRowData.StarActorTransform);
	// if the actor is the first element it set initial position
	// from the initial position there is a distance between stars 
	if (SpawnedStarActorsInternal.Num() == 1)
	{
		//SpawnedActor.SetActorTransform(CurrentRowData.StarActorTransform);
	}
	else if (SpawnedStarActorsInternal.Num() > 1)
	{
		SpawnedActor.SetActorLocation(SpawnedStarActorsInternal[SpawnedStarActorsInternal.Num() - 2]->GetActorLocation() + CurrentRowData.OffsetBetweenStarActors);
	}

	if (AmountOfUnlockedStars > 0) //unlocked stars
	{
		if (AmountOfUnlockedStars > 0 && AmountOfUnlockedStars < 1) // dynamic 
		{
			MeshComponent->SetMaterial(0, StarDynamicProgressMaterial);
			StarDynamicProgressMaterial->SetScalarParameterValue(TEXT("Percentage2"), AmountOfUnlockedStars / 3);
		}
		else
		{
			MeshComponent->SetMaterial(0, UPSDataAsset::Get().GetUnlockedProgressionMaterial());
		}
		return;
	}

	if (AmountOfLockedStars > 0) //locked stars
	{
		MeshComponent->SetMaterial(0, UPSDataAsset::Get().GetLockedProgressionMaterial());
	}
}

// Triggers when a spot is loaded
void UPSWorldSubsystem::OnSpotComponentLoad(UPSSpotComponent* SpotComponent)
{
	if (SpotComponent)
	{
		PSSpotComponentInternal = SpotComponent;
		UpdateProgressionActorsForSpot();
	}
}

// Called when the current game state was changed
void UPSWorldSubsystem::OnGameStateChanged(ECurrentGameState CurrentGameState)
{
	// Show Progression Menu widget in Main Menu
	if (CurrentGameState == ECurrentGameState::Menu)
	{
		UpdateProgressionActorsForSpot();
	}
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
	UpdateProgressionActorsForSpot();
}

// Unlocks all levels of the Progression System
void UPSWorldSubsystem::UnlockAllLevels()
{
	SaveGameInstanceInternal->UnlockAllLevels();
	SaveDataAsync();
	UPSHUDComponent* PSHUDComponent = GetProgressionSystemHUDComponent();
	checkf(PSHUDComponent, TEXT("ERROR: 'PSHUDComponent' is null"));
	PSHUDComponent->UpdateProgressionWidgetForPlayer();
	UpdateProgressionActorsForSpot();
}

// Returns difficultyMultiplier
float UPSWorldSubsystem::GetDifficultyMultiplier()
{
	TMap<EGameDifficulty, float> DifficultyMap = UPSDataAsset::Get().GetProgressionDifficultyMultiplier();
	ensureMsgf(!DifficultyMap.IsEmpty(), TEXT("ASSERT: DifficultyMap is empty"));

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
