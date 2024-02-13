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

	PlayerSpotOnLevelInternal = GetMeshChecked();
	checkf(PlayerSpotOnLevelInternal, TEXT("ERROR: 'PlayerSpotOnLevelInternal' is null"));

	UPSWorldSubsystem::Get().OnCurrentRowDataChanged.AddDynamic(this, &ThisClass::OnPlayerTypeChanged);

	PSHUDComponentInternal = UPSWorldSubsystem::Get().GetProgressionSystemHUDComponent();
	checkf(PSHUDComponentInternal, TEXT("ERROR: 'MyProgressionSystemComponentInterna is null'"));

	SaveGameInstanceInternal = UPSWorldSubsystem::Get().GetCurrentSaveGameData();
	checkf(SaveGameInstanceInternal, TEXT("ERROR: 'SaveGameInstanceInternal' is null"));

	ChangeSpotVisibilityStatus();
}

void UPSSpotComponent::OnUnregister()
{
	Super::OnUnregister();
	PSHUDComponentInternal = nullptr;
	PlayerSpotOnLevelInternal = nullptr;
	CurrentProgressionRowDataInternal = FPSRowData::EmptyData;
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
