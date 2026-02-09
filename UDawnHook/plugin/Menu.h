#pragma once
#include <atomic>
#include <string>
#include <mutex>
#include "CharacterInfo.h"
#include "Settings.h"
#include "../unreal/FVector.h"
#include "../unreal/FRotator.h"
#include "../unreal/APlayerCameraManager.h"

#define UNTILDAWN_HOOK_VERSION "1.1"

struct SwapRule
{
	std::string			name;
	int					id;
	struct CharacterAndVariant
	{
		std::string characterName;
		std::string variantName;
	};
	CharacterAndVariant originalCharacter;
	CharacterAndVariant swapCharacter;
};

struct CharacterOverride
{
	const char* const		characterName;
	bool* const				overrideEnabled;
	int* const				characterOverride;
	int* const				variantOverride;
};

enum eMenuSubMenus
{
	SUBMENU_SETTINGS,
	TOTAL_SUBMENUS
};

enum eCameraModes
{
	FREECAM,
	FIRST_PERSON
};

class Menu
{
public:
	std::atomic<uint8_t>	m_iIsActive = 0;
	bool					m_bSubmenuActive[TOTAL_SUBMENUS] = {};
	bool					m_bPressingKey = false;
	bool					m_bIsFocused = true;
	std::atomic<bool>		m_bWantsKeyboard = false;
	std::atomic<bool>		m_bMenuReached = false;
	std::atomic<bool>		m_bCharacterValid = false;
	std::atomic<bool>		m_bCameraDisableDOF = false;


	bool					m_bAshleyOverrideEnabled = false;
	bool					m_bBethOverrideEnabled = false;
	bool					m_bChrisOverrideEnabled = false;
	bool					m_bEmilyOverrideEnabled = false;
	bool					m_bHannahOverrideEnabled = false;
	bool					m_bJackhOverrideEnabled = false;
	bool					m_bJessicahOverrideEnabled = false;
	bool					m_bJoshOverrideEnabled = false;
	bool					m_bMattOverrideEnabled = false;
	bool					m_bMikeOverrideEnabled = false;
	bool					m_bPsychiatristOverrideEnabled = false;
	bool					m_bPsychoOverrideEnabled = false;
	bool					m_bSamOverrideEnabled = false;
	bool					m_bWendigoOverrideEnabled = false;

	int						m_iAshleyCharacterOverride = 0;
	int						m_iBethCharacterOverride = 1;
	int						m_iChrisCharacterOverride = 2;
	int						m_iEmilyCharacterOverride = 3;
	int						m_iHannahCharacterOverride = 4;
	int						m_iJackCharacterOverride = 5;
	int						m_iJessicaCharacterOverride = 6;
	int						m_iJoshCharacterOverride = 7;
	int						m_iMattCharacterOverride = 8;
	int						m_iMikeCharacterOverride = 9;
	int						m_iPsychiatristCharacterOverride = 10;
	int						m_iPsychoCharacterOverride = 11;
	int						m_iSamCharacterOverride = 12;
	int						m_iWendigoCharacterOverride = 13;
	int						m_iAshleyVariantOverride = 0;
	int						m_iBethVariantOverride = 0;
	int						m_iChrisVariantOverride = 0;
	int						m_iEmilyVariantOverride = 0;
	int						m_iHannahVariantOverride = 0;
	int						m_iJackVariantOverride = 0;
	int						m_iJessicaVariantOverride = 0;
	int						m_iJoshVariantOverride = 0;
	int						m_iMattVariantOverride = 0;
	int						m_iMikeVariantOverride = 0;
	int						m_iPsychiatristVariantOverride = 0;
	int						m_iPsychoVariantOverride = 0;
	int						m_iSamVariantOverride = 0;
	int						m_iWendigoVariantOverride = 0;

	//Characters
	CharacterOverride		characterOverrides[NUM_CHARS];
	std::mutex				mtx_characterOverrides;

	std::vector<SwapRule>	swapRules;
	std::mutex				mtx_swapRules;


	//Camera
	struct FreeCamSharedData
	{
		bool					bFreeCamEnabled = false;
		bool					bFreeCamModified = false;
		bool					bFreeCamShouldReset = false;
		bool					bBlockOverlappingKeys = true;
		FVector					FreeCameraPos = {};
		FRotator				FreeCameraRot = {};
		float					fFreeCameraFOV = 0.0f;
		float					fFreeCamSpeed = 200.0f;
		float					fFreeCamRotationSpeed = 50.0f;
	};

	FreeCamSharedData		freeCamSharedData;
	std::mutex				mtx_freecamera;
	
	struct FPCamSharedData
	{
		bool					bFPCamEnabled = false;
		float					fFPCamFOV = 90.0f;
		float					fFPCamNearClipPlane = 20.0f;
	};
	FPCamSharedData			fpCamSharedData;
	
	std::mutex				mtx_firstpersoncamera;


	//Extras
	std::atomic<float>		m_fMorphWeightMultiplier = 100.0f;
	bool					m_bEnableChangeSpeed = false;
	float					m_fSpeed = 100.0f;
	bool					m_bFreezeTime = false;
	std::mutex				mtx_speed;
	std::atomic<uint8_t>	m_iSprintingEnabled = 0;

	int*					m_pCurrentVarToChange = nullptr;

	Menu();

	void	Initialize();
	void	Draw();

	void	DrawCharacterTab();
	void	DrawCameraTab();
	void	DrawExtrasTab();
	void	DrawSettings();

	bool	DrawFreeCam();
	bool	DrawFirstPersonCam();

	void	DrawKeyBind(const char* name, int* var);
	void	KeyBind(int* var, const char* bindName, const char* name);

private:
	struct FPCamLocalData
	{
		bool	bFPCamEnabled;
		float	fFPCamFOV;
		float	fFPCamNearClipPlane;
	};
	FPCamLocalData			fpCamLocalData = { fpCamSharedData.bFPCamEnabled, fpCamSharedData.fFPCamFOV, fpCamSharedData.fFPCamNearClipPlane };

	struct FreeCamLocalData
	{
		bool	bFreeCamEnabled;
		bool	bFreeCamModified;
		bool	bFreeCamShouldReset;
		bool	bBlockOverlappingKeys;
		float	fFreeCamPos[3];
		float	fFreeCamRot[3];
		float	fFreeCamFOV;
		float	fFreeCamSpeed;
		float	fFreeCamRotationSpeed;
	};
	FreeCamLocalData		freeCamLocalData = {};

	void SaveFirstPersonCamData();
	void LoadFirstPersonCamData();

	void SaveFreeCamData();
	void LoadFreeCamData();
};

extern Menu* TheMenu;