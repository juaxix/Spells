// Spells - xixgames - juaxix - 2021/2022

using UnrealBuildTool;
using System.Collections.Generic;

public class SpellsTarget : TargetRules
{
	public SpellsTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.AddRange( new string[] { "Spells" } );
	}
}
