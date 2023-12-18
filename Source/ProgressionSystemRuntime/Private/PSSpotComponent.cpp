// Copyright (c) Yevhenii Selivanov


#include "PSSpotComponent.h"

#include "ProgressionSystemComponent.h"
#include "PSCWorldSubsystem.h"
#include "Components/MySkeletalMeshComponent.h"
#include "Controllers/MyPlayerController.h"
#include "GameFramework/MyGameStateBase.h"
#include "GameFramework/MyPlayerState.h"
#include "LevelActors/PlayerCharacter.h"
#include "UtilityLibraries/MyBlueprintFunctionLibrary.h"

// Sets default values for this component's properties
UPSSpotComponent::UPSSpotComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

// Called when the game starts
void UPSSpotComponent::BeginPlay()
{
	Super::BeginPlay();

	PlayerSpotOnLevelInternal = GetMeshChecked();
	
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
}

UMySkeletalMeshComponent* UPSSpotComponent::GetMySkeletalMeshComponent() const
{
	return GetOwner()->FindComponentByClass<UMySkeletalMeshComponent>();
}

UMySkeletalMeshComponent& UPSSpotComponent::GetMeshChecked() const
{
	UMySkeletalMeshComponent* Mesh = GetMySkeletalMeshComponent();
	checkf(Mesh, TEXT("'Mesh' is nullptr, can not get mesh for '%s' spot."), *GetNameSafe(this));
	return *Mesh;
}

void UPSSpotComponent::OnGameStateChanged(ECurrentGameState CurrentGameState)
{
	if (CurrentGameState == ECurrentGameState::Menu)
	{
		// Locks and unlocks the spot depends on the current level progression status
		ChangeSpotVisibilityStatus();
	}
}

void UPSSpotComponent::OnEndGameStateChanged(EEndGameState EndGameState)
{
	if (EndGameState != EEndGameState::None)
	{
		// Locks and unlocks the spot depends on the current level progression status
		ChangeSpotVisibilityStatus();
	}
}

void UPSSpotComponent::HandleGameState(AMyGameStateBase* MyGameState)
{
	checkf(MyGameState, TEXT("ERROR: 'MyGameState' is null!"));

	// Listen states to handle
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

void UPSSpotComponent::HandleEndGameState(AMyPlayerState* MyPlayerState)
{
	checkf(MyPlayerState, TEXT("ERROR: 'MyGameState' is null!")); 
	MyPlayerState->OnEndGameStateChanged.AddUniqueDynamic(this, &ThisClass::OnEndGameStateChanged);
}

void UPSSpotComponent::OnPlayerTypeChanged(FPlayerTag PlayerTag)
{
	ChangeSpotVisibilityStatus();
}

void UPSSpotComponent::ChangeSpotVisibilityStatus()
{
	MyProgressionSystemComponentInternal = UPSCWorldSubsystem::Get().GetProgressionSystemComponent();	 
	
	// Locks and unlocks the spot depends on the current level progression status
	if (MyProgressionSystemComponentInternal != nullptr)
	{
		if (PlayerSpotOnLevelInternal)
		{
			CurrentProgressionRowDataInternal = MyProgressionSystemComponentInternal->GetSavedProgressionRowData();
			PlayerSpotOnLevelInternal->SetActive(!CurrentProgressionRowDataInternal.IsLevelLocked);
		}
	}
}

