// Copyright (c) Valerii Rotermel & Yevhenii Selivanov

#pragma once

#include "Modules/ModuleInterface.h"

class PROGRESSIONSYSTEMRUNTIME_API FProgressionSystemRuntimeModule : public IModuleInterface
{
public:
	//~IModuleInterface
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	//~End of IModuleInterface
};
