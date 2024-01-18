// Copyright (c) Valeriy Rotermel and Yevhenii Selivanov

using UnrealBuildTool;

public class ProgressionSystemRuntime : ModuleRules
{
	public ProgressionSystemRuntime(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		CppStandard = CppStandardVersion.Latest;
		bEnableNonInlinedGenCppWarnings = true;

		PublicDependencyModuleNames.AddRange(new[]
			{
				"Core"
				, "UMG"
				// Bomber modules
				, "Bomber" // Is included in header files
			}
		);

		PrivateDependencyModuleNames.AddRange(new[]
			{
				"CoreUObject", "Engine", "Slate", "SlateCore", // Core
				// Bomber modules 
				"MyUtils" 
			}
		);
	}
}
