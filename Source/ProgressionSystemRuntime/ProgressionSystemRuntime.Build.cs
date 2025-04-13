// Copyright (c) Valeriy Rotermel and Yevhenii Selivanov

using UnrealBuildTool;

public class ProgressionSystemRuntime : ModuleRules
{
	public ProgressionSystemRuntime(ReadOnlyTargetRules Target) : base(Target)
	{
		if (Target.bBuildEditor)
		{
			//no optimization for editor to be applied for debug 
			PCHUsage = PCHUsageMode.NoPCHs;
			CppStandard = CppStandardVersion.Latest;
			bEnableNonInlinedGenCppWarnings = true;
			OptimizeCode = CodeOptimization.Never; 
			bUseUnity = false;
		}
		else
		{
			CppStandard = CppStandardVersion.Latest;
			bEnableNonInlinedGenCppWarnings = true;
			PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		}

		PublicDependencyModuleNames.AddRange(new[]
			{
				"Core", "UMG"
				// Bomber modules
				,
				"Bomber", "SettingsWidgetConstructor"
			}
		);

		PrivateDependencyModuleNames.AddRange(new[]
			{
				"CoreUObject", "Engine", "Slate", "SlateCore" // Core
				,
				"GameplayTags" // UE_DEFINE_GAMEPLAY_TAG_STATIC
				// Bomber modules 
				,
				"MyUtils", "PoolManager" // Star and Widget Actors
				,
				"MetaCheatManager" // PSCheatExtension
			}
		);
	}
}