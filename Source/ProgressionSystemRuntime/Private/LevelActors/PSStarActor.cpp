// Copyright (c) Valerii Rotermel and Yevhenii Selivanov


#include "LevelActors/PSStarActor.h"

#include "PoolManagerSubsystem.h"
#include "Components/StaticMeshComponent.h"
#include "Controllers/MyPlayerController.h"
#include "Data/PSTypes.h"
#include "Data/PSWorldSubsystem.h"
#include "Engine/World.h"
#include "GameFramework/MyGameStateBase.h"
#include "LevelActors/PlayerCharacter.h"
#include "MyUtilsLibraries/GameplayUtilsLibrary.h"
#include "Subsystems/GlobalEventsSubsystem.h"
#include "UtilityLibraries/MyBlueprintFunctionLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PSStarActor)

// Sets default values
APSStarActor::APSStarActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

// Called when the game starts or when spawned
void APSStarActor::BeginPlay()
{
	Super::BeginPlay();

	// Listen to hande when local character is ready 
	BIND_ON_LOCAL_CHARACTER_READY(this, ThisClass::OnLocalCharacterReady);

	// Listen to handle input for each game state
	BIND_ON_GAME_STATE_CHANGED(this, ThisClass::OnGameStateChanged);
}

// Function called every frame on this Actor
void APSStarActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// play only if cinematic is started
	// else play menu animation
	if (StartTimeHideStarsInternal)
	{
		TryPlayHideStarAnimation();
	}
	else if (StartTimeMenuStarsInternal)
	{
		TryPlayMenuStarAnimation();
	}
}

// When a local character load finished
void APSStarActor::OnLocalCharacterReady(APlayerCharacter* Character, int32 CharacterID)
{
	AMyPlayerController* LocalPC = Character ? Character->GetController<AMyPlayerController>() : nullptr;
	if (ensureMsgf(LocalPC, TEXT("ASSERT: [%i] %hs:\n'LocalPC' is null!"), __LINE__, __FUNCTION__))
	{
		LocalPC->OnAnyCinematicStarted.AddDynamic(this, &APSStarActor::OnAnyCinematicStarted);
	}
}

// Called when the current game state was changed
void APSStarActor::OnGameStateChanged(ECurrentGameState GameState)
{
	if (GameState == ECurrentGameState::Menu)
	{
		SetStartTimeMenuStars();
		TryPlayMenuStarAnimation();
	}
	else
	{
		StartTimeMenuStarsInternal = 0.f;
	}
}

// Is called when any cinematic started
void APSStarActor::OnAnyCinematicStarted(const UObject* LevelSequence, const UObject* FromInstigator)
{
	SetStartTimeHideStars();
	TryPlayHideStarAnimation();
}

// Hiding stars with animation in main menu when cinematic is start to play
void APSStarActor::TryPlayHideStarAnimation()
{
	const FPSRowData& CurrentProgressionSettingsRow = UPSWorldSubsystem::Get().GetCurrentProgressionSettingsRowByName();
	const bool bIsFinished = !TryPlayStarAnimation(StartTimeHideStarsInternal, CurrentProgressionSettingsRow.HideStarsAnimation);
	if (bIsFinished)
	{
		StartTimeHideStarsInternal = 0.f;
		UPoolManagerSubsystem::Get().ReturnToPool(this);
	}
}

// Menu stars with animation in main menu idle 
void APSStarActor::TryPlayMenuStarAnimation()
{
	const FPSRowData& CurrentProgressionSettingsRow = UPSWorldSubsystem::Get().GetCurrentProgressionSettingsRowByName();
	const bool bIsFinished = !TryPlayStarAnimation(StartTimeMenuStarsInternal, CurrentProgressionSettingsRow.MenuStarsAnimation);
	if (bIsFinished)
	{
		StartTimeMenuStarsInternal = GetWorld()->GetTimeSeconds();
	}
}

// Helper function that plays any given star animation from various places
bool APSStarActor::TryPlayStarAnimation(float& StartTimeRef, UCurveTable* AnimationCurveTable)
{
	if (!StartTimeRef || !AnimationCurveTable)
	{
		StartTimeRef = 0.f;
		return false;
	}

	const float SecondsSinceStart = GetWorld()->GetTimeSeconds() - StartTimeRef;

	return UGameplayUtilsLibrary::ApplyTransformFromCurveTable(this, AnimationCurveTable, SecondsSinceStart);
}

// Set the start time for hiding stars
void APSStarActor::SetStartTimeHideStars()
{
	const UWorld* World = GetWorld();
	check(World);

	StartTimeHideStarsInternal = World->GetTimeSeconds();
}

// Set the start time for main menu stars animation
void APSStarActor::SetStartTimeMenuStars()
{
	const UWorld* World = GetWorld();
	check(World);

	StartTimeMenuStarsInternal = World->GetTimeSeconds();
}
