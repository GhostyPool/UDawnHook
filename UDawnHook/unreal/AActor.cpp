#include "pch.h"
#include "AActor.h"
#include "UObject.h"

UObject* AActor::GetWorld()
{
	static uintptr_t pat = _pattern(PATID_AActor_GetWorld);

	if (pat)
		return ((UObject*(*)(AActor*))pat)(this);

	return nullptr;
}