// Fill out your copyright notice in the Description page of Project Settings.

using System.IO;
using UnrealBuildTool;

public class AliyunOSS : ModuleRules
{
	public AliyunOSS(ReadOnlyTargetRules Target) : base(Target)
	{
		Type = ModuleType.External;

		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
            PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "include"));


            string path = Path.Combine(ModuleDirectory, "lib", "Release", "alibabacloud-oss-cpp-sdk.lib");

            PublicAdditionalLibraries.Add(path);
        }

        AddEngineThirdPartyPrivateStaticDependencies(Target, new string[]
        {
            "libcurl",
            "OpenSSL",
        });
    }
}
