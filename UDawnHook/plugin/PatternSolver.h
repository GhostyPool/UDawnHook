#pragma once
#include "..\pch.h"

enum EPatternID {
	PATID_FEngineLoop_Tick,

	PATID_AppendActiveMorphTargets,

	PATID_FName_ctor_char,
	PATID_FName_ctor_wchar,
	PATID_FName_ToString,

	PATID_FString_ctor_char,
	PATID_FString_ctor_wchar,
	PATID_FString_dtor,
	PATID_FString_ResizeTo,

	PATID_AActor_GetWorld,
	PATID_UWorld_GetWorldSettings,

	PATID_MenuActor_Activate,

	PATID_GetCharacterByCharacterVariant,
	PATID_GetCharacterSoftClassByCharacterAndVariant_StreamingManager,

	PATID_DebugResetMode,
	PATID_UWidget_SetVisibility,

	PATID_DoUpdateCamera,
	PATID_OverridePostProcessSettings,

	PATID_USkeletalMeshComponent_GetBoneMatrix,

	//PATID_FMatrix_MakeFromX_call,
	//PATID_FMatrix_MakeFromX,
	PATID_FMatrix_Rotator,

	PATID_SetCharacterLocoMode,
	PATID_StateDriven_SetCharacterMode,
	PATID_StateDriven_PushCharacterMode,

	PATID_Total_Patterns
};


class PatternSolver {
public:
	static uintptr_t ms_patterns[PATID_Total_Patterns];

	static uintptr_t GetPattern(const char* szPattern, int offset);

	static void			Initialize();
	static int			GetNumPatternsOK();
	static bool			CheckMissingPatterns();
	static const char*	GetPatternName(int id);

};


uintptr_t _pattern(EPatternID id);