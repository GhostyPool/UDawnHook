#include "pch.h"
#include "hooks.h"
#include "unreal/AWorldSettings.h"
#include "plugin/Menu.h"

using namespace Hooks::Gamespeed;

void Hooks::Gamespeed::HandleChangeSpeed()
{
	bool bEnableChangeSpeed;
	float fSpeed;
	bool bFreezeTime;
	{
		std::lock_guard<std::mutex> lock(TheMenu->mtx_speed);
		bEnableChangeSpeed = TheMenu->m_bEnableChangeSpeed;
		fSpeed = TheMenu->m_fSpeed;
		bFreezeTime = TheMenu->m_bFreezeTime;
	}

	float& fTimeDilation = reinterpret_cast<AWorldSettings*>(Hooks::pWorldSettings)->TimeDilation;

	static bool bTimeChanged = false;

	if (bFreezeTime)
	{
		fTimeDilation = 0.0f;
		bTimeChanged = true;
	}
	else if (bEnableChangeSpeed)
	{
		fTimeDilation = fSpeed / 100.0f;
		bTimeChanged = true;
	}
	else if (!bFreezeTime && !bEnableChangeSpeed && bTimeChanged)
	{
		fTimeDilation = 1.0f;
		bTimeChanged = false;
	}
}