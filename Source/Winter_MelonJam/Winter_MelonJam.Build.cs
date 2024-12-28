// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Winter_MelonJam : ModuleRules
{
	public Winter_MelonJam(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG" });
	}
}
