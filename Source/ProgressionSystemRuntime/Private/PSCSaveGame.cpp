// Copyright (c) Valerii Rotermel & Yevhenii Selivanov

#include "PSCSaveGame.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PSCSaveGame)

const FString& UPSCSaveGame::GetSaveSlotName()
{
	static const FString SaveSlotName = StaticClass()->GetName();
	return SaveSlotName;
}
