// Spells - xixgames - juaxix - 2021

using UnrealBuildTool;
using System.Collections.Generic;

public class SpellsEditorTarget : TargetRules
{
	public SpellsEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;

		ExtraModuleNames.AddRange( new string[] { "Spells" } );
	}
}
