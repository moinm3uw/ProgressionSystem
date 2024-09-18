// Copyright (c) Valerii Rotermel and Yevhenii Selivanov


#include "LevelActors/PSStarActor.h"

#include "Components/StaticMeshComponent.h"
#include "Controllers/MyPlayerController.h"
#include "Data/PSTypes.h"
#include "Data/PSWorldSubsystem.h"
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
	PrimaryActorTick.bStartWithTickEnabled = false;
}

// Called when the game starts or when spawned
void APSStarActor::BeginPlay()
{
	Super::BeginPlay();

	BIND_ON_LOCAL_CHARACTER_READY(this, ThisClass::OnLocalCharacterReady);
}

// Function called every frame on this Actor
void APSStarActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TryPlayHideStarAnimation();
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

// Is called when any cinematic started
void APSStarActor::OnAnyCinematicStarted(const UObject* LevelSequence, const UObject* FromInstigator)
{
	const UWorld* World = GetWorld();
	check(World);

	StartTimeHideStarsInternal = World->GetTimeSeconds();

	// Enable tick and process this frame
	SetActorTickEnabled(true);
	TryPlayHideStarAnimation();
}

// Hiding stars with animation
void APSStarActor::TryPlayHideStarAnimation()
{
	if (!StartTimeHideStarsInternal)
	{
		return;
	}

	const FPSRowData& CurrentRow = UPSWorldSubsystem::Get().GetCurrentRow();
	if (!CurrentRow.HideStarsAnimation)
	{
		SetActorTickEnabled(false);
		StartTimeHideStarsInternal = 0.f;
		return;
	}

	const float SecondsSinceStart = GetWorld()->GetTimeSeconds() - StartTimeHideStarsInternal;

	const bool bIsFinished = !UGameplayUtilsLibrary::ApplyTransformFromCurveTable(this, CurrentRow.HideStarsAnimation, SecondsSinceStart);
	if (bIsFinished)
	{
		SetActorTickEnabled(false);
		StartTimeHideStarsInternal = 0.f;
	}
}
