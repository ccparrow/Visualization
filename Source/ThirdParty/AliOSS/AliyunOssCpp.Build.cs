// Fill out your copyright notice in the Description page of Project Settings.

using System.IO;
using UnrealBuildTool;

public class AliyunOssCpp : ModuleRules
{
	public AliyunOssCpp(ReadOnlyTargetRules Target) : base(Target)
	{
		Type = ModuleType.External;

		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
            PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "include"));

            // Add the import library
            // PublicLibraryPaths.Add(Path.Combine(ModuleDirectory, "lib", "Release"));
            //PublicAdditionalLibraries.Add("alibabacloud-oss-cpp-sdk.lib");

            string path = Path.Combine(ModuleDirectory, "lib", "Release", "alibabacloud-oss-cpp-sdk.lib");

            PublicAdditionalLibraries.Add(path);
        }

        AddEngineThirdPartyPrivateStaticDependencies(Target, new string[]
        {
            "libcurl",
            "OpenSSL",
//           "zlib"
        });
    }
}
