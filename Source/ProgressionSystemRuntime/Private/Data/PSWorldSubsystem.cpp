// Copyright (c) Valerii Rotermel & Yevhenii Selivanov

#include "Data/PSWorldSubsystem.h"

#include "ProgressionSystemRuntimeModule.h"
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

// Returns the data asset that contains all the assets of Progression System game feature
const UPSDataAsset* UPSWorldSubsystem::GetPSDataAsset() const
{
	return PSDataAssetInternal.LoadSynchronous();
}

void UPSWorldSubsystem::SetProgressionSystemComponent(UPSHUDComponent* MyProgressionSystemComponent)
{
	checkf(MyProgressionSystemComponent, TEXT("%s: My progression system component is null"), *FString(__FUNCTION__));
	PSHUDComponentInternal = MyProgressionSystemComponent;
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
	if (PSDataAssetInternal)
	{
		ProgressionDataTableInternal = PSDataAssetInternal->GetProgressionDataTable();
		LoadGameFromSave();	
	} else
	{
		UE_LOG(LogProgressionSystem, Warning, TEXT("PSDataAssetInternal null"))
	}
}

void UPSWorldSubsystem::Deinitialize()
{
	Super::Deinitialize();
	PSHUDComponentInternal = nullptr;
	ProgressionDataTableInternal = nullptr;
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
	if (SaveGameInstanceInternal)
	{
		for (const auto& KeyValue : SaveGameInstanceInternal->SavedProgressionRows)
		{
			FName Key = KeyValue.Key;
			FPSRowData RowData = KeyValue.Value;

			if (RowData.Map == UMyBlueprintFunctionLibrary::GetLevelType() && RowData.Character == PlayerTag)
			{
				SavedProgressionRowDataInternal = SaveGameInstanceInternal->SavedProgressionRows[Key];
			}
		}
		SaveDataAsync();
	}
	
	OnCurrentRowDataChanged.Broadcast(SavedProgressionRowDataInternal);
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
		checkf(ProgressionDataTableInternal, TEXT("ERROR: 'ProgressionDataTableInternal' is null"));
		UMyDataTable::GetRows(*ProgressionDataTableInternal, SavedProgressionRows);
		SaveGameInstanceInternal = Cast<UPSSaveGameData>(UGameplayStatics::CreateSaveGameObject(UPSSaveGameData::StaticClass()));

		if (SaveGameInstanceInternal)
		{
			SaveGameInstanceInternal->SavedProgressionRows = SavedProgressionRows;
		}
	}
	SetFirstElemetAsCurrent();
}

void UPSWorldSubsystem::SetFirstElemetAsCurrent()
{
	if (SaveGameInstanceInternal)
	{
		// Create an iterator for the TMap
		TMap<FName, FPSRowData>::TIterator Iterator = SaveGameInstanceInternal->SavedProgressionRows.CreateIterator();

		// Check if the iterator is pointing to a valid position
		if (Iterator)
		{
			// Access the key and value of the first element
			SavedProgressionRowDataInternal = Iterator.Value();
			SavedProgressionRowDataInternal.IsLevelLocked = false;
			Iterator.Value().IsLevelLocked = false;
		}		
		else
		{
			// The TMap is empty
			UE_LOG(LogTemp, Warning, TEXT("The TMap is empty."));
		}		
	}
	SaveDataAsync();
}

void UPSWorldSubsystem::SaveDataAsync()
{
	checkf(SaveGameInstanceInternal, TEXT("ERROR: 'SaveGameInstanceInternal' is null"));
	UGameplayStatics::AsyncSaveGameToSlot(SaveGameInstanceInternal, SaveGameInstanceInternal->GetSaveSlotName(), SaveGameInstanceInternal->GetSaveSlotIndex());
}
