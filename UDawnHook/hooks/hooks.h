#pragma once
#include <mutex>
#include <queue>
#include <functional>

namespace Hooks
{
	inline float deltaTime = 0.0f;

	inline uintptr_t pGameInstance = 0;
	inline uintptr_t pCharacter = 0;
	inline uintptr_t pWorldSettings = 0;

	namespace FEngineLoop
	{
		void Tick_Hook(int64);
		inline void(*oFEngineLoop_Tick)(int64) = nullptr;
	}

	namespace MenuActor
	{
		void Activate_Hook(int64, int64, int64, bool, uint8_t, uint8_t, bool);
		inline void(*oMenuActor_Activate)(int64, int64, int64, bool, uint8_t, uint8_t, bool) = nullptr;
	}

	namespace Characters
	{
		int64 GetCharacterByCharacterVariant_Hook(int64, int64, int64);
		inline int64(*oGetCharacterByCharacterVariant)(int64, int64, int64) = nullptr;
		
		int64 GetCharacterSoftClassByCharacterAndVariant_Hook(int64, int64, int64, int64);
		inline int64(*oGetCharacterSoftClassByCharacterAndVariant)(int64, int64, int64, int64) = nullptr;
	}

	namespace Morphs
	{
		void AppendActiveMorphTargets_Hook(int64, int64, int64, int64);
		inline void(*oAppendActiveMorphTargets)(int64, int64, int64, int64) = nullptr;
	}

	namespace DebugMenu
	{
		void UWidget_SetVisibility_Hook(int64, uint8_t);
		inline void(*oUWidget_SetVisibility)(int64, uint8_t) = nullptr;
	}

	namespace Camera
	{
		void DoUpdateCamera_Hook(int64, float);
		inline void(*oDoUpdateCamera)(int64, float) = nullptr;
	}

	namespace PostProcess
	{
		void OverridePostProcessSettings_Hook(int64, int64, float);
		inline void(*oOverridePostProcessSettings)(int64, int64, float) = nullptr;
	}

	namespace Locomotion
	{
		void HandleSprinting();
		
		void StateDriven_SetCharacterMode_Hook(int64, uint8_t, bool, bool);
		inline void(*oStateDriven_SetCharacterMode)(int64, uint8_t, bool, bool) = nullptr;

		void StateDriven_PushCharacterMode_Hook(int64, int64, uint8_t, bool);
		inline void(*oStateDriven_PushCharacterMode)(int64, int64, uint8_t, bool) = nullptr;
	}

	namespace Gamespeed
	{
		void HandleChangeSpeed();
	}
}