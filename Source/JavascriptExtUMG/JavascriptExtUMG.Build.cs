// 

using UnrealBuildTool;

public class JavascriptExtUMG : ModuleRules
{
	public JavascriptExtUMG(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.AddRange(new string[] { 
            "Core", 
            "CoreUObject", 
            "Engine", 
            "InputCore", 
            "Slate",
			"SlateCore",
            "ShaderCore",
			"RenderCore",
			"RHI", 
            "UMG",
            "V8"
        });
	}
}
