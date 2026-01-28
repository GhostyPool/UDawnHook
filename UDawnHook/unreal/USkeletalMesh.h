#pragma once
#include "UObject.h"
#include "FMatrix.h"
#include "FVector.h"
#include "FRotator.h"
#include "../utils/core.h"

struct FReferenceSkeleton
{
	char pad[0x20];
	struct FMeshBoneInfo
	{
		FName Name;
		int ParentIndex;
	};

	TArray<FMeshBoneInfo> FinalRefBoneInfo;
};

class USkeletalMesh : public UObject
{
public:
	char pad[0x2A8];
	FReferenceSkeleton RefSkeleton;

	int GetNumBones() const;
};
VALIDATE_OFFSET(USkeletalMesh, RefSkeleton, 0x2D0);

class USkeletalMeshComponent : public UObject
{
public:
	char pad[0x588];
	USkeletalMesh* SkeletalMesh;
	char _pad[0x188];
	TArray<float> MorphTargetWeights;

	void GetBoneMatrix(FMatrix* result, int BoneIdx) const;
	FName GetBoneName(int BoneIdx) const;
	int GetBoneIndexFromName(const wchar_t* BoneName) const;
	FVector GetBoneLocation(const wchar_t* BoneName) const;
	FRotator GetBoneRotation(const wchar_t* BoneName) const;
};
VALIDATE_OFFSET(USkeletalMeshComponent, SkeletalMesh, 0x5B0);
VALIDATE_OFFSET(USkeletalMeshComponent, MorphTargetWeights, 0x740);