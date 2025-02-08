// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class BerserkGame : ModuleRules
{
	public BerserkGame(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "HeadMountedDisplay",
            "AIModule",
            "GameplayCameras",
            "GameplayTasks", //Tasks
			"NavigationSystem",
            "UMG", //UI
			"Slate",
            "SlateCore"
        });
    }
}
