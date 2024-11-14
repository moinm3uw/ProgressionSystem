// Copyright (c) Valerii Rotermel & Yevhenii Selivanov

#include "Data/PSDataAsset.h"

#include "Data/PSWorldSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PSDataAsset)

const UPSDataAsset& UPSDataAsset::Get()
{
	const UPSDataAsset* DataAsset = UPSWorldSubsystem::Get().GetPSDataAsset();
	ensureMsgf(DataAsset, TEXT("ASSERT: [%i] %hs:\n'DataAsset' is null!"), __LINE__, __FUNCTION__);
	return *DataAsset;
}
