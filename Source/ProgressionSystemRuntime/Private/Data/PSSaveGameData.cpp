// Copyright (c) Valerii Rotermel & Yevhenii Selivanov

#include "Data/PSSaveGameData.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PSSaveGameData)

const FString& UPSSaveGameData::GetSaveSlotName()
{
	static const FString SaveSlotName = StaticClass()->GetName();
	return SaveSlotName;
}
