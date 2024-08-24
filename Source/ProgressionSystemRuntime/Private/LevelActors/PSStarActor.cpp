// Copyright (c) Valerii Rotermel and Yevhenii Selivanov


#include "LevelActors/PSStarActor.h"

#include "Components/StaticMeshComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PSStarActor)

// Sets default values
APSStarActor::APSStarActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

void APSStarActor::SetStaticMesh(UStaticMesh* Mesh)
{
	if (Mesh)
	{
		MeshComponentInternal->SetStaticMesh(Mesh);
	}
}

// Called when the game starts or when spawned
void APSStarActor::BeginPlay()
{
	Super::BeginPlay();
}

