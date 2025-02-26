// Copyright (c) Valerii Rotermel & Yevhenii Selivanov

#include "Components/PSSpotComponent.h"

#include "Components/PSHUDComponent.h"
#include "Data/PSWorldSubsystem.h"
#include "Components/MySkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/MyGameStateBase.h"
#include "LevelActors/PlayerCharacter.h"
#include "Subsystems/GlobalEventsSubsystem.h"
#include "UtilityLibraries/MyBlueprintFunctionLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PSSpotComponent)

// Sets default values for this component's properties
UPSSpotComponent::UPSSpotComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

// Called when progression module ready
void UPSSpotComponent::OnInitialized_Implementation()
{
	UPSWorldSubsystem& WorldSubsystem = UPSWorldSubsystem::Get();
	WorldSubsystem.OnCurrentActiveSaveRowChanged.AddUniqueDynamic(this, &ThisClass::OnCurrentActiveSaveRowChanged);
	WorldSubsystem.OnCurrentScoreChanged.AddUniqueDynamic(this, &ThisClass::OnCurrentScoreChanged);

	// Listen to handle input for each game state
	BIND_ON_GAME_STATE_CHANGED(this, ThisClass::OnGameStateChanged);

	// Save reference of this component to the world subsystem
	WorldSubsystem.RegisterSpotComponent(this);
}

// Once the save file is reset the spot component needs to reset skins
void UPSSpotComponent::OnReset_Implementation()
{
	// reset 
	UMySkeletalMeshComponent& SpotMeshComponent = GetMeshChecked();
	for (int32 Index = 1; Index < SpotMeshComponent.GetSkinTexturesNum(); Index++)
	{
		SpotMeshComponent.SetSkinAvailable(false, Index);
	}
}

// Called when the game starts
void UPSSpotComponent::BeginPlay()
{
	Super::BeginPlay();
	UPSWorldSubsystem& WorldSubsystem = UPSWorldSubsystem::Get();
	WorldSubsystem.OnInitialize.AddDynamic(this, &ThisClass::OnInitialized);
	WorldSubsystem.OnReset.AddDynamic(this, &ThisClass::OnReset);
	OnReset();
}

// Clears all transient data created by this component.
void UPSSpotComponent::OnUnregister()
{
	// reset back to initial state. By default, the spot is unlocked 
	constexpr bool bSpotUnlocked = true;
	GetMeshChecked().SetActive(bSpotUnlocked);

	Super::OnUnregister();
}

// Called when the end game state was changed to toggle progression widget visibility.
void UPSSpotComponent::OnGameStateChanged_Implementation(ECurrentGameState CurrentGameState)
{
	const APlayerCharacter* PlayerCharacter = UMyBlueprintFunctionLibrary::GetLocalPlayerCharacter();
	if (!ensureMsgf(PlayerCharacter, TEXT("ASSERT: [%i] %hs:\n'PlayerCharacter' is not valid!"), __LINE__, __FUNCTION__))
	{
		return;
	}
	const FPlayerTag& PlayerTag = PlayerCharacter->GetPlayerTag();

	if (GetMeshChecked().GetPlayerTag() == PlayerTag)
	{
		constexpr bool bApplySkin = true;

		switch (CurrentGameState)
		{
		case ECurrentGameState::Menu:
			RefreshAmountOfUnlockedSkins(bApplySkin);
			break;
		default: break;
		}
	}
}

void UPSSpotComponent::OnCurrentScoreChanged_Implementation(const FPSSaveToDiskData& CurrentSaveToDiskDataRow, const FPSRowData& CurrentProgressionSettingsRow)
{
	const APlayerCharacter* PlayerCharacter = UMyBlueprintFunctionLibrary::GetLocalPlayerCharacter();

	if (!ensureMsgf(PlayerCharacter, TEXT("ASSERT: [%i] %hs:\n'PlayerCharacter' is not valid!"), __LINE__, __FUNCTION__))
	{
		return;
	}
	const FPlayerTag& PlayerTag = PlayerCharacter->GetPlayerTag();
	if (GetMeshChecked().GetPlayerTag() == PlayerTag)
	{
		constexpr bool bApplySkin = false;
		RefreshAmountOfUnlockedSkins(bApplySkin);
	}
}

// Updates the progression menu widget when player changed
void UPSSpotComponent::OnCurrentActiveSaveRowChanged_Implementation(const FPlayerTag PlayerTag)
{
	UMySkeletalMeshComponent& Mesh = GetMeshChecked();
	if (Mesh.GetPlayerTag() == PlayerTag)
	{
		ChangeSpotVisibilityStatus(&Mesh);
		constexpr bool bApplySkin = false;
		RefreshAmountOfUnlockedSkins(bApplySkin);
	}
}

// Returns the Skeletal Mesh of the Bomber character
UMySkeletalMeshComponent* UPSSpotComponent::GetMySkeletalMeshComponent() const
{
	return GetOwner()->FindComponentByClass<UMySkeletalMeshComponent>();
}

// Returns the Skeletal Mesh of the Bomber character
UMySkeletalMeshComponent& UPSSpotComponent::GetMeshChecked() const
{
	UMySkeletalMeshComponent* Mesh = GetMySkeletalMeshComponent();
	ensureMsgf(Mesh, TEXT("ASSERT: [%i] %hs:\n'Mesh' is nullptr, can not get mesh for spot.!"), __LINE__, __FUNCTION__);
	return *Mesh;
}

// Changes the player spot depends on current level state 
void UPSSpotComponent::ChangeSpotVisibilityStatus(UMySkeletalMeshComponent* Mesh)
{
	// Locks and unlocks the spot depends on the current level progression status
	FPSSaveToDiskData SaveToDiskDataRow = UPSWorldSubsystem::Get().GetCurrentSaveToDiskRowByName();
	Mesh->SetActive(!SaveToDiskDataRow.IsLevelLocked);
}

// Refresh Amount Of Unlocked skins for the character (level)s
void UPSSpotComponent::RefreshAmountOfUnlockedSkins(bool bApplySkin)
{
	UMySkeletalMeshComponent& SpotMeshComponent = GetMeshChecked();
	const int32 UnlockedSkinsAmount = UPSWorldSubsystem::Get().GetCurrentSaveToDiskRowByName().UnlockedSkinsAmount;

	for (int32 Index = 0; Index < UnlockedSkinsAmount; Index++)
	{
		SpotMeshComponent.SetSkinAvailable(true, Index);
		if (bApplySkin)
		{
			SpotMeshComponent.ApplySkinByIndex(Index);
		}
	}
}
