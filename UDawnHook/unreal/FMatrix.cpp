#include "pch.h"
#include "FMatrix.h"
#include <numbers>
#include <cmath>

FMatrix::FMatrix(const FRotator& rotator)
{
	constexpr double PI = std::numbers::pi;

	double rollRad = rotator.Roll * (PI / 180.0);
	double pitchRad = rotator.Pitch * (PI / 180.0);
	double yawRad = rotator.Yaw * (PI / 180.0);

	double rollCos = std::cos(rollRad);
	double rollSin = std::sin(rollRad);

	double pitchCos = std::cos(pitchRad);
	double pitchSin = std::sin(pitchRad);

	double yawCos = std::cos(yawRad);
	double yawSin = std::sin(yawRad);

	//Forward
	M[0][0] = pitchCos * yawCos;
	M[0][1] = pitchCos * yawSin;
	M[0][2] = pitchSin;
	M[0][3] = 0.0;

	//Right
	M[1][0] = rollSin * pitchSin * yawCos - rollCos * yawSin;
	M[1][1] = rollSin * pitchSin * yawSin + rollCos * yawCos;
	M[1][2] = -pitchCos * rollSin;
	M[1][3] = 0.0;

	//Up
	M[2][0] = -(rollCos * pitchSin * yawCos + rollSin * yawSin);
	M[2][1] = -(rollCos * pitchSin * yawSin - rollSin * yawCos);
	M[2][2] = pitchCos * rollCos;
	M[2][3] = 0.0;

	M[3][0] = 0.0;
	M[3][1] = 0.0;
	M[3][2] = 0.0;
	M[3][3] = 1.0;
}

FVector FMatrix::GetForward() const
{
	return { M[0][0], M[0][1], M[0][2] };
}

FVector FMatrix::GetRight() const
{
	return { M[1][0], M[1][1], M[1][2] };
}

FVector FMatrix::GetUp() const
{
	return { M[2][0], M[2][1], M[2][2] };
}

FVector FMatrix::GetPos() const
{
	return { M[3][0], M[3][1], M[3][2] };
}

/*void FMatrix::MakeFromX(FMatrix* result, FVector* XAxis)
{
	static uintptr_t pat = _pattern(PATID_FMatrix_MakeFromX);
	if (pat)
		((void(*)(FMatrix*, FVector*))pat)(result, XAxis);
}*/

void FMatrix::Rotator(FRotator* rot)
{
	static uintptr_t pat = _pattern(PATID_FMatrix_Rotator);
	if (pat)
		((void(*)(FMatrix*, FRotator*))pat)(this, rot);
}

/*FRotator FMatrix::FindLookAtRotation(const FVector& From, const FVector& To)
{
	FVector delta = To - From;
	FMatrix matrix = {};
	MakeFromX(&matrix, &delta);
	FRotator rot = {};
	matrix.Rotator(&rot);
	return rot;
}*/