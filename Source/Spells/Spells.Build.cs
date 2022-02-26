// Spells - xixgames - juaxix - 2021/2022

using UnrealBuildTool;

public class Spells : ModuleRules
{
	public Spells(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "AIModule", "GameplayTasks", "GameplayTags", "UMG", "PhotonCloudAPI"});

		PublicIncludePaths.Add(ModuleDirectory + "/Public");
		PrivateIncludePaths.Add(ModuleDirectory+ "/Private");
	}
}
