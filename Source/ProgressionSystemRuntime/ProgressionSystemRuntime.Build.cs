// Copyright (c) Valeriy Rotermel and Yevhenii Selivanov

using UnrealBuildTool;

public class ProgressionSystemRuntime : ModuleRules
{
	public ProgressionSystemRuntime(ReadOnlyTargetRules Target) : base(Target)
	{
		CppStandard = CppStandardVersion.Latest; 
		bEnableNonInlinedGenCppWarnings = true;
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PublicDependencyModuleNames.AddRange(new[]
			{
				"Core", "UMG"
				// Bomber modules
				, "Bomber" // Is included in header files
				, "SettingsWidgetConstructor" // SettingWidgetConstructor
			}
		);

		PrivateDependencyModuleNames.AddRange(new[]
			{
				"CoreUObject", "Engine", "Slate", "SlateCore", // Core
				// Bomber modules 
				"MyUtils" 
				, "PoolManager" // Star and Widget Actors
				, "MetaCheatManager" // PSCheatExtension
			}
		);
	}
}