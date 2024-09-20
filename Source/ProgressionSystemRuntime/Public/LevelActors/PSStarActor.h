// Copyright (c) Valerii Rotermel & Yevhenii Selivanov

#pragma once

#include "GameFramework/Actor.h"
#include "PSStarActor.generated.h"

enum class ECurrentGameState : uint8;

UCLASS()
class PROGRESSIONSYSTEMRUNTIME_API APSStarActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APSStarActor();

	/** Helper function that plays any given star animation from various places. */
	UFUNCTION(BlueprintCallable, Category= "C++")
	bool TryPlayStarAnimation(UPARAM(ref) float& StartTimeRef, class UCurveTable* AnimationCurveTable);

	/** Set the start time for hiding stars in the main menu */
	UFUNCTION(BlueprintCallable, Category = "C++")
	void SetStartTimeHideStars();

	/** Set the start time for main menu stars animation */
	UFUNCTION(BlueprintCallable, Category = "C++")
	void SetStartTimeMenuStars();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Function called every frame on this Actor */
	virtual void Tick(float DeltaTime) override;

	/** Stores the starting time to hide stars in the main menu when cinematic started */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Transient, Category = "C++", meta = (BlueprintProtected, DisplayName = "Starting time to hide stars"))
	float StartTimeHideStarsInternal = 0.0f;

	/** Stores the starting time to animate stars in main menu */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Transient, Category = "C++", meta = (BlueprintProtected, DisplayName = "Starting time to animate stars in menu"))
	float StartTimeMenuStarsInternal = 0.0f;

	/** When a local character load finished */
	UFUNCTION(BlueprintCallable, Category= "C++", meta = (BlueprintProtected))
	void OnLocalCharacterReady(class APlayerCharacter* Character, int32 CharacterID);

	/** Called when the current game state was changed */
	UFUNCTION(BlueprintCallable, Category = "C++", meta = (BlueprintProtected))
	void OnGameStateChanged(ECurrentGameState GameState);

	/** Is called when any cinematic started to play in the main menu */
	UFUNCTION(BlueprintCallable, Category= "C++", meta = (BlueprintProtected))
	void OnAnyCinematicStarted(const UObject* LevelSequence, const UObject* FromInstigator);

	/** Hiding stars with animation in main menu when cinematic is start to play */
	UFUNCTION(BlueprintCallable, Category= "C++", meta = (BlueprintProtected))
	void TryPlayHideStarAnimation();

	/** Menu stars with animation in main menu idle */
	UFUNCTION(BlueprintCallable, Category= "C++", meta = (BlueprintProtected))
	void TryPlayMenuStarAnimation();
};
