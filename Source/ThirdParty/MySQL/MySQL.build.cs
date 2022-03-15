// Fill out your copyright notice in the Description page of Project Settings.

using System.IO;
using UnrealBuildTool;

public class MySQL: ModuleRules
{
	public MySQL(ReadOnlyTargetRules Target) : base(Target)
	{
		Type = ModuleType.External;

        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "MySQL_64", "include"));

        if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			// Add the import library
			PublicLibraryPaths.Add(Path.Combine(ModuleDirectory, "MySQL_64", "lib")); 
			PublicAdditionalLibraries.Add("mariadbclient.lib");	

					
		}	
	}
}
