// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Cyberbowl : ModuleRules
{
	public Cyberbowl(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
       
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "OnlineSubsystem", "OnlineSubsystemUtils", "Steamworks", "Niagara", "FMODStudio" });

        PrivateDependencyModuleNames.Add("OnlineSubsystem");
	}
}
