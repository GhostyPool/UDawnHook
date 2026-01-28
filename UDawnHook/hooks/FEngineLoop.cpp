#include "pch.h"
#include "hooks.h"
#include "unreal/UObject.h"
#include "unreal/AActor.h"
#include "unreal/UWorld.h"
#include "unreal/TArray.h"
#include "plugin/Menu.h"

using namespace Hooks;
using namespace Hooks::FEngineLoop;

static void GetDeltaTime()
{
	using steady_clock = std::chrono::steady_clock;
	static steady_clock::time_point last = steady_clock::now();
	steady_clock::time_point now = steady_clock::now();

	deltaTime = std::chrono::duration<float>(now - last).count();
	last = now;
}

static void FindThings()
{
	TArray<UObject*>* localPlayers = reinterpret_cast<TArray<UObject*>*>(pGameInstance + 0x38);
	UObject*& localPlayer = localPlayers->Get(0);
	UObject*& playerController = *reinterpret_cast<UObject**>((char*)localPlayer + 0x30);

	// world settings
	UWorld* world = (UWorld*)reinterpret_cast<AActor*>(playerController)->GetWorld();
	if (world)
		pWorldSettings = reinterpret_cast<uintptr_t>(world->GetWorldSettings());
	else
		pWorldSettings = 0;

	UObject*& pawn = *reinterpret_cast<UObject**>((char*)playerController + 0x2E0);

	if (pawn)
	{
		pCharacter = reinterpret_cast<uintptr_t>(pawn);
		TheMenu->m_bCharacterValid.store(true, std::memory_order_relaxed);
	}
	else
	{
		pCharacter = 0;
		TheMenu->m_bCharacterValid.store(false, std::memory_order_relaxed);
	}
}

void Hooks::FEngineLoop::Tick_Hook(int64 a1)
{
	if (oFEngineLoop_Tick)
		oFEngineLoop_Tick(a1);

	GetDeltaTime();

	if (pGameInstance)
		FindThings();
	
	if (pCharacter)
		Hooks::Locomotion::HandleSprinting();

	if (pWorldSettings)
		Hooks::Gamespeed::HandleChangeSpeed();
}