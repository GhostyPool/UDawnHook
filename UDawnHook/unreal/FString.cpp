#include "pch.h"
#include "FString.h"

FString::FString(const char* str)
{
	static uintptr_t pat = _pattern(PATID_FString_ctor_char);
	if (pat)
		((void(*)(FString*, const char*))pat)(this, str);
}

FString::FString(const wchar_t* str)
{
	static uintptr_t pat = _pattern(PATID_FString_ctor_wchar);
	if (pat)
		((void(*)(FString*, const wchar_t*))pat)(this, str);
}

void FString::Destroy()
{
	static uintptr_t pat = _pattern(PATID_FString_dtor);
	if (pat)
		((void(*)(FString*))pat)(this);
}

void FString::ResizeTo(int newMax)
{
	static uintptr_t pat = _pattern(PATID_FString_ResizeTo);
	if (pat)
		((void(*)(FString*, int))pat)(this, newMax);
}