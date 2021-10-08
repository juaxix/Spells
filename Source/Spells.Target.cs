// Spells - xixgames - juaxix - 2021

using UnrealBuildTool;
using System.Collections.Generic;

public class SpellsTarget : TargetRules
{
	public SpellsTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;

		ExtraModuleNames.AddRange( new string[] { "Spells" } );
	}
}
