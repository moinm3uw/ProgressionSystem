// Copyright (c) Yevhenii Selivanov

#include "ProgressionSystemComponent.h"
//---
#include "Bomber.h"
#include "Controllers/MyPlayerController.h"
#include "UtilityLibraries/MyBlueprintFunctionLibrary.h"
#include "ProgressionSystemDataAsset.h"
#include "ProgressionSystemRuntimeModule.h"
#include "Blueprint/WidgetTree.h"
#include "UI/MyHUD.h"
#include "Widgets/ProgressionMenuWidget.h"
#include "Widgets/ProgressionSaveWidget.h"
#include "ModuleStructures.h"
#include "PSCSaveGame.h"
#include "PSCWorldSubsystem.h"
#include "Components/Image.h"
#include "GameFramework/MyGameStateBase.h"
#include "GameFramework/MyPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "LevelActors/PlayerCharacter.h"
#include "MyDataTable/MyDataTable.h"

// Sets default values for this component's properties
UProgressionSystemComponent::UProgressionSystemComponent()
{	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

// Called when the game starts
void UProgressionSystemComponent::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogProgressionSystem, Warning, TEXT("--- I'm running ---"));
	
	AMyHUD* MyHUD = Cast<AMyHUD>(GetOwner());
	const AMyHUD& HUD = *MyHUD;
	ProgressionMenuWidgetInternal = HUD.CreateWidgetByClass<UProgressionMenuWidget>(ProgressionSystemDataAssetInternal->GetProgressionMenuWidget(), true, 1);
	ProgressionDataTableInternal = ProgressionSystemDataAssetInternal->GetProgressionDataTable();

	// Listen states to spawn widgets
	if (AMyGameStateBase* MyGameState = UMyBlueprintFunctionLibrary::GetMyGameState())
	{
		HandleGameState(MyGameState);	
	}
	else if (AMyPlayerController* MyPC = GetOwner<AMyPlayerController>())
	{
		MyPC->OnGameStateCreated.AddUniqueDynamic(this, &ThisClass::HandleGameState);
	}

	if (AMyPlayerState* MyPlayerState = UMyBlueprintFunctionLibrary::GetLocalPlayerState())
	{
		HandleEndGameState(MyPlayerState);	
	}
	if (APlayerCharacter* MyPlayerCharacter = UMyBlueprintFunctionLibrary::GetLocalPlayerCharacter())
	{
		MyPlayerCharacter->OnPlayerTypeChanged.AddUniqueDynamic(this, &ThisClass::OnPlayerTypeChanged);
	}

	// Save reference of this component to the world subsystem
	UPSCWorldSubsystem::Get().SetProgressionSystemComponent(this);
	
	LoadGameFromSave();
}

void UProgressionSystemComponent::LoadGameFromSave()
{
	SaveGameInstanceInternal = Cast<UPSCSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveGameInstanceInternal->GetSaveSlotName(), SaveGameInstanceInternal->GetSaveSlotIndex()));
	
	// Check if the save game file exists
	if (UGameplayStatics::DoesSaveGameExist(SaveGameInstanceInternal->GetSaveSlotName(), SaveGameInstanceInternal->GetSaveSlotIndex()))
	{		
		if (SaveGameInstanceInternal)
		{
			for (const auto& KeyValue : SaveGameInstanceInternal->SavedProgressionRows)
			{
				FName Key = KeyValue.Key;
				FProgressionRowData RowData = KeyValue.Value;

				if (RowData.Map == UMyBlueprintFunctionLibrary::GetLevelType() && RowData.Character == CurrentPlayerTagInternal)
				{
					SavedProgressionRowDataInternal = SaveGameInstanceInternal->SavedProgressionRows[Key];
				}
			}
		}
	}
	else
	{
		// Save file does not exist
		// do initial load from data table
		TMap<FName, FProgressionRowData> SavedProgressionRows;
		UMyDataTable::GetRows(*ProgressionDataTableInternal, SavedProgressionRows);
		SaveGameInstanceInternal = Cast<UPSCSaveGame>(UGameplayStatics::CreateSaveGameObject(UPSCSaveGame::StaticClass()));
		
		if (SaveGameInstanceInternal)
		{
			SaveGameInstanceInternal->SavedProgressionRows = SavedProgressionRows;
			SaveDataAsync();
		}
				
		for (const auto& KeyValue : SaveGameInstanceInternal->SavedProgressionRows)
		{
			FName Key = KeyValue.Key;
			FProgressionRowData RowData = KeyValue.Value;

			if (RowData.Map == UMyBlueprintFunctionLibrary::GetLevelType() && RowData.Character == UMyBlueprintFunctionLibrary::GetLocalPlayerCharacter()->GetPlayerTag())
			{
				SavedProgressionRowDataInternal = SaveGameInstanceInternal->SavedProgressionRows[Key];
			}
		}
	}
	UpdateProgressionWidgetForPlayer();
}

// Save the progression depends on EEndGameState
void UProgressionSystemComponent::SavePoints(ELevelType Map, FPlayerTag Character, EEndGameState EndGameState)
{
	SavedProgressionRowDataInternal.CurrentLevelProgression += GetProgressionReward(Map, Character, EndGameState);

	if (SavedProgressionRowDataInternal.CurrentLevelProgression >= SavedProgressionRowDataInternal.PointsToUnlock)
	{
		NextLevelProgressionRowData();
	}
	
	SaveCurrentGameProgression();
}

// Returns rewards from data table for each type of game endings 
int32 UProgressionSystemComponent::GetProgressionReward(ELevelType Map, FPlayerTag Character, EEndGameState EndGameState)
{
	FName CurrentProgressionRowName = GetProgressionRowName(Map, Character);
	FProgressionRowData* ProgressionRowData = ProgressionDataTableInternal->FindRow<FProgressionRowData>(CurrentProgressionRowName, "Finding a needed row");
	if (!ProgressionRowData)
	{
		return 0; 
	}
	
	switch (EndGameState)
	{
	case EEndGameState::Win:
		return ProgressionRowData->WinReward;
	case EEndGameState::Draw:
		return ProgressionRowData->DrawReward;
	case EEndGameState::Lose:
		return ProgressionRowData->LossReward;
	default:
		return 0;
	}
}

// Finds current game progression and save to save file
void UProgressionSystemComponent::SaveCurrentGameProgression()
{
	if (SaveGameInstanceInternal)
	{
		SaveGameInstanceInternal->SavedProgressionRowDataInternal = SavedProgressionRowDataInternal;
		
		for (const auto& KeyValue : SaveGameInstanceInternal->SavedProgressionRows)
		{
			FName Key = KeyValue.Key;
			FProgressionRowData RowData = KeyValue.Value;

			if (RowData.Map == UMyBlueprintFunctionLibrary::GetLevelType() && RowData.Character == CurrentPlayerTagInternal)
			{
				SaveGameInstanceInternal->SavedProgressionRows[Key] = SavedProgressionRowDataInternal;
			}
		}
	}
	SaveDataAsync();
}

void UProgressionSystemComponent::SaveDataAsync()
{
	UGameplayStatics::AsyncSaveGameToSlot(SaveGameInstanceInternal, SaveGameInstanceInternal->GetSaveSlotName(), SaveGameInstanceInternal->GetSaveSlotIndex());	
}

FName UProgressionSystemComponent::GetProgressionRowName(ELevelType Map, FPlayerTag Character)
{
	FName CurrentProgressionRowName;
	TArray<FName> RowNames = ProgressionDataTableInternal->GetRowNames();
	for (auto RowName : RowNames)
	{
		FProgressionRowData* Row = ProgressionDataTableInternal->FindRow<FProgressionRowData>(
			RowName, "Finding progression row name");
		if (Row)
		{
			if (Row->Map == Map && Row->Character == Character)
			{
				CurrentProgressionRowName = RowName;
			}
		}
	}
	return CurrentProgressionRowName;
}

void UProgressionSystemComponent::NextLevelProgressionRowData()
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

		// Find the iterator for the current key
		TMap<FName, FProgressionRowData>::TIterator CurrentIterator = SaveGameInstanceInternal->SavedProgressionRows.CreateIterator();

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
				
				SaveDataAsync();
			}
		}
	}
}

void UProgressionSystemComponent::OnGameStateChanged(ECurrentGameState CurrentGameState)
{
	// Show Progression Menu widget in Main Menu
	ProgressionMenuWidgetInternal->SetVisibility(CurrentGameState == ECurrentGameState::Menu ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	if (CurrentGameState == ECurrentGameState::Menu)
	{
		UpdateProgressionWidgetForPlayer();
	}
}

void UProgressionSystemComponent::OnEndGameStateChanged(EEndGameState EndGameState)
{
	if (EndGameState != EEndGameState::None)
	{
		SavePoints(UMyBlueprintFunctionLibrary::GetLevelType(), CurrentPlayerTagInternal, EndGameState);
	}
}

void UProgressionSystemComponent::HandleGameState(AMyGameStateBase* MyGameState)
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

void UProgressionSystemComponent::HandleEndGameState(AMyPlayerState* MyPlayerState)
{
	checkf(MyPlayerState, TEXT("ERROR: 'MyGameState' is null!")); 
	MyPlayerState->OnEndGameStateChanged.AddUniqueDynamic(this, &ThisClass::OnEndGameStateChanged);
}

void UProgressionSystemComponent::OnPlayerTypeChanged(FPlayerTag PlayerTag)
{
	CurrentPlayerTagInternal = PlayerTag;
	UpdateProgressionWidgetForPlayer();
}

void UProgressionSystemComponent::UpdateProgressionWidgetForPlayer()
{
	if (SaveGameInstanceInternal)
	{
		for (const auto& KeyValue : SaveGameInstanceInternal->SavedProgressionRows)
		{
			FName Key = KeyValue.Key;
			FProgressionRowData RowData = KeyValue.Value;

			if (RowData.Map == UMyBlueprintFunctionLibrary::GetLevelType() && RowData.Character == CurrentPlayerTagInternal)
			{
				SavedProgressionRowDataInternal  = SaveGameInstanceInternal->SavedProgressionRows[Key];
			}
		}
		SaveDataAsync();
	}
	
	ProgressionMenuWidgetInternal->ClearImagesFromHorizontalBox();
	
	if (SavedProgressionRowDataInternal.CurrentLevelProgression >= SavedProgressionRowDataInternal.PointsToUnlock)
	{
		ProgressionMenuWidgetInternal->AddImagesToHorizontalBox(SavedProgressionRowDataInternal.PointsToUnlock, 0);
	} else
	{
		ProgressionMenuWidgetInternal->AddImagesToHorizontalBox(SavedProgressionRowDataInternal.CurrentLevelProgression, SavedProgressionRowDataInternal.PointsToUnlock - SavedProgressionRowDataInternal.CurrentLevelProgression);
	}
	DisplayLevelUIOverlay(SavedProgressionRowDataInternal.IsLevelLocked);
}

void UProgressionSystemComponent::DisplayLevelUIOverlay(bool IsLevelLocked)
{
	
	if(IsLevelLocked)
	{
		ProgressionMenuWidgetInternal->PSCBackgroundOverlay->SetVisibility(ESlateVisibility::Visible);
		ProgressionMenuWidgetInternal->PSCBackgroundIconLock->SetVisibility(ESlateVisibility::Visible);
	} else
	{
		ProgressionMenuWidgetInternal->PSCBackgroundOverlay->SetVisibility(ESlateVisibility::Collapsed);
		ProgressionMenuWidgetInternal->PSCBackgroundIconLock->SetVisibility(ESlateVisibility::Collapsed);
	}
}
