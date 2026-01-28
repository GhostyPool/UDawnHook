#pragma once
#include "FName.h"
#include <string>

class UClass;

class UObject {
public:
	void*		vtable;

	int			Flags;
	int			Index;
	UClass*		Class;
	FName		Name;
	UObject*	Outer;

	const FName& GetFName() const { return Name; }
};


class UClass : public UObject {
public:
	char pad[24];
	UClass* SuperStruct;
};