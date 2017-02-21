// Fill out your copyright notice in the Description page of Project Settings.
using System;
using System.IO;

using UnrealBuildTool;

public class Smile : ModuleRules
{
    private string ModulePath
    {
        get { return ModuleDirectory; }
    }

    private string ThirdPartyPath
    {
        get { return Path.GetFullPath(Path.Combine(ModulePath, "../../ThirdParty/")); }
    }

    private string BinariesPath
    {
        get { return Path.GetFullPath(Path.Combine(ModulePath, "../../Binaries/")); }
    }

    public bool LoadOpenCV(TargetInfo Target)
    {
        if (Target.Platform != UnrealTargetPlatform.Win64)
        {
            Console.WriteLine("{0} dedicated server is made to depend on {1}. We want to avoid this, please correct module dependencies.", Target.Platform, this.ToString());
            return false;
        }

        var platform = Target.Platform.ToString();

        // Paths
        var openCVPath = Path.Combine(ThirdPartyPath, "OpenCV");
        var libPath = Path.Combine(openCVPath, "Libraries", platform);

        // Options
        var debug = Target.Configuration == UnrealTargetConfiguration.Debug && BuildConfiguration.bDebugBuildsActuallyUseDebugCRT;

        //Add Include path
        PrivateIncludePaths.AddRange(new string[] { Path.Combine(openCVPath, "Includes") });

        // Add Library Path
        PublicLibraryPaths.Add(libPath);

        // Add Dependencies
        var dependencies = new[] { "videoio", "core", "face", "imgcodecs", "imgproc" };
        var version = "320";
        var binariesPath = Path.Combine(BinariesPath, platform);
        Directory.CreateDirectory(binariesPath);
        foreach (var dependency in dependencies)
        {
            var name = string.Format("opencv_{0}{1}{2}", dependency, version, debug ? "d" : "");
            var lib = string.Format("{0}.lib", name);
            var dll = string.Format("{0}.dll", name);

            PublicAdditionalLibraries.Add(lib);
            PublicDelayLoadDLLs.Add(dll);
            File.Copy(Path.Combine(libPath, dll), Path.Combine(binariesPath, dll), true);
        }

        Definitions.Add("WITH_OPENCV_BINDING=1");

        return true;
    }

    public Smile(TargetInfo Target)
	{
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
        LoadOpenCV(Target);
    }
}
