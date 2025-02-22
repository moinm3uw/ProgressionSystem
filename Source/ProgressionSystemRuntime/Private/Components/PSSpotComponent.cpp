// Copyright (c) Valerii Rotermel & Yevhenii Selivanov

#include "Components/PSSpotComponent.h"

#include "Components/PSHUDComponent.h"
#include "Data/PSWorldSubsystem.h"
#include "Data/PSSaveGameData.h"
#include "Components/MySkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "LevelActors/PlayerCharacter.h"
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
	// Save reference of this component to the world subsystem
	WorldSubsystem.RegisterSpotComponent(this);
}

// Called when the game starts
void UPSSpotComponent::BeginPlay()
{
	Super::BeginPlay();
	UPSWorldSubsystem& WorldSubsystem = UPSWorldSubsystem::Get();
	WorldSubsystem.OnInitialize.AddDynamic(this, &ThisClass::OnInitialized);
}

// Clears all transient data created by this component.
void UPSSpotComponent::OnUnregister()
{
	// reset back to initial state. By default, the spot is unlocked 
	constexpr bool bSpotUnlocked = true;
	GetMeshChecked().SetActive(bSpotUnlocked);

	Super::OnUnregister();
}

void UPSSpotComponent::OnCurrentScoreChanged_Implementation(const FPSSaveToDiskData& CurrentSaveToDiskDataRow, const FPSRowData& CurrenProgressionSettingsRow)
{
	RefreshAmountOfUnlockedSkins(CurrentSaveToDiskDataRow.UnlockedSkinsAmount);
}

// Updates the progression menu widget when player changed
void UPSSpotComponent::OnCurrentActiveSaveRowChanged_Implementation(const FPlayerTag PlayerTag)
{
	UMySkeletalMeshComponent& Mesh = GetMeshChecked();
	if (Mesh.GetPlayerTag() == PlayerTag)
	{
		ChangeSpotVisibilityStatus(&Mesh);
		int32 CurrentAmountOfUnlockedSkins = UPSWorldSubsystem::Get().GetCurrentSaveToDiskRowByName().UnlockedSkinsAmount;
		RefreshAmountOfUnlockedSkins(CurrentAmountOfUnlockedSkins);
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
void UPSSpotComponent::RefreshAmountOfUnlockedSkins(int32 UnlockedSkinsAmount)
{
	for (int Index = 0; Index < UnlockedSkinsAmount; Index++)
	{
		UMySkeletalMeshComponent& SpotMeshComponent = GetMeshChecked();
		SpotMeshComponent.SetSkin(Index);
	}
}
