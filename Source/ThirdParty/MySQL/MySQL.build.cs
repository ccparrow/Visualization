// Fill out your copyright notice in the Description page of Project Settings.

using System.IO;
using UnrealBuildTool;

public class MySQL: ModuleRules
{
	public MySQL(ReadOnlyTargetRules Target) : base(Target)
	{
		Type = ModuleType.External;
		
        if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "MySQL_64", "include"));
			PublicSystemLibraryPaths.Add(Path.Combine(ModuleDirectory, "MySQL_64", "lib")); 
			PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "MySQL_64", "lib", "mariadbclient.lib"));

		}	
	}
}
