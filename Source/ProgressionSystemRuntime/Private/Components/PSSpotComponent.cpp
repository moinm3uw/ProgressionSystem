// Copyright (c) Valerii Rotermel & Yevhenii Selivanov

#include "Components/PSSpotComponent.h"

#include "Components/PSHUDComponent.h"
#include "Data/PSWorldSubsystem.h"
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

	UPSWorldSubsystem::Get().OnCurrentRowDataChanged.AddUObject(this, &ThisClass::OnPlayerTypeChanged);

	PSHUDComponentInternal = UPSWorldSubsystem::Get().GetProgressionSystemHUDComponent();
	checkf(PSHUDComponentInternal, TEXT("ERROR: 'MyProgressionSystemComponentInterna is null'"));
	CurrentProgressionRowDataInternal = PSHUDComponentInternal->GetSavedProgressionRowData();
	
	ChangeSpotVisibilityStatus();
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

void UPSSpotComponent::OnPlayerTypeChanged(FPSRowData RowData)
{
	CurrentProgressionRowDataInternal = RowData;
	ChangeSpotVisibilityStatus();
}

void UPSSpotComponent::ChangeSpotVisibilityStatus()
{
	// Locks and unlocks the spot depends on the current level progression status
	
		if (PlayerSpotOnLevelInternal)
		{
			if (CurrentProgressionRowDataInternal.IsLevelLocked)
			{
				PlayerSpotOnLevelInternal->SetActive(false);
			} else if (CurrentProgressionRowDataInternal.IsLevelLocked == false)
			{
				PlayerSpotOnLevelInternal->SetActive(true);
			}
		}
}
