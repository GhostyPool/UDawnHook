#pragma once
#include "FRotator.h"
#include "FVector.h"

struct FMatrix
{
	double M[4][4] = {};

	FMatrix() = default;
	FMatrix(const FRotator& rotator);

	FVector GetForward() const;
	FVector GetRight() const;
	FVector GetUp() const;
	FVector GetPos() const;

	//static void MakeFromX(FMatrix* result, FVector* XAxis);
	void Rotator(FRotator* rot);
	//static FRotator FindLookAtRotation(const FVector& From, const FVector& To);
};