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
#include "NativeGameplayTags.h"
#include "Components/MapComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PSSpotComponent)

// Skeletal mesh actor should own this tag, used to prevent initializing PS spots on other skeletal mesh actors, like from cinematics
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_NMM_SPOT, TEXT("NMM.Spot"));

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
	BIND_ON_GAME_STATE_CHANGED(this, ThisClass::OnGameStateChanged);
	constexpr bool bApplySkin = false;
	RefreshAmountOfUnlockedSkins(bApplySkin);

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

// Listen game states to switch character skin. 
void UPSSpotComponent::OnGameStateChanged_Implementation(ECurrentGameState CurrentGameState)
{
	if (!IsCurrentSpot())
	{
		return;
	}

	constexpr bool bApplySkin = true;
	switch (CurrentGameState)
	{
	case ECurrentGameState::GameStarting:
		TryRestorePlayerSkin();
		break;
	case ECurrentGameState::Menu:
		RefreshAmountOfUnlockedSkins(bApplySkin);
		break;
	default: break;
	}
}

// Called when the game starts
void UPSSpotComponent::BeginPlay()
{
	Super::BeginPlay();

	// Skeletal mesh actor should own this tag, used to prevent initializing PS spots on other skeletal mesh actors, like from cinematics
	static const FName ExpectedTagName = TAG_NMM_SPOT.GetTag().GetTagName();
	if (!GetOwner()->ActorHasTag(ExpectedTagName))
	{
		UE_LOG(LogBomber, Log, TEXT("[%i] %hs: Skip initializing '%s' spot for '%s' actor, it doesn't have '%s' tag."),
		       __LINE__, __FUNCTION__, *GetNameSafe(this), *GetNameSafe(GetOwner()), *ExpectedTagName.ToString());
		return;
	}

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

// Check is player is allowed to play with current skin if not switch to allowed
void UPSSpotComponent::TryRestorePlayerSkin()
{
	// it's possible that spot might not be loaded till that time so no ensure added
	const UPSSpotComponent* CurrentSpot = UPSWorldSubsystem::Get().GetCurrentSpot();
	if (CurrentSpot == nullptr || CurrentSpot != this)
	{
		return;
	}

	UMySkeletalMeshComponent& MeshComp = GetMeshChecked();
	const int32 CurrentSkinIndex = MeshComp.GetAppliedSkinIndex();

	// Current skins is available no need to switch to last avaialble
	if (MeshComp.IsSkinAvailable(CurrentSkinIndex))
	{
		return;
	}

	// find last unlocked skin 
	for (int32 Count = CurrentSkinIndex; Count >= 0; Count--)
	{
		if (Count == 0 || MeshComp.IsSkinAvailable(Count))
		{
			MeshComp.ApplySkinByIndex(Count);

			constexpr bool bApplySkin = true;
			RefreshAmountOfUnlockedSkins(bApplySkin);
			break;
		}
	}
}

// Updates the progression menu widget when player changed
void UPSSpotComponent::OnCurrentActiveSaveRowChanged_Implementation(const FPlayerTag NewPlayerTag, const FPlayerTag PreviousPlayerTag)
{
	UMySkeletalMeshComponent& Mesh = GetMeshChecked();
	if (Mesh.GetPlayerTag() == NewPlayerTag)
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
	if (!IsCurrentSpot())
	{
		return;
	}

	UMySkeletalMeshComponent& SpotMeshComponent = GetMeshChecked();
	const int32 UnlockedSkinsAmount = UPSWorldSubsystem::Get().GetCurrentSaveToDiskRowByName().UnlockedSkinsAmount;
	const int32 CurrentSkinIndex = SpotMeshComponent.GetAppliedSkinIndex();
	const int32 TotalSkins = SpotMeshComponent.GetSkinTexturesNum();

	if (!ensureMsgf(UnlockedSkinsAmount <= TotalSkins, TEXT("ASSERT: [%i] %hs:\n Unlocked amount of skins is more than characters has, check the configuration!"), __LINE__, __FUNCTION__))
	{
		return;
	}

	if (AMyGameStateBase::GetCurrentGameState() == ECurrentGameState::Menu)
	{
		int32 PreviousAmountOfUnlockedSkins = 0;
		for (int32 SkinIndex = 0; SkinIndex < TotalSkins; SkinIndex++)
		{
			if (SpotMeshComponent.IsSkinAvailable(SkinIndex))
			{
				PreviousAmountOfUnlockedSkins++;
				bApplySkin = true;
			}
		}
		// do nothing skins amount is not changed 
		if (PreviousAmountOfUnlockedSkins != 1 && PreviousAmountOfUnlockedSkins == UnlockedSkinsAmount + 1)
		{
			return;
		}
	}

	for (int32 Index = CurrentSkinIndex; Index <= UnlockedSkinsAmount; Index++)
	{
		SpotMeshComponent.SetSkinAvailable(true, Index);
		if (bApplySkin)
		{
			SpotMeshComponent.ApplySkinByIndex(Index);
			const APlayerCharacter* PlayerCharacter = UMyBlueprintFunctionLibrary::GetLocalPlayerCharacter();
			if (UMapComponent* MapComponent = UMapComponent::GetMapComponent(PlayerCharacter))
			{
				MapComponent->SetReplicatedMeshData(SpotMeshComponent.GetMeshData());
			}
		}
	}
}

// Returns true if this is a current spot
bool UPSSpotComponent::IsCurrentSpot() const
{
	const APlayerCharacter* PlayerCharacter = UMyBlueprintFunctionLibrary::GetLocalPlayerCharacter();
	const FPlayerTag& PlayerTag = PlayerCharacter ? PlayerCharacter->GetPlayerTag() : FPlayerTag::None;
	return PlayerCharacter && GetMeshChecked().GetPlayerTag() == PlayerTag;
}
