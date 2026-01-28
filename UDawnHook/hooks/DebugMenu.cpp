#include "pch.h"
#include "hooks.h"
#include "unreal/UObject.h"
#include "plugin/Settings.h"

using namespace Hooks::DebugMenu;

void Hooks::DebugMenu::UWidget_SetVisibility_Hook(int64 a1, uint8_t a2)
{
	if (SettingsMgr->bEnableDebugMenu.load(std::memory_order_relaxed))
	{
		static FName button_dbg("Button_DEBUG", FNAME_Add);
		if (reinterpret_cast<UObject*>(a1)->GetFName() == button_dbg)
			a2 = 0; //show the debug menu
	}

	if (oUWidget_SetVisibility)
		oUWidget_SetVisibility(a1, a2);
}