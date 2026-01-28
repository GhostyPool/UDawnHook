#pragma once
#include "UObject.h"

class AActor : public UObject
{
public:
	UObject* GetWorld();
};