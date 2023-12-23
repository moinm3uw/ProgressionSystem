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
				, "Bomber" // Is included in header files
				, "UMG"
			}
		);

		PrivateDependencyModuleNames.AddRange(new[]
			{
				"CoreUObject", "Engine", "Slate", "SlateCore", "MyUtils" // Core
			}
		);
	}
}
