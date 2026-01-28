#include "pch.h"
#include "Settings.h"
#include "../utils/IniReader.h"

eSettingsManager* SettingsMgr = new eSettingsManager;

eSettingsManager::eSettingsManager()
{
	CIniReader ini("udawnhook.ini");
	CIniReader user("udawnhook_user.ini");

	bEnableConsoleWindow = ini.ReadBoolean("Misc", "EnableConsoleWindow", false);
	bEnableDebugMenu.store(ini.ReadBoolean("General", "EnableDebugMenu", false), std::memory_order_relaxed);
	menuCharacter.bEnableCustomMenuCharacter = ini.ReadBoolean("General", "EnableCustomMenuCharacter", false);
	snprintf(menuCharacter.szMenuCharacter, sizeof(menuCharacter.szMenuCharacter), "%s", ini.ReadString("General", "MenuCharacter", "Ashley"));
	snprintf(menuCharacter.szMenuVariant, sizeof(menuCharacter.szMenuVariant), "%s", ini.ReadString("General", "MenuVariant", "Ashley_intro"));
	snprintf(menuCharacter.szMenuEmotion, sizeof(menuCharacter.szMenuEmotion), "%s", ini.ReadString("General", "MenuEmotion", "Relaxed"));

	keys.iHookMenuOpenKey = user.ReadInteger("General", "HookMenuOpenKey", VK_INSERT);

	fMenuScale = user.ReadFloat("General", "MenuScale", 1.0f);

	if (fMenuScale < 1.0f)
		fMenuScale = 1.0f;

	keys.iToggleFreeCameraKey = user.ReadInteger("General", "ToggleFreeCameraKey", VK_F1);
	keys.iToggleFirstPersonCamKey = user.ReadInteger("General", "ToggleFirstPersonCamKey", VK_F2);
	keys.iToggleFreezeTime = user.ReadInteger("General", "ToggleFreezeTime", VK_F3);
	keys.iToggleSprinting = user.ReadInteger("General", "ToggleSprinting", VK_F4);

	//freecam
	keys.freeCam.iFreeCameraKeyForward = user.ReadInteger("Freecam", "FreeCameraKeyForward", 0x57);
	keys.freeCam.iFreeCameraKeyBackward = user.ReadInteger("Freecam", "FreeCameraKeyBackward", 0x53);
	keys.freeCam.iFreeCameraKeyLeft = user.ReadInteger("Freecam", "FreeCameraKeyLeft", 0x41);
	keys.freeCam.iFreeCameraKeyRight = user.ReadInteger("Freecam", "FreeCameraKeyRight", 0x44);
	keys.freeCam.iFreeCameraKeyUp = user.ReadInteger("Freecam", "FreeCameraKeyUp", 0x45);
	keys.freeCam.iFreeCameraKeyDown = user.ReadInteger("Freecam", "FreeCameraKeyDown", 0x51);
	keys.freeCam.iFreeCameraKeyYawPlus = user.ReadInteger("Freecam", "FreeCameraKeyYaw+", VK_RIGHT);
	keys.freeCam.iFreeCameraKeyYawMinus = user.ReadInteger("Freecam", "FreeCameraKeyYaw-", VK_LEFT);
	keys.freeCam.iFreeCameraKeyRollPlus = user.ReadInteger("Freecam", "FreeCameraKeyRoll+", 0x43);
	keys.freeCam.iFreeCameraKeyRollMinus = user.ReadInteger("Freecam", "FreeCameraKeyRoll-", 0x5A);
	keys.freeCam.iFreeCameraKeyPitchPlus = user.ReadInteger("Freecam", "FreeCameraKeyPitch+", VK_UP);
	keys.freeCam.iFreeCameraKeyPitchMinus = user.ReadInteger("Freecam", "FreeCameraKeyPitch-", VK_DOWN);
	keys.freeCam.iFreeCameraKeyReset = user.ReadInteger("Freecam", "FreeCameraKeyReset", VK_BACK);

	//misc
	keys.iSprint = user.ReadInteger("Misc", "Sprint", VK_SHIFT);
}

void eSettingsManager::SaveMenuCharSettings(const MenuCharacter& menuChar)
{
	CIniReader ini("udawnhook.ini");
	ini.WriteBoolean("General", "EnableCustomMenuCharacter", menuChar.bEnableCustomMenuCharacter);
	ini.WriteString("General", "MenuCharacter", menuChar.szMenuCharacter);
	ini.WriteString("General", "MenuVariant", menuChar.szMenuVariant);
	ini.WriteString("General", "MenuEmotion", menuChar.szMenuEmotion);
}

void eSettingsManager::SaveSettings()
{
	CIniReader ini("udawnhook.ini");
	ini.WriteBoolean("Misc", "EnableConsoleWindow", bEnableConsoleWindow);
	ini.WriteBoolean("General", "EnableDebugMenu", bEnableDebugMenu.load(std::memory_order_relaxed));

	CIniReader user("udawnhook_user.ini");
	user.WriteFloat("General", "MenuScale", fMenuScale);

	user.WriteInteger("General", "HookMenuOpenKey", keys.iHookMenuOpenKey);
	user.WriteInteger("General", "ToggleFreeCameraKey", keys.iToggleFreeCameraKey);
	user.WriteInteger("General", "ToggleFirstPersonCamKey", keys.iToggleFirstPersonCamKey);
	user.WriteInteger("General", "ToggleFreezeTime", keys.iToggleFreezeTime);
	user.WriteInteger("General", "ToggleSprinting", keys.iToggleSprinting);

	user.WriteInteger("Freecam", "FreeCameraKeyForward", keys.freeCam.iFreeCameraKeyForward);
	user.WriteInteger("Freecam", "FreeCameraKeyBackward", keys.freeCam.iFreeCameraKeyBackward);
	user.WriteInteger("Freecam", "FreeCameraKeyLeft", keys.freeCam.iFreeCameraKeyLeft);
	user.WriteInteger("Freecam", "FreeCameraKeyRight", keys.freeCam.iFreeCameraKeyRight);
	user.WriteInteger("Freecam", "FreeCameraKeyUp", keys.freeCam.iFreeCameraKeyUp);
	user.WriteInteger("Freecam", "FreeCameraKeyDown", keys.freeCam.iFreeCameraKeyDown);
	user.WriteInteger("Freecam", "FreeCameraKeyYaw+", keys.freeCam.iFreeCameraKeyYawPlus);
	user.WriteInteger("Freecam", "FreeCameraKeyYaw-", keys.freeCam.iFreeCameraKeyYawMinus);
	user.WriteInteger("Freecam", "FreeCameraKeyRoll+", keys.freeCam.iFreeCameraKeyRollPlus);
	user.WriteInteger("Freecam", "FreeCameraKeyRoll-", keys.freeCam.iFreeCameraKeyRollMinus);
	user.WriteInteger("Freecam", "FreeCameraKeyPitch+", keys.freeCam.iFreeCameraKeyPitchPlus);
	user.WriteInteger("Freecam", "FreeCameraKeyPitch-", keys.freeCam.iFreeCameraKeyPitchMinus);
	user.WriteInteger("Freecam", "FreeCameraKeyReset", keys.freeCam.iFreeCameraKeyReset);

	user.WriteInteger("Misc", "Sprint", keys.iSprint);
}

void eSettingsManager::ResetKeys()
{
	std::lock_guard<std::mutex> lock(mtx_keys);

	keys.iHookMenuOpenKey = VK_INSERT;
	keys.iToggleFreeCameraKey = VK_F1;
	keys.iToggleFirstPersonCamKey = VK_F2;
	keys.iToggleFreezeTime = VK_F3;
	keys.iToggleSprinting = VK_F4;
	keys.freeCam.iFreeCameraKeyForward = 0x57;
	keys.freeCam.iFreeCameraKeyBackward = 0x53;
	keys.freeCam.iFreeCameraKeyLeft = 0x41;
	keys.freeCam.iFreeCameraKeyRight = 0x44;
	keys.freeCam.iFreeCameraKeyUp = 0x45;
	keys.freeCam.iFreeCameraKeyDown = 0x51;
	keys.freeCam.iFreeCameraKeyYawPlus = VK_RIGHT;
	keys.freeCam.iFreeCameraKeyYawMinus = VK_LEFT;
	keys.freeCam.iFreeCameraKeyPitchPlus = VK_UP;
	keys.freeCam.iFreeCameraKeyPitchMinus = VK_DOWN;
	keys.freeCam.iFreeCameraKeyRollPlus = 0x43;
	keys.freeCam.iFreeCameraKeyRollMinus = 0x5A;
	keys.freeCam.iFreeCameraKeyReset = VK_BACK;
	keys.iSprint = VK_SHIFT;
}
