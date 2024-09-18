// Copyright (c) Valerii Rotermel & Yevhenii Selivanov

#pragma once

#include "GameFramework/Actor.h"
#include "PSStarActor.generated.h"

UCLASS()
class PROGRESSIONSYSTEMRUNTIME_API APSStarActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APSStarActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Function called every frame on this Actor */
	virtual void Tick(float DeltaTime) override;

	/** Stores the starting time to hide stars */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Transient, Category = "C++", meta = (BlueprintProtected, DisplayName = "Starting time to hide stars"))
	float StartTimeHideStarsInternal = 0.0f;

	/** When a local character load finished */
	UFUNCTION(BlueprintCallable, Category= "C++", meta = (BlueprintProtected))
	void OnLocalCharacterReady(APlayerCharacter* Character, int32 CharacterID);

	/** Is called when any cinematic started */
	UFUNCTION(BlueprintCallable, Category= "C++", meta = (BlueprintProtected))
	void OnAnyCinematicStarted(const UObject* LevelSequence, const UObject* FromInstigator);

	/** Hiding stars with animation */
	UFUNCTION(BlueprintCallable, Category= "C++", meta = (BlueprintProtected))
	void TryPlayHideStarAnimation();
};
