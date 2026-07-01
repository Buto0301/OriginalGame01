// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class adauchi : ModuleRules
{
	public adauchi(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"adauchi",
			"adauchi/Variant_Platforming",
			"adauchi/Variant_Platforming/Animation",
			"adauchi/Variant_Combat",
			"adauchi/Variant_Combat/AI",
			"adauchi/Variant_Combat/Animation",
			"adauchi/Variant_Combat/Gameplay",
			"adauchi/Variant_Combat/Interfaces",
			"adauchi/Variant_Combat/UI",
			"adauchi/Variant_SideScrolling",
			"adauchi/Variant_SideScrolling/AI",
			"adauchi/Variant_SideScrolling/Gameplay",
			"adauchi/Variant_SideScrolling/Interfaces",
			"adauchi/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
