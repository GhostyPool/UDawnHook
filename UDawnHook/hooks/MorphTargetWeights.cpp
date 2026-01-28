#include "pch.h"
#include "hooks.h"
#include "unreal/FName.h"
#include "unreal/FString.h"
#include "unreal/USkeletalMesh.h"
#include "plugin/Menu.h"

using namespace Hooks::Morphs;

void Hooks::Morphs::AppendActiveMorphTargets_Hook(int64 a1, int64 a2, int64 a3, int64 a4)
{
	if (oAppendActiveMorphTargets)
		oAppendActiveMorphTargets(a1, a2, a3, a4);

	TArray<float>& morphWeights = *reinterpret_cast<TArray<float>*>(a4);
	for (int i = 0; i < morphWeights.Count; ++i)
	{
		morphWeights.Get(i) *= TheMenu->m_fMorphWeightMultiplier.load(std::memory_order_relaxed) / 100.0f;
	}
}