#include "pch.h"
#include "USkeletalMesh.h"

void USkeletalMeshComponent::GetBoneMatrix(FMatrix* result, int BoneIdx) const
{
	static uintptr_t pat = _pattern(PATID_USkeletalMeshComponent_GetBoneMatrix);
	if (pat)
		((void(*)(const USkeletalMeshComponent*, FMatrix*, int))pat)(this, result, BoneIdx);
}

FVector USkeletalMeshComponent::GetBoneLocation(const wchar_t* BoneName) const
{
	FVector loc = {};
	FMatrix matrix = {};
	int boneIdx = GetBoneIndexFromName(BoneName);
	if (boneIdx != -1)
		GetBoneMatrix(&matrix, boneIdx);

	loc = matrix.GetPos();
	return loc;
}

FRotator USkeletalMeshComponent::GetBoneRotation(const wchar_t* BoneName) const
{
	FRotator rot = {};
	FMatrix matrix = {};
	int boneIdx = GetBoneIndexFromName(BoneName);
	if (boneIdx != -1)
		GetBoneMatrix(&matrix, boneIdx);

	matrix.Rotator(&rot);
	return rot;
}

int USkeletalMeshComponent::GetBoneIndexFromName(const wchar_t* BoneName) const
{
	int index = -1;

	for (int i = 0; i < SkeletalMesh->GetNumBones(); ++i)
	{
		FString temp;
		GetBoneName(i).ToString(&temp);
		if (wcscmp(temp.GetStr(), BoneName) == 0)
		{
			index = i;
			temp.Destroy();
			break;
		}
		temp.Destroy();
	}

	return index;
}

FName USkeletalMeshComponent::GetBoneName(int BoneIdx) const { return SkeletalMesh->RefSkeleton.FinalRefBoneInfo.Get(BoneIdx).Name; }

int USkeletalMesh::GetNumBones() const { return RefSkeleton.FinalRefBoneInfo.Count >= 0 ? RefSkeleton.FinalRefBoneInfo.Count : 0; }