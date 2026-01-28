#include "pch.h"
#include "FName.h"

void FName::ToString(FString* str) const
{
	static uintptr_t pat = _pattern(PATID_FName_ToString);
	if (pat)
		((void(*)(const FName*, FString*))pat)(this, str);
}

FName::FName(const char* Name, EFindName FindType)
{
	static uintptr_t pat = _pattern(PATID_FName_ctor_char);
	if (pat)
		((void(*)(FName*, const char*, EFindName))pat)(this, Name, FindType);
}

FName::FName(const wchar_t* Name, EFindName FindType)
{
	static uintptr_t pat = _pattern(PATID_FName_ctor_wchar);
	if (pat)
		((void(*)(FName*, const wchar_t*, EFindName))pat)(this, Name, FindType);
}