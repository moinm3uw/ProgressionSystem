// Copyright (c) Valerii Rotermel & Yevhenii Selivanov

#include "Data/PSWorldSubsystem.h"

#include "MyUtilsLibraries/UtilsLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PSWorldSubsystem)

// Returns this Subsystem, is checked and wil crash if can't be obtained
UPSWorldSubsystem& UPSWorldSubsystem::Get()
{
	const UWorld* World = UUtilsLibrary::GetPlayWorld();
	checkf(World, TEXT("%s: 'World' is null"), *FString(__FUNCTION__));
	UPSWorldSubsystem* ThisSubsystem = World->GetSubsystem<ThisClass>();
	checkf(ThisSubsystem, TEXT("%s: 'SoundsSubsystem' is null"), *FString(__FUNCTION__));
	return *ThisSubsystem;
}

// Returns this Subsystem, is checked and wil crash if can't be obtained
UPSWorldSubsystem& UPSWorldSubsystem::Get(const UObject& WorldContextObject)
{
	const UWorld* World = GEngine->GetWorldFromContextObjectChecked(&WorldContextObject);
	checkf(World, TEXT("%s: 'World' is null"), *FString(__FUNCTION__));
	UPSWorldSubsystem* ThisSubsystem = World->GetSubsystem<ThisClass>();
	checkf(ThisSubsystem, TEXT("%s: 'SoundsSubsystem' is null"), *FString(__FUNCTION__));
	return *ThisSubsystem;
}

void UPSWorldSubsystem::SetProgressionSystemComponent(UPSHUDComponent* MyProgressionSystemComponent)
{
	checkf(MyProgressionSystemComponent, TEXT("%s: My progression system component is null"), *FString(__FUNCTION__));
	ProgressionSystemComponentInternal = MyProgressionSystemComponent;
}
