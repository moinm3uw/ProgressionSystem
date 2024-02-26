// Copyright (c) Valerii Rotermel & Yevhenii Selivanov

#include "Data/PSDataAsset.h"

#include "Data/PSWorldSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PSDataAsset)

const UPSDataAsset& UPSDataAsset::Get()
{
	const UPSDataAsset* DataAsset = UPSWorldSubsystem::Get().GetPSDataAsset();
	checkf(DataAsset, TEXT("%s: 'DataAsset' is not set"), *FString(__FUNCTION__));
	return *DataAsset;
}
