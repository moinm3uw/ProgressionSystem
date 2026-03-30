// Copyright (c) Valeriy Rotermel and Yevhenii Selivanov

using UnrealBuildTool;

public class ProgressionSystemRuntime : ModuleRules
{
	public ProgressionSystemRuntime(ReadOnlyTargetRules Target) : base(Target)
	{
	    CppCompileWarningSettings.NonInlinedGenCppWarningLevel = WarningLevel.Error;
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PublicDependencyModuleNames.AddRange(new[]
			{
				"Core", "UMG"
				// Bomber modules
				, "Bomber"
				, "DataAssetsLoader" // Created UPSDataAsset
				,"SettingsWidgetConstructor"
			}
		);

		PrivateDependencyModuleNames.AddRange(new[]
			{
				"CoreUObject", "Engine", "Slate", "SlateCore" // Core
				, "GameplayTags", "GameplayAbilities" // Tags
				// Bomber modules 
				, "MyUtils" 
				, "PoolManager" // Star and Widget Actors
				, "MetaCheatManager" // PSCheatExtension
			}
		);
	}
}
