#pragma once
#include "USkeletalMesh.h"
#include "../utils/core.h"

struct ABatesCharacter
{
	char pad[0x328];
	USkeletalMeshComponent* body;
	char _pad[0x368];
	USkeletalMeshComponent* head;
	USkeletalMeshComponent* hair;
};
VALIDATE_OFFSET(ABatesCharacter, head, 0x698);
VALIDATE_OFFSET(ABatesCharacter, hair, 0x6A0);