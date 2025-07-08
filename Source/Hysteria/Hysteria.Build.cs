// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class Hysteria : ModuleRules
{
	public Hysteria(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore"});

		PrivateDependencyModuleNames.AddRange(new string[] { "AIModule" });
		PrivateDependencyModuleNames.AddRange(new string[] { "NavigationSystem" });
		PrivateDependencyModuleNames.AddRange(new string[] { "UMG" });
		PrivateDependencyModuleNames.AddRange(new string[] { "Slate" });
		PrivateDependencyModuleNames.AddRange(new string[] { "SlateCore" });
		PublicDefinitions.Add("HYSTERIA_USE_UNREAL");
		PublicDefinitions.Add("_HAS_EXCEPTIONS=1");
		bEnableExceptions = true;
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
