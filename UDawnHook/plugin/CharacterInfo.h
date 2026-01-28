#pragma once

struct CharacterInfo
{
	const char* const			characterName;
	const char* const* const	variants;
	const int					variantCount;
};

#define NUM_CHARS 14

inline const char* const szCharacters[] =
{
	"Ashley",
	"Beth",
	"Chris",
	"Emily",
	"Hannah",
	"Jack",
	"Jessica",
	"Josh",
	"Matt",
	"Mike",
	"Psychiatrist",
	"Psycho",
	"Sam",
	"Wendigo"
};
inline const char* const szAshleyVariants[] =
{
	"Ashley_intro",
	"Ashley_outdoor",
	"Ashley_bloody",
	"Ashley_hanging",
	"Ashley_bloodybruised",
	"Ashley_luggage",
	"Ashley_bloodybruiseddead",
	"Ashley_bloodydead",
	"Ashley_burnt",
	"Ashley_indoor"
};
inline const char* const szBethVariants[] =
{
	"Beth_indoor",
	"Beth_outdoor",
	"Beth_dead",
	"Beth_ghost2",
	"Beth_halfeaten",
	"Beth_ghost1"
};
inline const char* const szChrisVariants[] =
{
	"Chris_intro",
	"Chris_luggage",
	"Chris_outdoor",
	"Chris_indoor",
	"Chris_monk",
	"Chris_outdoordirty",
	"Chris_beaten"
};
inline const char* const szEmilyVariants[] =
{
	"Emily_intro",
	"Emily_outdoor",
	"Emily_beaten",
	"Emily_beatennobite",
	"Emily_beatennobitewet",
	"Emily_beatenshot",
	"Emily_beatennobiteshot",
	"Emily_hanging",
	"Emily_frontend",
	"Emily_frontendbeaten",
	"Emily_burnt"
};
inline const char* const szHannahVariants[] =
{
	"Hannah_halfdressed",
	"Hannah_indoor",
	"Hannah_outdoor",
	"Hannah_indoor_hoodie",
	"Hannah_dead",
	"Hannah_ghost1",
	"Hannah_ghost5",
	"Hannah_ghost2",
	"Hannah_ghost3",
	"Hannah_ghost4"
};
inline const char* const szJackVariants[] =
{
	"Jack_flamethrower",
	"Jack_outdoor",
	"Jack_dead",
	"Jack_flamethrowerglasses"
};
inline const char* const szJessicaVariants[] =
{
	"Jessica_intro",
	"Jessica_outdoor",
	"Jessica_beaten",
	"Jessica_beatendead",
	"Jessica_indoor",
	"Jessica_indoorbeaten",
	"Jessica_indoorbeatendead",
	"Jessica_outdoorbeaten",
	"Jessica_outdoorbeatendead",
	"Jessica_outdoorwet",
	"Jessica_underwear",
	"Jessica_luggage",
	"Jessica_minersclothes",
	"Jessica_blanket",
	"Jessica_indoorminersclothes"
};
inline const char* const szJoshVariants[] =
{
	"Josh_indoor",
	"Josh_intro",
	"Josh_psycho",
	"Josh_hanging",
	"Josh_outdoor",
	"Josh_session1",
	"Josh_session5",
	"Josh_session7",
	"Josh_semiwendigo",
	"Josh_psychostabbedwet",
	"Josh_psychowet"
};
inline const char* const szMattVariants[] =
{
	"Matt_intro",
	"Matt_outdoor",
	"Matt_luggage",
	"Matt_dirty",
	"Matt_dead",
	"Matt_beaten",
	"Matt_beatensmashed",
	"Matt_dirtydead"
};
inline const char* const szMikeVariants[] =
{
	"Mike_intro",
	"Mike_outdoor",
	"Mike_outdoordirty",
	"Mike_outdoorwet",
	"Mike_vest",
	"Mike_vestdirty",
	"Mike_vestdirtywet",
	"Mike_vestlostfingers",
	"Mike_vestwet",
	"Mike_outdoorbeaten",
	"Mike_indoor",
	"Mike_outdoordirtywet",
	"Mike_postwendigoeviscerated",
	"Mike_postwendigo",
	"Mike_postwendigowet",
	"Mike_postwendigoevisceratedburnt"
};
inline const char* const szPsychiatristVariants[] =
{
	"Psychiatrist_frontend",
	"Psychiatrist_indoor",
	"Psychiatrist_stage3",
	"Psychiatrist_stage2",
	"Psychiatrist_stage4"
};
inline const char* const szPsychoVariants[] =
{
	"Psycho_masked",
	"Psycho_nogloves",
	"Cop_swat1"
};
inline const char* const szSamVariants[] =
{
	"Sam_intro",
	"Sam_luggage",
	"Sam_outdoor",
	"Sam_indoor",
	"Sam_towel",
	"Sam_climbing",
	"Sam_climbingcleannotorch",
	"Sam_towelluggage",
	"Sam_towelluggageseated",
	"Sam_dummy",
	"Sam_scarecrow",
	"Sam_zombie",
	"Sam_climbingbeaten",
	"Sam_climbingdirty",
	"Sam_climbingbeatennotorch",
	"Sam_climbingbeatenwet",
	"Sam_frontend",
	"Sam_climbingeviscerated"
};
inline const char* const szWendigoVariants[] =
{
	"Wendigo_hannah",
	"Wendigo_hannahsilhouette",
	"Wendigo_miner1",
	"Wendigo_spirit",
	"Wendigo_miner3",
	"Wendigo_miner2",
	"Wendigo_hannahwet",
	"Wendigo_miner1burning",
	"Wendigo_straightjacket",
	"Wendigo_miner2burning",
	"Wendigo_indian",
	"Wendigo_indianburning"
};

inline const char* const szEmotions[] =
{
	"Relaxed",
	"Cautious",
	"Scared",
	"Terrified",
	"Fatigued"
};

inline const CharacterInfo characterInfo[] =
{
	{ "Ashley", szAshleyVariants, _countof(szAshleyVariants) },
	{ "Beth", szBethVariants, _countof(szBethVariants) },
	{ "Chris", szChrisVariants, _countof(szChrisVariants) },
	{ "Emily", szEmilyVariants, _countof(szEmilyVariants) },
	{ "Hannah", szHannahVariants, _countof(szHannahVariants) },
	{ "Jack", szJackVariants, _countof(szJackVariants) },
	{ "Jessica", szJessicaVariants, _countof(szJessicaVariants) },
	{ "Josh", szJoshVariants, _countof(szJoshVariants) },
	{ "Matt", szMattVariants, _countof(szMattVariants) },
	{ "Mike", szMikeVariants, _countof(szMikeVariants) },
	{ "Psychiatrist", szPsychiatristVariants, _countof(szPsychiatristVariants) },
	{ "Psycho", szPsychoVariants, _countof(szPsychoVariants) },
	{ "Sam", szSamVariants, _countof(szSamVariants) },
	{ "Wendigo", szWendigoVariants, _countof(szWendigoVariants) }
};

inline int GetCharacterIndex(const char* characterName)
{
	int index = -1;

	for (int i = 0; i < NUM_CHARS; ++i)
	{
		if (strcmp(szCharacters[i], characterName) == 0)
		{
			index = i;
			break;
		}
	}

	return index;
}

inline int GetVariantIndex(const char* characterName, const char* variantName)
{
	int index = -1;

	int characterIndex = GetCharacterIndex(characterName);
	if (characterIndex != -1)
	{
		const CharacterInfo& chr = characterInfo[characterIndex];

		for (int i = 0; i < chr.variantCount; ++i)
		{
			if (strcmp(chr.variants[i], variantName) == 0)
			{
				index = i;
				break;
			}
		}
	}

	return index;
}

inline int GetEmotionIndex(const char* emotion)
{
	int index = -1;

	for (int i = 0; i < _countof(szEmotions); ++i)
	{
		if (strcmp(szEmotions[i], emotion) == 0)
		{
			index = i;
			break;
		}
	}

	return index;
}