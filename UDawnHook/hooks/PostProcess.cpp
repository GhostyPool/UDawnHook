#include "pch.h"
#include "hooks.h"
#include "plugin/Menu.h"

using namespace Hooks::PostProcess;

struct FPostProcessSettings
{
	char _pad[0x620];
	float DepthOfFieldSensorWidth;
	float DepthOfFieldSqueezeFactor;
	float DepthOfFieldFocalDistance;
	float DepthOfFieldDepthBlurAmount;
	float DepthOfFieldDepthBlurRadius;
	float DepthOfFieldFocalRegion;
	float DepthOfFieldNearTransitionRegion;
	float DepthOfFieldFarTransitionRegion;
	float DepthOfFieldScale;
	float DepthOfFieldNearBlurSize;
	float DepthOfFieldFarBlurSize;
	float DepthOfFieldOcclusion;
	float DepthOfFieldSkyFocusDistance;
	float DepthOfFieldVignetteSize;
	float MotionBlurAmount;
	float MotionBlurMax;
	int MotionBlurTargetFPS;
};

void Hooks::PostProcess::OverridePostProcessSettings_Hook(int64 a1, int64 a2, float a3)
{
	if (TheMenu->m_bCameraDisableDOF.load(std::memory_order_relaxed))
	{
		FPostProcessSettings* settings = reinterpret_cast<FPostProcessSettings*>(a2);
		settings->DepthOfFieldSensorWidth = 0.0f;
	}

	if (oOverridePostProcessSettings)
		oOverridePostProcessSettings(a1, a2, a3);
}