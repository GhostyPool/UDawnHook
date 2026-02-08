// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "plugin/Settings.h"
#include "plugin/Menu.h"

#include "gui/dx12hook.h"
#include "gui/gui_impl.h"

#include "hooks/hooks.h"

using namespace Memory::VP;

static bool ValidateGameVersion();

extern "C" __declspec(dllexport) void InitializeASI()
{
    g_MHStatus = MH_Initialize();
    eLog::Initialize();
    PatternSolver::Initialize();

    if (!ValidateGameVersion())
        return;

    if (SettingsMgr->bEnableConsoleWindow)
    {
        AllocConsole();
        freopen("CONOUT$", "w", stdout);
        freopen("CONOUT$", "w", stderr);
    }

    eLog::Message(__FUNCTION__, "INFO: UDawnHook (%s | %s) Begin!", UNTILDAWN_HOOK_VERSION, __DATE__);

    Trampoline* tramp = Trampoline::MakeTrampoline(GetModuleHandle(nullptr));
    ReadCall(_pattern(PATID_FEngineLoop_Tick), Hooks::FEngineLoop::oFEngineLoop_Tick);
    InjectHook(_pattern(PATID_FEngineLoop_Tick), tramp->Jump(Hooks::FEngineLoop::Tick_Hook));

    ReadCall(_pattern(PATID_UWidget_SetVisibility), Hooks::DebugMenu::oUWidget_SetVisibility);
    InjectHook(_pattern(PATID_UWidget_SetVisibility), tramp->Jump(Hooks::DebugMenu::UWidget_SetVisibility_Hook));
    
    MH_CreateHook((void*)_pattern(PATID_AppendActiveMorphTargets), &Hooks::Morphs::AppendActiveMorphTargets_Hook, (void**)&Hooks::Morphs::oAppendActiveMorphTargets);
    MH_EnableHook((void*)_pattern(PATID_AppendActiveMorphTargets));

    MH_CreateHook((void*)_pattern(PATID_MenuActor_Activate), &Hooks::MenuActor::Activate_Hook, (void**)&Hooks::MenuActor::oMenuActor_Activate);
    MH_EnableHook((void*)_pattern(PATID_MenuActor_Activate));

    MH_CreateHook((void*)_pattern(PATID_GetCharacterByCharacterVariant), &Hooks::Characters::GetCharacterByCharacterVariant_Hook, (void**)&Hooks::Characters::oGetCharacterByCharacterVariant);
    MH_EnableHook((void*)_pattern(PATID_GetCharacterByCharacterVariant));

    ReadCall(_pattern(PATID_GetCharacterSoftClassByCharacterAndVariant_StreamingManager), Hooks::Characters::oGetCharacterSoftClassByCharacterAndVariant);
    InjectHook(_pattern(PATID_GetCharacterSoftClassByCharacterAndVariant_StreamingManager), tramp->Jump(Hooks::Characters::GetCharacterSoftClassByCharacterAndVariant_Hook));

    //So default reset mode doesnt reset save game
    Patch<int>(_pattern(PATID_DebugResetMode), 0);

    MH_CreateHook((void*)_pattern(PATID_DoUpdateCamera), &Hooks::Camera::DoUpdateCamera_Hook, (void**)&Hooks::Camera::oDoUpdateCamera);
    MH_EnableHook((void*)_pattern(PATID_DoUpdateCamera));

    MH_CreateHook((void*)_pattern(PATID_OverridePostProcessSettings), &Hooks::PostProcess::OverridePostProcessSettings_Hook, (void**)&Hooks::PostProcess::oOverridePostProcessSettings);
    MH_EnableHook((void*)_pattern(PATID_OverridePostProcessSettings));

    //ReadCall(_pattern(PATID_FMatrix_MakeFromX_call), ((void(*&)(int64, int64))PatternSolver::ms_patterns[PATID_FMatrix_MakeFromX]));

    MH_CreateHook((void*)_pattern(PATID_StateDriven_SetCharacterMode), &Hooks::Locomotion::StateDriven_SetCharacterMode_Hook, (void**)&Hooks::Locomotion::oStateDriven_SetCharacterMode);
    MH_EnableHook((void*)_pattern(PATID_StateDriven_SetCharacterMode));

    MH_CreateHook((void*)_pattern(PATID_StateDriven_PushCharacterMode), &Hooks::Locomotion::StateDriven_PushCharacterMode_Hook, (void**)&Hooks::Locomotion::oStateDriven_PushCharacterMode);
    MH_EnableHook((void*)_pattern(PATID_StateDriven_PushCharacterMode));

    HANDLE h = 0;

    h = CreateThread(NULL, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(DX12Hook_Thread), 0, NULL, 0);

    if (!(h == nullptr)) CloseHandle(h);

}

static bool ValidateGameVersion()
{
    if (PatternSolver::CheckMissingPatterns())
    {
        int result = MessageBoxW(nullptr, 
            L"Could not start UDawnHook!\n\n"
            L"One or more code patterns could not be found, this might indicate the game version is not supported or the plugin has not been updated yet.\n\n"
            L"You might experience bugs or crashes.\n"
            L"Launch anyway?\n",
            L"UDawnHook",
            MB_ICONWARNING | MB_YESNO | MB_SYSTEMMODAL);

        if (result == IDYES)
            return true;
        else
            ExitProcess(1);
    }
    return true;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

