#pragma once
#include <atomic>
#include <mutex>

struct eMouseSettings {
	int sens;
	bool invert_y;
	bool invert_x;
};



class eSettingsManager {
public:
	eSettingsManager();

	bool				bEnableConsoleWindow;
	std::atomic<bool>	bEnableDebugMenu;

	struct MenuCharacter
	{
		bool			bEnableCustomMenuCharacter;
		char			szMenuCharacter[256];
		char			szMenuVariant[256];
		char			szMenuEmotion[256];
	};
	MenuCharacter		menuCharacter;

	struct Keys
	{
		int					iHookMenuOpenKey;
		int					iToggleFreeCameraKey;
		int					iToggleFirstPersonCamKey;
		int					iToggleFreezeTime;
		int					iToggleSprinting;

		struct FreeCam 
		{
			int					iFreeCameraKeyForward;
			int					iFreeCameraKeyBackward;
			int					iFreeCameraKeyLeft;
			int					iFreeCameraKeyRight;
			int					iFreeCameraKeyUp;
			int					iFreeCameraKeyDown;

			int					iFreeCameraKeyYawPlus;
			int					iFreeCameraKeyYawMinus;

			int					iFreeCameraKeyPitchPlus;
			int					iFreeCameraKeyPitchMinus;

			int					iFreeCameraKeyRollPlus;
			int					iFreeCameraKeyRollMinus;

			int					iFreeCameraKeyReset;
		};
		FreeCam				freeCam;

		int					iSprint;
	};
	Keys					keys;
	std::mutex			mtx_keys;

	float				fMenuScale;

	void SaveMenuCharSettings(const MenuCharacter& menuChar);
	void SaveSettings();
	void ResetKeys();
};

extern eSettingsManager* SettingsMgr;