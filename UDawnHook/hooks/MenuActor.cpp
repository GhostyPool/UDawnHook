#include "pch.h"
#include "hooks.h"
#include "unreal/UObject.h"
#include "unreal/FName.h"
#include "unreal/FString.h"
#include "unreal/AActor.h"
#include "../plugin/Menu.h"
#include "../plugin/Settings.h"
#include "../plugin/CharacterInfo.h"

using namespace Hooks::MenuActor;

void Hooks::MenuActor::Activate_Hook(int64 a1, int64 CharacterName, int64 VariantName, bool bIgnoreHitchWarning, uint8_t EmotionState, uint8_t Gender, bool bIsPlayer)
{
	if (SettingsMgr->menuCharacter.bEnableCustomMenuCharacter)
	{
		UObject* world = reinterpret_cast<AActor*>(a1)->GetWorld();
		TArray<UObject*>* levels = reinterpret_cast<TArray<UObject*>*>((char*)world + 0x170);

		static FName menuMap(L"LV_FrontEnd", FNAME_Add);

		if (levels->Count >= 2 && levels->Get(1)->Outer->GetFName() == menuMap) //make sure were in menu map
		{
			if (GetVariantIndex(SettingsMgr->menuCharacter.szMenuCharacter, SettingsMgr->menuCharacter.szMenuVariant) != -1)
			{
				FString* charName = reinterpret_cast<FString*>(CharacterName);
				FString* varName = reinterpret_cast<FString*>(VariantName);

				wchar_t newName[256] = {};
				wchar_t newVariant[256] = {};
				swprintf(newName, sizeof(newName), L"%S", SettingsMgr->menuCharacter.szMenuCharacter);
				swprintf(newVariant, sizeof(newVariant), L"%S", SettingsMgr->menuCharacter.szMenuVariant);

				int newNameLength = wcslen(newName) + 1;
				if (newNameLength > charName->Max)
					charName->ResizeTo(newNameLength);

				if (newNameLength <= charName->Max)
				{
					wcscpy(charName->Data, newName);
					charName->Count = newNameLength;
				}
				else
					eLog::Message(__FUNCTION__, "ERROR: Failed to resize character name!");

				int newVariantLength = wcslen(newVariant) + 1;
				if (newVariantLength > varName->Max)
					varName->ResizeTo(newVariantLength);

				if (newVariantLength <= varName->Max)
				{
					wcscpy(varName->Data, newVariant);
					varName->Count = newVariantLength;
				}
				else
					eLog::Message(__FUNCTION__, "ERROR: Failed to resize variant name!");

				if (wcscmp(newName, L"Ashley") == 0
					|| wcscmp(newName, L"Beth") == 0
					|| wcscmp(newName, L"Emily") == 0
					|| wcscmp(newName, L"Hannah") == 0
					|| wcscmp(newName, L"Jessica") == 0
					|| wcscmp(newName, L"Sam") == 0)
				{
					Gender = 0;
				}
				else
					Gender = 1;

				for (int i = 0; i < _countof(szEmotions); ++i)
				{
					if (strcmp(SettingsMgr->menuCharacter.szMenuEmotion, szEmotions[i]) == 0)
					{
						EmotionState = i;
						break;
					}
				}
			}
			else
				eLog::Message(__FUNCTION__, "ERROR: Invalid menu character or variant name provided!");
		}
	}

	if (!TheMenu->m_bMenuReached.load(std::memory_order_relaxed))
	{
		UObject*& gameInstance = *reinterpret_cast<UObject**>(((char*)(reinterpret_cast<AActor*>(a1)->GetWorld()) + 0x1B8)); //game instance

		pGameInstance = reinterpret_cast<uintptr_t>(gameInstance);
		TheMenu->m_bMenuReached.store(true, std::memory_order_relaxed);
	}

	if (oMenuActor_Activate)
		oMenuActor_Activate(a1, CharacterName, VariantName, bIgnoreHitchWarning, EmotionState, Gender, bIsPlayer);
}