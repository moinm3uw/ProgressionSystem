// Copyright (c) Valerii Rotermel & Yevhenii Selivanov

#include "Data/PSDataAsset.h"

#include "DalSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PSDataAsset)

const UPSDataAsset& UPSDataAsset::Get()
{
	return UDalSubsystem::GetDataAssetChecked<ThisClass>();
}
