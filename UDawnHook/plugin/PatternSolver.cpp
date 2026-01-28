#include "PatternSolver.h"
#include "..\utils\Patterns.h"
#include "..\gui\log.h"
#include <chrono>

uintptr_t PatternSolver::ms_patterns[PATID_Total_Patterns];


uintptr_t PatternSolver::GetPattern(const char* szPattern, int offset)
{
    uintptr_t addr = 0;
    try {
        addr = (uintptr_t)hook::txn::get_pattern(szPattern, offset);
    }
    TXN_CATCH();

    return addr;
}

void PatternSolver::Initialize()
{
    eLog::Message(__FUNCTION__, "Starting pattern search");

    for (int i = 0; i < PATID_Total_Patterns; i++)
        ms_patterns[i] = 0;

    auto begin = std::chrono::high_resolution_clock::now();

    ms_patterns[PATID_FEngineLoop_Tick] = GetPattern("E8 ? ? ? ? 80 3D ? ? ? ? 00 74 ? 80 3D ? ? ? ? 00 75", 0);

    ms_patterns[PATID_AppendActiveMorphTargets] = GetPattern("48 85 C9 0F 84 ? ? ? ? 4C 8B DC 55 56 41 54", 0);

    ms_patterns[PATID_FName_ctor_char] = GetPattern("48 89 5C 24 ? 56 48 83 EC ? 48 89 54 24", 0);
    ms_patterns[PATID_FName_ctor_wchar] = GetPattern("48 89 5C 24 ? 57 48 83 EC ? 48 8B D9 48 89 54 24 ? 33 C9", 0);
    ms_patterns[PATID_FName_ToString] = GetPattern("48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC ? 83 79 ? 00 48 8B FA ? ? 48 8B F1", 0);

    ms_patterns[PATID_FString_ctor_char] = GetPattern("48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 41 56 48 83 EC ? 33 ED 48 8B DA ? ? ? 4C 8B F1", 0);
    ms_patterns[PATID_FString_ctor_wchar] = GetPattern("48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC ? 33 C0 48 8B F2 ? ? ? 48 8B F9 48 89 41 ? 48 85 D2 74 ? ? ? ? 74 ? 48 C7 C3 ? ? ? ? 90 48 FF C3 ? ? ? ? 75 ? 83 C3", 0);
    ms_patterns[PATID_FString_dtor] = GetPattern("40 53 48 83 EC ? ? ? ? 48 85 DB 74 ? 48 8B 0D", 0);
    ms_patterns[PATID_FString_ResizeTo] = GetPattern("48 89 5C 24 ? 57 48 83 EC ? 48 63 DA 48 8B F9 85 D2 74 ? 48 8B CB BA ? ? ? ? 48 03 C9 E8 ? ? ? ? ? ? ? B9 ? ? ? ? 3B D8 0F 4F C1 8B D8 3B 5F ? 74 ? 8B 57 ? 41 B9 ? ? ? ? 44 8B C3 89 5F ? 48 8B CF C7 44 24 ? ? ? ? ? E8 ? ? ? ? 48 8B 5C 24 ? 48 83 C4 ? 5F C3 ? ? ? ? ? ? ? ? ? ? ? ? ? ? 40 53", 0);

    ms_patterns[PATID_AActor_GetWorld] = GetPattern("40 53 48 83 EC ? 8B 41 ? 48 8B D9 C1 E8 ? A8 ? 75 ? 48 8B 41 ? 48 85 C0", 0);
    ms_patterns[PATID_UWorld_GetWorldSettings] = GetPattern("48 89 6C 24 ? 48 89 74 24 ? 57 48 83 EC ? 48 8B F9 41 0F B6 F0 48 8B 49 ? 0F B6 EA", 0);

    ms_patterns[PATID_MenuActor_Activate] = GetPattern("4C 8B DC 41 56 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 ? ? ? ? ? ? ? 4C 8B F1 49 89 5B", 0);

    ms_patterns[PATID_GetCharacterByCharacterVariant] = GetPattern("48 8B C4 56 48 83 EC ? 48 89 58 ? 4D 8B C8", 0);
    ms_patterns[PATID_GetCharacterSoftClassByCharacterAndVariant] = GetPattern("48 89 5C 24 ? 48 89 6C 24 ? 56 57 41 56 48 83 EC ? ? ? ? 48 8B F1 8B CB 4D 8B F1", 0);

    ms_patterns[PATID_DebugResetMode] = GetPattern("C7 83 ? ? ? ? ? ? ? ? C7 83 ? ? ? ? ? ? ? ? 48 89 BB ? ? ? ? 48 89 BB ? ? ? ? 89 BB ? ? ? ? C7 83", 6);
    ms_patterns[PATID_UWidget_SetVisibility] = GetPattern("E9 ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? 48 8B C4 48 89 58 ? 57 48 83 EC ? 48 89 70", 0);

    ms_patterns[PATID_DoUpdateCamera] = GetPattern("4C 8B DC 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 ? ? ? ? 49 89 5B ? 48 8D 91", 0);
    ms_patterns[PATID_OverridePostProcessSettings] = GetPattern("48 8B C4 55 53 41 55 48 8D 68 ? 48 81 EC ? ? ? ? 44 0F 29 40", 0);

    ms_patterns[PATID_USkeletalMeshComponent_GetBoneMatrix] = GetPattern("48 8B C4 48 89 58 ? 48 89 70 ? 57 48 81 EC ? ? ? ? 0F 29 70 ? 0F 29 78 ? 44 0F 29 40 ? 44 0F 29 48 ? 44 0F 29 50 ? 44 0F 29 58 ? 44 0F 29 60 ? 44 0F 29 A8 ? ? ? ? 48 8B 05", 0);

    //ms_patterns[PATID_FMatrix_MakeFromX_call] = GetPattern("E8 ? ? ? ? 48 8D 55 ? 48 8B C8 E8 ? ? ? ? 41 8D 45", 0);
    //ms_patterns[PATID_FMatrix_MakeFromX] = ms_patterns[PATID_FMatrix_MakeFromX_call]; //placeholder until call gets read
    ms_patterns[PATID_FMatrix_Rotator] = GetPattern("48 8B C4 53 48 81 EC ? ? ? ? 0F 29 70 ? 0F 29 78 ? 44 0F 29 40 ? 44 0F 29 48 ? 44 0F 29 50 ? 44 0F 29 58 ? 44 0F 29 60 ? 44 0F 29 A8 ? ? ? ? 44 0F 29 B0 ? ? ? ? 44 0F 29 B8 ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 ? ? ? ? ? ? ? ? ? 48 8B DA", 0);

    ms_patterns[PATID_SetCharacterLocoMode] = GetPattern("40 57 48 83 EC ? 48 63 81", 0);
    ms_patterns[PATID_StateDriven_SetCharacterMode] = GetPattern("40 57 48 83 EC ? 48 89 5C 24 ? 84 D2", 0);
    ms_patterns[PATID_StateDriven_PushCharacterMode] = GetPattern("48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 48 83 EC ? 45 84 C0 45 0F B6 D0", 0);


    auto end = std::chrono::high_resolution_clock::now();

    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
    auto timeSeconds = std::chrono::duration_cast<std::chrono::seconds>(end - begin);
    eLog::Message(__FUNCTION__, "Checked %d patterns in %dms (%ds)", PATID_Total_Patterns, time.count(), timeSeconds.count());

}

int PatternSolver::GetNumPatternsOK()
{
    int patternNum = 0;

    for (int i = 0; i < PATID_Total_Patterns; i++)
        if (ms_patterns[i]) patternNum++;
    return patternNum;
}

bool PatternSolver::CheckMissingPatterns()
{
    int missingPatterns = 0;
    for (int i = 0; i < PATID_Total_Patterns; i++)
        if (ms_patterns[i] == 0)
        {
            missingPatterns++;
            eLog::Message(__FUNCTION__, "ERROR: Could not find %s!", GetPatternName(i));
        }
    return missingPatterns > 0;
}

const char* PatternSolver::GetPatternName(int id)
{
    if (id >= PATID_Total_Patterns)
        return "UNKNOWN";

    static const char* szPatternNames[PATID_Total_Patterns] = {
        "FEngineLoop_Tick",
        "AppendActiveMorphTargets",
        "FName_ctor_char",
        "FName_ctor_wchar",
        "FName_ToString",
        "FString_ctor_char",
        "FString_ctor_wchar",
        "FString_dtor",
        "FString_ResizeTo",
        "AActor_GetWorld",
        "UWorld_GetWorldSettings",
        "MenuActor_Activate",
        "GetCharacterByCharacterVariant",
        "GetCharacterSoftClassByCharacterAndVariant",
        "DebugResetMode",
        "UWidget_SetVisibility",
        "DoUpdateCamera",
        "OverridePostProcessSettings",
        "USkeletalMeshComponent_GetBoneMatrix",
        "FMatrix_Rotator",
        "SetCharacterLocoMode",
        "StateDriven_SetCharacterLocoMode",
        "StateDriven_PushCharacterMode"
    };   

    return szPatternNames[id];
}

uintptr_t _pattern(EPatternID id)
{
    if (id >= PATID_Total_Patterns)
        return 0;

    return PatternSolver::ms_patterns[id];
}
