#pragma once
#include "TArray.h"

class FString : public TArray< wchar_t > {
public:
	wchar_t* GetStr() const { return (wchar_t*)(&Data[0]); }

	FString() = default;
	FString(const char* str);
	FString(const wchar_t* str);

	void Destroy();
	void ResizeTo(int newMax);
};