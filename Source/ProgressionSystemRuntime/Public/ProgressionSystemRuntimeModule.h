// Copyright (c) Valerii Rotermel & Yevhenii Selivanov

#pragma once
#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"

class PROGRESSIONSYSTEMRUNTIME_API FProgressionSystemRuntimeModule : public IModuleInterface
{
public:
	inline static const FName PSModuleName = TEXT("ProgressionSystem");

	//~IModuleInterface
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	//~End of IModuleInterface
};
