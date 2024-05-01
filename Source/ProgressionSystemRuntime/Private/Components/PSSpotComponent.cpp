// Copyright (c) Valerii Rotermel & Yevhenii Selivanov

#include "Components/PSSpotComponent.h"

#include "Components/PSHUDComponent.h"
#include "Data/PSWorldSubsystem.h"
#include "Data/PSSaveGameData.h"
#include "Components/MySkeletalMeshComponent.h"
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

// Called when the game starts
void UPSSpotComponent::BeginPlay()
{
	Super::BeginPlay();

	// Ensure the component's mesh is properly assigned and not null.
	PlayerSpotOnLevelInternal = GetMeshChecked();

	UPSWorldSubsystem::Get().OnCurrentRowDataChanged.AddDynamic(this, &ThisClass::OnPlayerTypeChanged);

	// Ensure the save game data is properly loaded and not null.
	SaveGameInstanceInternal = UPSWorldSubsystem::Get().GetCurrentSaveGameData();
	checkf(SaveGameInstanceInternal, TEXT("ERROR: 'SaveGameInstanceInternal' is null"));

	// Update the visibility status of the spot based on current conditions or settings
	ChangeSpotVisibilityStatus();
}

void UPSSpotComponent::OnUnregister()
{
	Super::OnUnregister();
	PlayerSpotOnLevelInternal = nullptr;
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

void UPSSpotComponent::OnPlayerTypeChanged(FPlayerTag PlayerTag)
{
	ChangeSpotVisibilityStatus();
}

void UPSSpotComponent::ChangeSpotVisibilityStatus()
{
	// Locks and unlocks the spot depends on the current level progression status
	if (PlayerSpotOnLevelInternal)
	{
		PlayerSpotOnLevelInternal->SetActive(!SaveGameInstanceInternal->GetCurrentRow().IsLevelLocked);
	}
}
