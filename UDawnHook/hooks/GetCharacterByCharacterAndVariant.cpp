#include "pch.h"
#include "hooks.h"
#include "unreal/UObject.h"
#include "unreal/FName.h"
#include "unreal/FString.h"
#include "unreal/TArray.h"
#include "unreal/FSoftObjectPtr.h"
#include "plugin/CharacterInfo.h"
#include "plugin/Menu.h"
#include "plugin/Settings.h"
#include <mutex>

using namespace Hooks::Characters;

//Soft class gets pre-loaded here
int64 Hooks::Characters::GetCharacterSoftClassByCharacterAndVariant_Hook(int64 a1, int64 a2, int64 a3, int64 a4)
{
	int64 result = 0;
	static bool firstRun = true;

	FName* CharacterName = reinterpret_cast<FName*>(a3);
	FString* VariantName = reinterpret_cast<FString*>(a4);

	FString temp;
	CharacterName->ToString(&temp);

	char name[256] = {};
	char variant[256] = {};
	snprintf(name, sizeof(name), "%ws", temp.GetStr());
	snprintf(variant, sizeof(variant), "%ws", VariantName->GetStr());
	temp.Destroy();

	if (!firstRun)
	{
		{
			//Swap rules first
			bool bRuleMatched = false;
			FName newName;
			FString newVariant;
			{
				std::lock_guard<std::mutex> lock(TheMenu->mtx_swapRules);
				for (const SwapRule& rule : TheMenu->swapRules)
				{
					if (strcmp(rule.originalCharacter.characterName.c_str(), name) == 0 && strcmp(rule.originalCharacter.variantName.c_str(), variant) == 0)
					{
						DEBUG_LOG(__FUNCTION__, "Swap rule matched, overriding with character: %s and variant: %s", rule.swapCharacter.characterName.c_str(), rule.swapCharacter.variantName.c_str());

						newName = FName(rule.swapCharacter.characterName.c_str(), FNAME_Add);
						newVariant = FString(rule.swapCharacter.variantName.c_str());

						bRuleMatched = true;
						break;
					}
				}
			}

			if (bRuleMatched)
			{
				result = oGetCharacterSoftClassByCharacterAndVariant(a1, a2, reinterpret_cast<int64>(&newName), reinterpret_cast<int64>(&newVariant));

				newVariant.Destroy();
				return result;
			}
		}

		//Then character swapper
		int index = GetCharacterIndex(name);
		if (index != -1)
		{
			bool bChrOverrideEnabled = false;
			FName newName;
			FString newVariant;
			{
				std::lock_guard<std::mutex> lock(TheMenu->mtx_characterOverrides);
				const CharacterOverride& chrOverride = TheMenu->characterOverrides[index];
				if (*chrOverride.overrideEnabled)
				{
					const CharacterInfo& swapChr = characterInfo[*chrOverride.characterOverride];

					DEBUG_LOG(__FUNCTION__, "Overriding with character: %s and variant: %s", swapChr.characterName, swapChr.variants[*chrOverride.variantOverride]);

					newName = FName(swapChr.characterName, FNAME_Add);
					newVariant = FString(swapChr.variants[*chrOverride.variantOverride]);
					bChrOverrideEnabled = true;
				}
			}
			
			if (bChrOverrideEnabled)
			{
				result = oGetCharacterSoftClassByCharacterAndVariant(a1, a2, reinterpret_cast<int64>(&newName), reinterpret_cast<int64>(&newVariant));

				newVariant.Destroy();
				return result;
			}

		}
	}
	else //first run aka menu character
	{
		firstRun = false;

		if (SettingsMgr->menuCharacter.bEnableCustomMenuCharacter)
		{
			if (GetVariantIndex(SettingsMgr->menuCharacter.szMenuCharacter, SettingsMgr->menuCharacter.szMenuVariant) != -1)
			{
				FName newMenuName(SettingsMgr->menuCharacter.szMenuCharacter, FNAME_Add);
				FString newMenuVariant(SettingsMgr->menuCharacter.szMenuVariant);
				result = oGetCharacterSoftClassByCharacterAndVariant(a1, a2, reinterpret_cast<int64>(&newMenuName), reinterpret_cast<int64>(&newMenuVariant));

				newMenuVariant.Destroy();
				return result;
			}
		}
	}

	return oGetCharacterSoftClassByCharacterAndVariant(a1, a2, a3, a4);
}

int64 Hooks::Characters::GetCharacterByCharacterVariant_Hook(int64 a1, int64 a2, int64 a3)
{
	int64 result = 0;

	FName* CharacterName = reinterpret_cast<FName*>(a2);
	FString* VariantName = reinterpret_cast<FString*>(a3);

	FString temp;
	CharacterName->ToString(&temp);

	char name[256] = {};
	char variant[256] = {};
	snprintf(name, sizeof(name), "%ws", temp.GetStr());
	snprintf(variant, sizeof(variant), "%ws", VariantName->GetStr());
	temp.Destroy();

	{
		//Swap rules first
		bool bRuleMatched = false;
		FName newName;
		FString newVariant;
		{
			std::lock_guard<std::mutex> lock(TheMenu->mtx_swapRules);
			for (const SwapRule& rule : TheMenu->swapRules)
			{
				if (strcmp(rule.originalCharacter.characterName.c_str(), name) == 0 && strcmp(rule.originalCharacter.variantName.c_str(), variant) == 0)
				{
					DEBUG_LOG(__FUNCTION__, "Swap rule matched, overriding with character: %s and variant: %s", rule.swapCharacter.characterName.c_str(), rule.swapCharacter.variantName.c_str());

					newName = FName(rule.swapCharacter.characterName.c_str(), FNAME_Add);
					newVariant = FString(rule.swapCharacter.variantName.c_str());

					bRuleMatched = true;
					break;
				}
			}
		}

		if (bRuleMatched)
		{
			result = oGetCharacterByCharacterVariant(a1, reinterpret_cast<int64>(&newName), reinterpret_cast<int64>(&newVariant));

			newVariant.Destroy();
			return result;
		}
	}


	//Then character swapper
	int index = GetCharacterIndex(name);
	if (index != -1)
	{
		bool bChrOverrideEnabled = false;
		FName newName;
		FString newVariant;
		{
			std::lock_guard<std::mutex> lock(TheMenu->mtx_characterOverrides);

			const CharacterOverride& chrOverride = TheMenu->characterOverrides[index];
			if (*chrOverride.overrideEnabled)
			{
				const CharacterInfo& swapChr = characterInfo[*chrOverride.characterOverride];

				DEBUG_LOG(__FUNCTION__, "Overriding with character: %s and variant: %s", swapChr.characterName, swapChr.variants[*chrOverride.variantOverride]);

				newName = FName(swapChr.characterName, FNAME_Add);
				newVariant = FString(swapChr.variants[*chrOverride.variantOverride]);
				bChrOverrideEnabled = true;
			}
		}

		if (bChrOverrideEnabled)
		{
			result = oGetCharacterByCharacterVariant(a1, reinterpret_cast<int64>(&newName), reinterpret_cast<int64>(&newVariant));

			newVariant.Destroy();
			return result;
		}
	}

	return oGetCharacterByCharacterVariant(a1, a2, a3);
}