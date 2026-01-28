#pragma once
#include "UObject.h"
#include "../utils/core.h"

class AWorldSettings : public UObject
{
public:
	char pat[0x3A8];
	float TimeDilation;
	float CinematicTimeDilation;
	float DemoPlayTimeDilation;
	float MinGlobalTimeDilation;
	float MaxGlobalTimeDilation;
	float MinUndilatedFrameTime;
	float MaxUndilatedFrameTime;
};
VALIDATE_OFFSET(AWorldSettings, TimeDilation, 0x3D0);