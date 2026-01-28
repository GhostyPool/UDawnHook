#include "pch.h"
#include "UWorld.h"

AWorldSettings* UWorld::GetWorldSettings(bool bCheckStreamingPersistent, bool bChecked) const
{
	static uintptr_t pat = _pattern(PATID_UWorld_GetWorldSettings);
	if (pat)
		return ((AWorldSettings* (*)(const UWorld*, bool, bool))pat)(this, bCheckStreamingPersistent, bChecked);

	return nullptr;
}