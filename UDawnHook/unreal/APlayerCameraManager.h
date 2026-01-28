#pragma once
#include "UObject.h"
#include "FVector.h"
#include "FRotator.h"
#include "FMatrix.h"
#include "../utils/core.h"

struct FMinimalViewInfo
{
	FVector Location;
	FRotator Rotation;
	float FOV;

	float DesiredFOV;
	float OrthoWidth;
	float OrthoNearClipPlane;
	float OrthoFarClipPlane;
	float PerspectiveNearClipPlane;
	float AspectRatio;
};

struct FCameraCacheEntry
{
	char pad[0x10];
	FMinimalViewInfo POV;
};

struct FTViewTarget
{
	UObject* Target;
	char pad[0x7D8];
};

class APlayerCameraManager
{
public:
	char pad[0x330];
	FTViewTarget ViewTarget;
	char _pad[0x810];
	FCameraCacheEntry CameraCache;

	FMatrix GetMatrix() const
	{
		return FMatrix(CameraCache.POV.Rotation);
	}
};
VALIDATE_OFFSET(APlayerCameraManager, CameraCache, 0x1320);

inline APlayerCameraManager* TheCamera = nullptr;