#include "pch.h"
#include "hooks.h"
#include "plugin/Settings.h"
#include "helper/eKeyboardMan.h"
#include "unreal/UObject.h"
#include "plugin/Menu.h"

using namespace Hooks::Locomotion;

static bool bStartedSprinting = false;
static bool bIsSprinting = false;
static bool bStoppedSprinting = false;

enum eCharacterLocoModes : uint8_t
{
	WalkSlow,
	WalkRegular,
	WalkFast,
	Jogging,
	Running,
	Injured,
	Wading,
	COUNT
};

void Hooks::Locomotion::HandleSprinting()
{
	int keyState;
	{
		std::lock_guard<std::mutex> keys_lock(SettingsMgr->mtx_keys);

		keyState = eKeyboardMan::GetKeyState(SettingsMgr->keys.iSprint);
	}

	UObject*& movementComp = *reinterpret_cast<UObject**>(pCharacter + 0x330);
	if (TheMenu->m_iSprintingEnabled.load(std::memory_order_relaxed) && (keyState & KEY_JUST_PRESSED))
	{
		bStartedSprinting = true;
	}
	else if (keyState == 0)
	{
		if (bIsSprinting)
		{
			bIsSprinting = false;
			bStoppedSprinting = true;
		}
	}

	static uintptr_t pat = _pattern(PATID_SetCharacterLocoMode);
	typedef void(*fnSetCharacterMode)(UObject*, uint8_t, bool, bool);
	if (bStartedSprinting)
	{
		((fnSetCharacterMode)pat)(movementComp, Running, 1, 1);
		bIsSprinting = true;
		bStartedSprinting = false;
	}
	else if (bStoppedSprinting)
	{
		((fnSetCharacterMode)pat)(movementComp, WalkFast, 1, 1);
		bStoppedSprinting = false;
	}
}

// Supress state driven stuff while sprinting
void Hooks::Locomotion::StateDriven_SetCharacterMode_Hook(int64 a1, uint8_t a2, bool a3, bool a4)
{
	if (bIsSprinting)
		return;

	if (oStateDriven_SetCharacterMode)
		oStateDriven_SetCharacterMode(a1, a2, a3, a4);
}

void Hooks::Locomotion::StateDriven_PushCharacterMode_Hook(int64 a1, int64 a2, uint8_t a3, bool a4)
{
	if (bIsSprinting)
		return;

	if (oStateDriven_PushCharacterMode)
		oStateDriven_PushCharacterMode(a1, a2, a3, a4);
}