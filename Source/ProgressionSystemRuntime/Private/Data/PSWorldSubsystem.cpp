// Copyright (c) Valerii Rotermel & Yevhenii Selivanov

#include "Data/PSWorldSubsystem.h"

#include "Controllers/MyPlayerController.h"
#include "Data/PSDataAsset.h"
#include "Data/PSSaveGameData.h"
#include "Kismet/GameplayStatics.h"
#include "LevelActors/PlayerCharacter.h"
#include "MyDataTable/MyDataTable.h"
#include "MyUtilsLibraries/UtilsLibrary.h"
#include "UtilityLibraries/MyBlueprintFunctionLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PSWorldSubsystem)

// Returns this Subsystem, is checked and wil crash if can't be obtained
UPSWorldSubsystem& UPSWorldSubsystem::Get()
{
	const UWorld* World = UUtilsLibrary::GetPlayWorld();
	checkf(World, TEXT("%s: 'World' is null"), *FString(__FUNCTION__));
	UPSWorldSubsystem* ThisSubsystem = World->GetSubsystem<ThisClass>();
	checkf(ThisSubsystem, TEXT("%s: 'ProgressiorSubsystem' is null"), *FString(__FUNCTION__));
	return *ThisSubsystem;
}

// Returns this Subsystem, is checked and wil crash if can't be obtained
UPSWorldSubsystem& UPSWorldSubsystem::Get(const UObject& WorldContextObject)
{
	const UWorld* World = GEngine->GetWorldFromContextObjectChecked(&WorldContextObject);
	checkf(World, TEXT("%s: 'World' is null"), *FString(__FUNCTION__));
	UPSWorldSubsystem* ThisSubsystem = World->GetSubsystem<ThisClass>();
	checkf(ThisSubsystem, TEXT("%s: 'ProgressiorSubsystem' is null"), *FString(__FUNCTION__));
	return *ThisSubsystem;
}

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

void UPSWorldSubsystem::SetHUDComponent(UPSHUDComponent* MyHUDComponent)
{
	checkf(MyHUDComponent, TEXT("%s: My progression system component is null"), *FString(__FUNCTION__));
	PSHUDComponentInternal = MyHUDComponent;
}

void UPSWorldSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	// Listen events on player type changed and Character spawned
	if (APlayerCharacter* MyPlayerCharacter = UMyBlueprintFunctionLibrary::GetLocalPlayerCharacter())
	{
		MyPlayerCharacter->OnPlayerTypeChanged.AddUniqueDynamic(this, &ThisClass::OnPlayerTypeChanged);
	}
	else if (AMyPlayerController* MyPC = UMyBlueprintFunctionLibrary::GetLocalPlayerController())
	{
		MyPC->GetOnNewPawnNotifier().AddUObject(this, &ThisClass::OnCharacterPossessed);
	}

	// requires to be executed to guarntee that data asset will be loaded from the ini file
	// otherwise the data asset file will not be loaded at fresh (heaviest) load of UE
	PSDataAssetInternal = GetPSDataAsset();
	if (ensureMsgf(PSDataAssetInternal, TEXT("ASSERT: PSDataAssetInternal null")))
	{
		LoadGameFromSave();
	}
}

void UPSWorldSubsystem::Deinitialize()
{
	Super::Deinitialize();
	PSHUDComponentInternal = nullptr;
}

void UPSWorldSubsystem::OnCharacterPossessed(APawn* MyPawn)
{
	if (APlayerCharacter* MyPlayerCharacter = Cast<APlayerCharacter>(MyPawn))
	{
		MyPlayerCharacter->OnPlayerTypeChanged.AddUniqueDynamic(this, &ThisClass::OnPlayerTypeChanged);

		//Unsubscribe to ignore null events call
		AMyPlayerController* MyPC = UMyBlueprintFunctionLibrary::GetLocalPlayerController();
		MyPC->GetOnNewPawnNotifier().RemoveAll(this);
	}
}

void UPSWorldSubsystem::OnPlayerTypeChanged(FPlayerTag PlayerTag)
{
	SaveGameInstanceInternal->SetRowByTag(PlayerTag);
	OnCurrentRowDataChanged.Broadcast(PlayerTag);
}

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
		UDataTable* ProgressionDataTable = PSDataAssetInternal->GetProgressionDataTable();
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

void UPSWorldSubsystem::SaveDataAsync()
{
	checkf(SaveGameInstanceInternal, TEXT("ERROR: 'SaveGameInstanceInternal' is null"));
	UGameplayStatics::AsyncSaveGameToSlot(SaveGameInstanceInternal, SaveGameInstanceInternal->GetSaveSlotName(), SaveGameInstanceInternal->GetSaveSlotIndex());
}

// Returns rewards from data table for each type of game endings 
int32 UPSWorldSubsystem::GetProgressionReward(EEndGameState EndGameState)
{
	switch (EndGameState)
	{
	case EEndGameState::Win:
		return SavedProgressionRowDataInternal.WinReward;
	case EEndGameState::Draw:
		return SavedProgressionRowDataInternal.DrawReward;
	case EEndGameState::Lose:
		return SavedProgressionRowDataInternal.LossReward;
	default:
		return 0;
	}
}
