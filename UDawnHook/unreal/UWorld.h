#pragma once
#include "UObject.h"
#include "AWorldSettings.h"

class UWorld : public UObject
{
public:
	AWorldSettings* GetWorldSettings(bool bCheckStreamingPersistent = false, bool bChecked = true) const;
};