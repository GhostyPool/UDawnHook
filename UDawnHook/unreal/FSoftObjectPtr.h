#pragma once
#include "FName.h"
#include "FString.h"

struct FSoftObjectPath
{
	FName PackageName;
	FName AssetName;
	FString SubPathString;
};

struct FSoftObjectPtr
{
	char pad[8];
	FSoftObjectPath SoftPath;
};