#include "pch.h"
#include "Menu.h"
#include "Settings.h"
#include "CharacterInfo.h"

#include "../helper/eKeyboardMan.h"

#include "../gui/notifications.h"
#include "../gui/imgui/imgui.h"
#include "../gui/gui_impl.h"

Menu* TheMenu = new Menu();


Menu::Menu()
	: characterOverrides{
		{ "Ashley", &m_bAshleyOverrideEnabled, &m_iAshleyCharacterOverride, &m_iAshleyVariantOverride },
		{ "Beth", &m_bBethOverrideEnabled, &m_iBethCharacterOverride, &m_iBethVariantOverride },
		{ "Chris", &m_bChrisOverrideEnabled, &m_iChrisCharacterOverride, &m_iChrisVariantOverride },
		{ "Emily", &m_bEmilyOverrideEnabled, &m_iEmilyCharacterOverride, &m_iEmilyVariantOverride },
		{ "Hannah", &m_bHannahOverrideEnabled, &m_iHannahCharacterOverride, &m_iHannahVariantOverride },
		{ "Jack", &m_bJackhOverrideEnabled, &m_iJackCharacterOverride, &m_iJackVariantOverride },
		{ "Jessica", &m_bJessicahOverrideEnabled, &m_iJessicaCharacterOverride, &m_iJessicaVariantOverride },
		{ "Josh", &m_bJoshOverrideEnabled, &m_iJoshCharacterOverride, &m_iJoshVariantOverride },
		{ "Matt", &m_bMattOverrideEnabled, &m_iMattCharacterOverride, &m_iMattVariantOverride },
		{ "Mike", &m_bMikeOverrideEnabled, &m_iMikeCharacterOverride, &m_iMikeVariantOverride },
		{ "Psychiatrist", &m_bPsychiatristOverrideEnabled, &m_iPsychiatristCharacterOverride, &m_iPsychiatristVariantOverride },
		{ "Psycho", &m_bPsychoOverrideEnabled, &m_iPsychoCharacterOverride, &m_iPsychoVariantOverride },
		{ "Sam", &m_bSamOverrideEnabled, &m_iSamCharacterOverride, &m_iSamVariantOverride },
		{ "Wendigo", &m_bWendigoOverrideEnabled, &m_iWendigoCharacterOverride, &m_iWendigoVariantOverride }
	}
{
}

void Menu::Initialize()
{
}

static void ShowHelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

static void ShowHelpMarkerVA(const char* desc, ...)
{
	char text[2048];

	va_list args;
	va_start(args, desc);
	vsnprintf(text, sizeof(text), desc, args);
	va_end(args);

	ShowHelpMarker(text);
}

void Menu::Draw()
{
	bool m_bIsActive = m_iIsActive.load(std::memory_order_relaxed);
	if (!m_bIsActive)
	{
		m_bWantsKeyboard.store(false, std::memory_order_relaxed);
		return;
	}

	ImGuiIO& io = ImGui::GetIO();
	
	m_bWantsKeyboard.store(io.WantCaptureKeyboard, std::memory_order_relaxed);
	io.MouseDrawCursor = true;

	if (ImGui::Begin("UDawnHook by ghostyface1337", &m_bIsActive, ImGuiWindowFlags_MenuBar))
		m_iIsActive.store(m_bIsActive ? 1 : 0, std::memory_order_relaxed);

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Settings"))
		{
			m_bSubmenuActive[SUBMENU_SETTINGS] = true;
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::BeginMenu("About"))
			{
				ImGui::MenuItem("Version: " UNTILDAWN_HOOK_VERSION);
				ImGui::MenuItem("Date: " __DATE__);
				ImGui::EndMenu();
			}

			ImGui::EndMenu();
		}
	}
	ImGui::EndMenuBar();

	if (ImGui::BeginTabBar("##tabs"))
	{
		if (ImGui::BeginTabItem("Characters"))
		{
			DrawCharacterTab();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Camera"))
		{
			DrawCameraTab();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Extras"))
		{
			DrawExtrasTab();
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
	ImGui::End();

	if (m_bSubmenuActive[SUBMENU_SETTINGS])
		DrawSettings();
}

void Menu::DrawCharacterTab()
{
	if (ImGui::BeginTabBar("##characters"))
	{
		constexpr ImVec4 warningCol = ImVec4(1.0f, 0.3f, 0.0f, 1.0f);
		if (ImGui::BeginTabItem("Character swapper"))
		{
			ImGui::TextWrapped("Swap a character with the specified character and variant.");
			ImGui::PushStyleColor(ImGuiCol_Text, warningCol);
			ImGui::PushStyleColor(ImGuiCol_TextDisabled, warningCol);
			ImGui::TextWrapped("Do not use \"continue\" when swapping characters! It can cause crashes or soft-locks.");
			ImGui::SameLine(); ShowHelpMarker("Use the episodes or debug menu to restart a chapter/level.");
			ImGui::PopStyleColor(2);
			ImGui::Separator();

			for (int i = 0; i < NUM_CHARS; ++i)
			{
				bool bChrOverride;
				std::string characterName;
				int iCharacterOverride;
				int iVariantOverride;
				{
					std::lock_guard<std::mutex> lock(TheMenu->mtx_characterOverrides);
					const CharacterOverride& chrOverride = characterOverrides[i];

					bChrOverride = *chrOverride.overrideEnabled;
					characterName = chrOverride.characterName;
					iCharacterOverride = *chrOverride.characterOverride;
					iVariantOverride = *chrOverride.variantOverride;
				}

				ImGui::PushID(i);
					
				bool bChanged = false;
				bChanged |= ImGui::Checkbox(characterName.c_str(), &bChrOverride);
				ImGui::Indent();
				{
					ImGui::AlignTextToFramePadding();
					ImGui::Text("Swap to:");
					ImGui::SameLine();
					ImGui::SetNextItemWidth(150.0f);
					if (ImGui::Combo("##swapcharacter", &iCharacterOverride, szCharacters, NUM_CHARS))
					{
						iVariantOverride = 0;
						bChanged = true;
					}

					const CharacterInfo& swapChr = characterInfo[iCharacterOverride]; //this is the character the user chose to swap to
					ImGui::SameLine();
					ImGui::SetNextItemWidth(250.0f);
					bChanged |= ImGui::Combo("##swapvariant", &iVariantOverride, swapChr.variants, swapChr.variantCount);
				}
				ImGui::Unindent();
				ImGui::Spacing();
				ImGui::PopID();

				if (bChanged)
				{
					std::lock_guard<std::mutex> lock(TheMenu->mtx_characterOverrides);
					const CharacterOverride& chrOverride = characterOverrides[i];

					*chrOverride.overrideEnabled = bChrOverride;
					*chrOverride.characterOverride = iCharacterOverride;
					*chrOverride.variantOverride = iVariantOverride;
				}
			}
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Swap rule maker"))
		{
			ImGui::TextWrapped("Create a custom rule which swaps a specific character's variant for another.");
			ImGui::PushStyleColor(ImGuiCol_Text, warningCol);
			ImGui::PushStyleColor(ImGuiCol_TextDisabled, warningCol);
			ImGui::TextWrapped("Do not use \"continue\" when swapping characters! It can cause crashes or soft-locks.");
			ImGui::SameLine(); ShowHelpMarker("Use the episodes or debug menu to restart a chapter/level.");
			ImGui::PopStyleColor(2);
			ImGui::Separator();

			static int iOriginalCharacter = 0;
			static int iOriginalVariant = 0;
			ImGui::SetNextItemWidth(150.0f);
			if (ImGui::Combo("##ruleogcharacter", &iOriginalCharacter, szCharacters, NUM_CHARS))
				iOriginalVariant = 0;

			ImGui::SameLine();
			const CharacterInfo& ogChr = characterInfo[iOriginalCharacter];
			ImGui::SetNextItemWidth(250.0f);
			ImGui::Combo("##ruleogvariant", &iOriginalVariant, ogChr.variants, ogChr.variantCount);

			ImGui::SameLine();
			ImGui::Text("->");
			ImGui::SameLine();

			static int iSwapCharacter = 0;
			static int iSwapVariant = 0;
			ImGui::SetNextItemWidth(150.0f);
			if (ImGui::Combo("##ruleswapcharacter", &iSwapCharacter, szCharacters, NUM_CHARS))
				iSwapVariant = 0;

			ImGui::SameLine();
			const CharacterInfo& swapChr = characterInfo[iSwapCharacter];
			ImGui::SetNextItemWidth(250.0f);
			ImGui::Combo("##ruleswapvariant", &iSwapVariant, swapChr.variants, swapChr.variantCount);

			ImGui::SameLine();
			if (ImGui::Button("+##addrule", ImVec2(50.0f, 0.0f)))
			{
				SwapRule::CharacterAndVariant og{ szCharacters[iOriginalCharacter], ogChr.variants[iOriginalVariant] };
				SwapRule::CharacterAndVariant swap{ szCharacters[iSwapCharacter], swapChr.variants[iSwapVariant] };

				static unsigned int id = 1;
				std::string name = std::string("Swap rule: ") + std::string(ogChr.variants[iOriginalVariant]);
				{
					std::lock_guard<std::mutex> lock(mtx_swapRules);
					swapRules.emplace_back(name, id, og, swap);
				}
				++id;
			}
			ImGui::Separator();
				
			for (int i = 0; i < swapRules.size(); )
			{
				std::string ruleName;
				int ruleId;
				SwapRule::CharacterAndVariant originalCharacter;
				SwapRule::CharacterAndVariant swapCharacter;
				{
					std::lock_guard<std::mutex> lock(mtx_swapRules);
					const SwapRule& rule = swapRules[i];

					ruleName = rule.name;
					ruleId = rule.id;
					originalCharacter = rule.originalCharacter;
					swapCharacter = rule.swapCharacter;
				}

				ImGui::PushID(ruleId);

				bool ruleDeleted = false;
				if (ImGui::CollapsingHeader(ruleName.c_str()))
				{
						
					ImGui::Text("Character: %s -> %s", originalCharacter.characterName.c_str(), swapCharacter.characterName.c_str());
					ImGui::Text("Variant: %s -> %s", originalCharacter.variantName.c_str(), swapCharacter.variantName.c_str());

					if (ImGui::Button("Delete##deleterule"))
						ruleDeleted = true;
				}

				if (ruleDeleted)
				{
					std::lock_guard<std::mutex> lock(mtx_swapRules);
					swapRules.erase(swapRules.begin() + i);
				}
				else
					++i;

				ImGui::PopID();
			}
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
}

void Menu::SaveFreeCamData()
{
	std::lock_guard<std::mutex> lock(mtx_freecamera);

	freeCamSharedData.bFreeCamEnabled = freeCamLocalData.bFreeCamEnabled;
	freeCamSharedData.bFreeCamModified = freeCamLocalData.bFreeCamModified;
	freeCamSharedData.bFreeCamShouldReset = freeCamLocalData.bFreeCamShouldReset;
	freeCamSharedData.bBlockOverlappingKeys = freeCamLocalData.bBlockOverlappingKeys;
	freeCamSharedData.FreeCameraPos.X = freeCamLocalData.fFreeCamPos[0]; freeCamSharedData.FreeCameraPos.Y = freeCamLocalData.fFreeCamPos[1]; freeCamSharedData.FreeCameraPos.Z = freeCamLocalData.fFreeCamPos[2];
	freeCamSharedData.FreeCameraRot.Pitch = freeCamLocalData.fFreeCamRot[0]; freeCamSharedData.FreeCameraRot.Yaw = freeCamLocalData.fFreeCamRot[1]; freeCamSharedData.FreeCameraRot.Roll = freeCamLocalData.fFreeCamRot[2];
	freeCamSharedData.fFreeCameraFOV = freeCamLocalData.fFreeCamFOV;
	freeCamSharedData.fFreeCamSpeed = freeCamLocalData.fFreeCamSpeed;
	freeCamSharedData.fFreeCamRotationSpeed = freeCamLocalData.fFreeCamRotationSpeed;
}
void Menu::LoadFreeCamData()
{
	std::lock_guard<std::mutex> lock(mtx_freecamera);

	freeCamLocalData.bFreeCamEnabled = freeCamSharedData.bFreeCamEnabled;
	freeCamLocalData.bFreeCamModified = freeCamSharedData.bFreeCamModified;
	freeCamLocalData.bFreeCamShouldReset = freeCamSharedData.bFreeCamShouldReset;
	freeCamLocalData.bBlockOverlappingKeys = freeCamSharedData.bBlockOverlappingKeys;
	freeCamLocalData.fFreeCamPos[0] = freeCamSharedData.FreeCameraPos.X; freeCamLocalData.fFreeCamPos[1] = freeCamSharedData.FreeCameraPos.Y; freeCamLocalData.fFreeCamPos[2] = freeCamSharedData.FreeCameraPos.Z;
	freeCamLocalData.fFreeCamRot[0] = freeCamSharedData.FreeCameraRot.Pitch; freeCamLocalData.fFreeCamRot[1] = freeCamSharedData.FreeCameraRot.Yaw; freeCamLocalData.fFreeCamRot[2] = freeCamSharedData.FreeCameraRot.Roll;
	freeCamLocalData.fFreeCamFOV = freeCamSharedData.fFreeCameraFOV;
	freeCamLocalData.fFreeCamSpeed = freeCamSharedData.fFreeCamSpeed;
	freeCamLocalData.fFreeCamRotationSpeed = freeCamSharedData.fFreeCamRotationSpeed;
}
static void DrawFreeCamTooltip()
{
	eSettingsManager::Keys::FreeCam& keys = SettingsMgr->keys.freeCam;

	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();

		ImGui::BeginDisabled();
		ImGui::Text("Move the camera around the world freely and explore.");
		ImGui::Separator();
		ImGui::Text("Controls:");
		ImGui::Separator();
		ImGui::Text("Forward: %s", eKeyboardMan::KeyToString(keys.iFreeCameraKeyForward));
		ImGui::Text("Backwards: %s", eKeyboardMan::KeyToString(keys.iFreeCameraKeyBackward));
		ImGui::Text("Left: %s", eKeyboardMan::KeyToString(keys.iFreeCameraKeyLeft));
		ImGui::Text("Right: %s", eKeyboardMan::KeyToString(keys.iFreeCameraKeyRight));
		ImGui::Text("Up: %s", eKeyboardMan::KeyToString(keys.iFreeCameraKeyUp));
		ImGui::Text("Down: %s", eKeyboardMan::KeyToString(keys.iFreeCameraKeyDown));
		ImGui::Separator();
		ImGui::Text("Pitch+: %s", eKeyboardMan::KeyToString(keys.iFreeCameraKeyPitchPlus));
		ImGui::Text("Pitch-: %s", eKeyboardMan::KeyToString(keys.iFreeCameraKeyPitchMinus));
		ImGui::Text("Yaw+: %s", eKeyboardMan::KeyToString(keys.iFreeCameraKeyYawPlus));
		ImGui::Text("Yaw-: %s", eKeyboardMan::KeyToString(keys.iFreeCameraKeyYawMinus));
		ImGui::Text("Roll+: %s", eKeyboardMan::KeyToString(keys.iFreeCameraKeyRollPlus));
		ImGui::Text("Roll-: %s", eKeyboardMan::KeyToString(keys.iFreeCameraKeyRollMinus));
		ImGui::Text("Reset camera: %s", eKeyboardMan::KeyToString(keys.iFreeCameraKeyReset));
		ImGui::EndDisabled();

		ImGui::EndTooltip();
	}
}

bool Menu::DrawFreeCam()
{
	bool bChanged = false;
	ImGui::Indent();
	{
		if (!freeCamLocalData.bFreeCamEnabled) ImGui::BeginDisabled();

		const ImVec2 itemSpacing = ImGui::GetStyle().ItemSpacing;

		const float posBeginPos = ImGui::GetCursorPosX();
		ImGui::SetCursorPosX(posBeginPos + (150.0f / 2.0f) - (ImGui::CalcTextSize("X").x / 2.0f));
		ImGui::Text("X");
		ImGui::SameLine(posBeginPos + itemSpacing.x + (150.0f + 150.0f / 2.0f) - (ImGui::CalcTextSize("Y").x / 2.0f));
		ImGui::Text("Y");
		ImGui::SameLine(posBeginPos + 2.0f * itemSpacing.x + (2.0f * 150.0f + 150.0f / 2.0f) - (ImGui::CalcTextSize("Z").x / 2.0f));
		ImGui::Text("Z");
		ImGui::PushItemWidth(150.0f);
		bool xChanged = ImGui::InputFloat("##freecamposX", &freeCamLocalData.fFreeCamPos[0]);
		if (xChanged) { bChanged |= xChanged; freeCamLocalData.bFreeCamModified = true; }
		ImGui::SameLine();
		bool yChanged = ImGui::InputFloat("##freecamposY", &freeCamLocalData.fFreeCamPos[1]);
		if (yChanged) { bChanged |= yChanged; freeCamLocalData.bFreeCamModified = true; }
		ImGui::SameLine();
		bool zChanged = ImGui::InputFloat("##freecamposZ", &freeCamLocalData.fFreeCamPos[2]);
		if (zChanged) { bChanged |= zChanged; freeCamLocalData.bFreeCamModified = true; }
		ImGui::PopItemWidth();
		ImGui::Dummy(ImVec2(0.0f, 8.0f));

		const float rotBeginPos = ImGui::GetCursorPosX();
		ImGui::SetCursorPosX(rotBeginPos + (150.0f / 2.0f) - (ImGui::CalcTextSize("Pitch").x / 2.0f));
		ImGui::Text("Pitch");
		ImGui::SameLine(rotBeginPos + itemSpacing.x + (150.0f + 150.0f / 2.0f) - (ImGui::CalcTextSize("Yaw").x / 2.0f));
		ImGui::Text("Yaw");
		ImGui::SameLine(rotBeginPos + 2.0f * itemSpacing.x + (2.0f * 150.0f + 150.0f / 2.0f) - (ImGui::CalcTextSize("Roll").x / 2.0f));
		ImGui::Text("Roll");
		ImGui::PushItemWidth(150.0f);
		bool pitchChanged = ImGui::InputFloat("##freecamrotPitch", &freeCamLocalData.fFreeCamRot[0]);
		if (pitchChanged) { bChanged |= pitchChanged; freeCamLocalData.bFreeCamModified = true; }
		ImGui::SameLine();
		bool yawChanged = ImGui::InputFloat("##freecamrotYaw", &freeCamLocalData.fFreeCamRot[1]);
		if (yawChanged) { bChanged |= yawChanged; freeCamLocalData.bFreeCamModified = true; }
		ImGui::SameLine();
		bool rollChanged = ImGui::InputFloat("##freecamrotRoll", &freeCamLocalData.fFreeCamRot[2]);
		if (rollChanged) { bChanged |= rollChanged; freeCamLocalData.bFreeCamModified = true; }
		ImGui::PopItemWidth();
		ImGui::Dummy(ImVec2(0.0f, 8.0f));

		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (3.0f * 150.0f + 2.0f * itemSpacing.x) / 2.0f - (ImGui::CalcTextSize("FOV").x / 2.0f));
		ImGui::Text("FOV");
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 150.0f + itemSpacing.x);
		ImGui::SetNextItemWidth(150.0f);
		bool fovChanged = ImGui::InputFloat("##freecamfov", &freeCamLocalData.fFreeCamFOV);
		if (fovChanged) { bChanged |= fovChanged; freeCamLocalData.bFreeCamModified = true; }
		ImGui::Spacing();

		if (!freeCamLocalData.bFreeCamModified) ImGui::BeginDisabled();
		ImGui::Dummy(ImVec2(0.0f, 5.0f));
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (3.0f * 150.0f + 2.0f * itemSpacing.x) / 2.0f - (100.0f / 2.0f));
		if (ImGui::Button("Reset##freecamreset", ImVec2(100.0f, 30.0f))) { freeCamLocalData.bFreeCamShouldReset = true; bChanged = true; }
		if (!freeCamLocalData.bFreeCamModified) ImGui::EndDisabled();
		ImGui::SameLine(); ShowHelpMarker("Reset free cam position, rotation and FOV back to the game's original camera values.");
		ImGui::Dummy(ImVec2(0.0f, 10.0f));

		ImGui::Indent();
		{
			if (ImGui::BeginTable("##freecamsettings", 2, ImGuiTableFlags_NoHostExtendX))
			{
				ImGui::TableSetupColumn("##freecamsettingslabels", ImGuiTableColumnFlags_WidthFixed, ImGui::CalcTextSize("Rotation speed").x);
				ImGui::TableSetupColumn("##freecamsettingssliders", ImGuiTableColumnFlags_WidthFixed, 300.0f);

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::AlignTextToFramePadding();
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetColumnWidth(0) - ImGui::CalcTextSize("Speed").x);
				ImGui::Text("Speed");
				ImGui::TableSetColumnIndex(1);
				ImGui::SetNextItemWidth(300.0f);
				bChanged |= ImGui::SliderFloat("##freecamspeed", &freeCamLocalData.fFreeCamSpeed, 0.0f, 1000.0f, "%.2f");

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::AlignTextToFramePadding();
				ImGui::Text("Rotation speed");
				ImGui::TableSetColumnIndex(1);
				ImGui::SetNextItemWidth(300.0f);
				bChanged |= ImGui::SliderFloat("##freecamrotspeed", &freeCamLocalData.fFreeCamRotationSpeed, 0.0f, 500.0f, "%.2f");

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				bool bAdvanced = ImGui::CollapsingHeader("Advanced##freecamadv", ImGuiTreeNodeFlags_SpanAllColumns);
				ImGui::EndTable();

				if (bAdvanced)
				{
					bChanged |= ImGui::Checkbox("Block overlapping inputs##freecamadvblockkeys", &freeCamLocalData.bBlockOverlappingKeys);
					ImGui::SameLine(); ShowHelpMarker("Prevents the game from registering the same key you pressed while interacting with the freecam.");
				}
			}
		}
		ImGui::Unindent();

		if (!freeCamLocalData.bFreeCamEnabled) ImGui::EndDisabled();
	}
	ImGui::Unindent();

	return bChanged;
}

void Menu::SaveFirstPersonCamData()
{
	std::lock_guard<std::mutex> lock(mtx_firstpersoncamera);

	fpCamSharedData.bFPCamEnabled = fpCamLocalData.bFPCamEnabled;
	fpCamSharedData.fFPCamFOV = fpCamLocalData.fFPCamFOV;
	fpCamSharedData.fFPCamNearClipPlane = fpCamLocalData.fFPCamNearClipPlane;
}
void Menu::LoadFirstPersonCamData()
{
	std::lock_guard<std::mutex> lock(mtx_firstpersoncamera);

	fpCamLocalData.bFPCamEnabled = fpCamSharedData.bFPCamEnabled;
}

bool Menu::DrawFirstPersonCam()
{
	bool bChanged = false;
	ImGui::Indent();
	{
		if (!fpCamLocalData.bFPCamEnabled) ImGui::BeginDisabled();
		ImGui::AlignTextToFramePadding();
		ImGui::Text("FOV");

		ImGui::SetNextItemWidth(100.0f);
		ImGui::SameLine();
		bChanged |= ImGui::InputFloat("##fpcamfov", &fpCamLocalData.fFPCamFOV);

		if (ImGui::BeginTable("##fpcamsettings", 2, ImGuiTableFlags_NoHostExtendX))
		{
			ImGui::TableSetupColumn("##fpcamsettingslabels", ImGuiTableColumnFlags_WidthFixed, ImGui::CalcTextSize("Near clip plane").x);
			ImGui::TableSetupColumn("##fpcamsettingssliders", ImGuiTableColumnFlags_WidthFixed, 100.0f);

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			bool bAdvanced = ImGui::CollapsingHeader("Advanced##fpcamadv", ImGuiTreeNodeFlags_SpanAllColumns);

			if (bAdvanced)
			{
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::AlignTextToFramePadding();
				ImGui::Text("Near clip plane");
				ImGui::TableSetColumnIndex(1);
				ImGui::SetNextItemWidth(100.0f);
				bChanged |= ImGui::InputFloat("##fpcamadvnearclipplane", &fpCamLocalData.fFPCamNearClipPlane);
				ImGui::SameLine(); ShowHelpMarker("The minimum distance from the camera at which geometry is rendered. Higher values hide more of the geometry surrounding the camera.");
			}
			ImGui::EndTable();
		}
		if (!fpCamLocalData.bFPCamEnabled) ImGui::EndDisabled();
	}
	ImGui::Unindent();

	return bChanged;
}

void Menu::DrawCameraTab()
{
	static int iCurrentCameraMode = 0;
	ImGui::RadioButton("Free camera", &iCurrentCameraMode, eCameraModes::FREECAM);
	ImGui::SameLine();
	ImGui::RadioButton("First person", &iCurrentCameraMode, eCameraModes::FIRST_PERSON);
	ImGui::Spacing();
	
	if (iCurrentCameraMode == eCameraModes::FREECAM)
	{
		bool bChanged = false;
		LoadFreeCamData();

		if (ImGui::Checkbox("Enable free cam##freecamenable", &freeCamLocalData.bFreeCamEnabled))
		{
			bChanged = true;
			if (fpCamLocalData.bFPCamEnabled)
			{
				//turn off fp cam and update shared data
				fpCamLocalData.bFPCamEnabled = false;
				{
					std::lock_guard<std::mutex> fpCamLock(TheMenu->mtx_firstpersoncamera);
					fpCamSharedData.bFPCamEnabled = fpCamLocalData.bFPCamEnabled;
				}
			}
		}
		ImGui::SameLine(); DrawFreeCamTooltip();
		bChanged |= DrawFreeCam();

		if (bChanged)
			SaveFreeCamData();
	}
	else if (iCurrentCameraMode == eCameraModes::FIRST_PERSON)
	{
		if (m_bCharacterValid.load(std::memory_order_relaxed))
		{
			LoadFirstPersonCamData();

			bool bChanged = false;
			if (ImGui::Checkbox("Enable first person cam##fpcamenable", &fpCamLocalData.bFPCamEnabled))
			{
				bChanged = true;
				if (freeCamLocalData.bFreeCamEnabled)
				{ //turn off free cam and update shared data
					freeCamLocalData.bFreeCamEnabled = false;
					{
						std::lock_guard<std::mutex> freeCamLock(TheMenu->mtx_freecamera);
						freeCamSharedData.bFreeCamEnabled = freeCamLocalData.bFreeCamEnabled;
					}
				}
			}
			ImGui::SameLine(); ShowHelpMarker("Replace the third-person camera with a first-person view.");
			bChanged |= DrawFirstPersonCam();

			if (bChanged)
				SaveFirstPersonCamData();
		}
		else
			ImGui::Text("First person mode becomes available once you are in a level!");
	}

	ImGui::Separator();
	bool bDisableDOF = m_bCameraDisableDOF.load(std::memory_order_relaxed);
	if (ImGui::Checkbox("Disable Depth Of Field##camdisableDOF", &bDisableDOF))
		m_bCameraDisableDOF.store(bDisableDOF, std::memory_order_relaxed);
}

void Menu::DrawExtrasTab()
{
	if (ImGui::BeginTabBar("##extras"))
	{

		if (ImGui::BeginTabItem("Animation"))
		{
			ImGui::Spacing();
			ImGui::AlignTextToFramePadding();
			ImGui::Text("Facial animation strength");
			ImGui::SameLine();

			float morphWeightMultiplier = TheMenu->m_fMorphWeightMultiplier.load(std::memory_order_relaxed);
			ImGui::SetNextItemWidth(300.0f);
			if (ImGui::SliderFloat("##morphweightmultiplier", &morphWeightMultiplier, 100.0f, 500.0f, "%.0f%%"))
				TheMenu->m_fMorphWeightMultiplier.store(morphWeightMultiplier, std::memory_order_relaxed);

			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Speed"))
		{
			bool bEnableChangeSpeed;
			float fSpeed = m_fSpeed;
			bool bFreezeTime;
			{
				std::lock_guard<std::mutex> lock(mtx_speed);
				bEnableChangeSpeed = m_bEnableChangeSpeed;
				bFreezeTime = m_bFreezeTime;
			}

			bool bChanged = false;
			bool bFreezeTimeChanged = ImGui::Checkbox("Freeze time##freezetime", &bFreezeTime);
			if (bFreezeTimeChanged) { bChanged |= bFreezeTimeChanged; bEnableChangeSpeed = false; }
			bool bEnableChangeSpeedChanged = ImGui::Checkbox("Enable custom game speed##enablechangespeed", &bEnableChangeSpeed);
			if (bEnableChangeSpeedChanged) { bChanged |= bEnableChangeSpeedChanged; bFreezeTime = false; }

			ImGui::Indent();
			{
				if (!bEnableChangeSpeed) ImGui::BeginDisabled();

				ImGui::AlignTextToFramePadding();
				ImGui::Text("Game speed");
				ImGui::SetNextItemWidth(300.0f);
				ImGui::SameLine();
				bChanged |= ImGui::SliderFloat("##gamespeed", &fSpeed, 100.0f, 500.0f, "%.0f%%");

				if (!bEnableChangeSpeed) ImGui::EndDisabled();
			}
			ImGui::Unindent();

			if (bChanged)
			{
				std::lock_guard<std::mutex> lock(mtx_speed);
				m_bEnableChangeSpeed = bEnableChangeSpeed;
				m_fSpeed = fSpeed;
				m_bFreezeTime = bFreezeTime;
			}

			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Misc"))
		{
			bool bEnableSprinting = m_iSprintingEnabled.load(std::memory_order_relaxed);
			if (ImGui::Checkbox("Enable sprinting##enablesprint", &bEnableSprinting))
				m_iSprintingEnabled.store(bEnableSprinting, std::memory_order_relaxed);
			ImGui::SameLine(); ShowHelpMarkerVA("Enables you to sprint using the '%s' key. This key can be configured in the Settings tab.", eKeyboardMan::KeyToString(SettingsMgr->keys.iSprint));

			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
}

void Menu::DrawSettings()
{
	ImGui::SetNextWindowPos({ ImGui::GetIO().DisplaySize.x / 2.0f, ImGui::GetIO().DisplaySize.y / 2.0f }, ImGuiCond_Once, { 0.5f, 0.5f });
	ImGui::SetNextWindowSize({ 35 * ImGui::GetFontSize(), 35 * ImGui::GetFontSize() }, ImGuiCond_Once);
	ImGui::Begin("Settings", &m_bSubmenuActive[SUBMENU_SETTINGS]);

	static int settingID = 0;
	static const char* settingNames[] = {
		"General",
		"Menu",
		"Keys",
		"Misc"
	};

	enum eSettings {
		GENERAL,
		MENU,
		KEYS,
		MISC
	};

	ImGui::BeginChild("##settings", { 12 * ImGui::GetFontSize(), 0 }, true);

	for (int n = 0; n < IM_ARRAYSIZE(settingNames); n++)
	{
		bool is_selected = (settingID == n);
		if (ImGui::Selectable(settingNames[n], is_selected))
			settingID = n;
		if (is_selected)
			ImGui::SetItemDefaultFocus();
	}

	ImGui::EndChild();

	ImGui::SameLine();
	ImGui::BeginChild("##content", { 0, -ImGui::GetFrameHeightWithSpacing() });

	bool bEnableDebugMenu = SettingsMgr->bEnableDebugMenu.load(std::memory_order_relaxed);
	
	static bool bEnableCustomMenuChar = SettingsMgr->menuCharacter.bEnableCustomMenuCharacter;
	static int iCurrentMenuChar = []() {
		int i = GetCharacterIndex(SettingsMgr->menuCharacter.szMenuCharacter);
		if (i < 0) i = 0;
		return i;
		}();
	static int iCurrentMenuVariant = []() {
		int i = GetVariantIndex(SettingsMgr->menuCharacter.szMenuCharacter, SettingsMgr->menuCharacter.szMenuVariant);
		if (i < 0) i = 0;
		return i;
		}();
	static int iCurrentEmotion = []() {
		int i = GetEmotionIndex(SettingsMgr->menuCharacter.szMenuEmotion);
		if (i < 0) i = 0;
		return i;
		}();

	static bool bMenuSettingsChanged = false;

	switch (settingID)
	{
	case GENERAL:
		ImGui::TextWrapped("These settings control udawnhook.ini options. Settings marked with a '*' require a game restart.");
		if (ImGui::Checkbox("Enable debug menu##enabledebugmenu", &bEnableDebugMenu))
			SettingsMgr->bEnableDebugMenu.store(bEnableDebugMenu, std::memory_order_relaxed);
		ImGui::SameLine(); ShowHelpMarker("Enables the game's hidden debug menu.");
		ImGui::Separator();

		ImGui::Checkbox("Enable custom menu character*##enablemenuchar", &bEnableCustomMenuChar);
		ImGui::Text("Menu character*");
		ImGui::SameLine();
		if (ImGui::Combo("##menuchar", &iCurrentMenuChar, szCharacters, NUM_CHARS))
		{
			bMenuSettingsChanged = true;
			iCurrentMenuVariant = 0;
		}
		
		ImGui::Text("Menu character variant*");
		ImGui::SameLine();
		bMenuSettingsChanged |= ImGui::Combo("##menuvariant", &iCurrentMenuVariant, characterInfo[iCurrentMenuChar].variants, characterInfo[iCurrentMenuChar].variantCount);

		ImGui::Text("Menu character emotion*");
		ImGui::SameLine();
		bMenuSettingsChanged |= ImGui::Combo("##menuemotion", &iCurrentEmotion, szEmotions, _countof(szEmotions));
		break;
	case MENU:
		ImGui::TextWrapped("These settings control udawnhook_user.ini options.");
		ImGui::Text("Menu Scale");
		ImGui::PushItemWidth(-FLT_MIN);
		ImGui::InputFloat("##", &SettingsMgr->fMenuScale);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			DEBUG_LOG(__FUNCTION__, "Font reload requested!");
			GUIImplementation::RequestFontReload();
		}
		ImGui::PopItemWidth();
		break;
	case KEYS:
		eSettingsManager::Keys	keys = SettingsMgr->keys;

		ImGui::TextWrapped("These settings control udawnhook_user.ini options.");

		if (m_bPressingKey)
			ImGui::TextColored(ImVec4(0.f, 1.f, 0.3f, 1.f), "Press a key!");

		if (ImGui::Button("Reset Keys", { -FLT_MIN, 0 }))
		{
			SettingsMgr->ResetKeys();
			Notifications->SetNotificationTime(4500);
			Notifications->PushNotification("Keys reset! Remember to save.");
			keys = SettingsMgr->keys;
		}
		ImGui::Separator();
		ImGui::LabelText("##", "Core");
		ImGui::Separator();

		KeyBind(&keys.iHookMenuOpenKey, "Open/Close Menu", "toggleMenu");
		ImGui::Separator();
		ImGui::LabelText("##", "Camera");
		ImGui::Separator();

		KeyBind(&keys.freeCam.iFreeCameraKeyYawPlus, "Yaw+", "ya_plus");
		KeyBind(&keys.freeCam.iFreeCameraKeyYawMinus, "Yaw-", "ya_minus");
		KeyBind(&keys.freeCam.iFreeCameraKeyPitchPlus, "Pitch+", "pi_plus");
		KeyBind(&keys.freeCam.iFreeCameraKeyPitchMinus, "Pitch-", "pi_minus");
		KeyBind(&keys.freeCam.iFreeCameraKeyRollPlus, "Roll+", "r_plus");
		KeyBind(&keys.freeCam.iFreeCameraKeyRollMinus, "Roll-", "r_minus");

		KeyBind(&keys.freeCam.iFreeCameraKeyForward, "Forwards", "x_plus");
		KeyBind(&keys.freeCam.iFreeCameraKeyBackward, "Backwards", "x_minus");
		KeyBind(&keys.freeCam.iFreeCameraKeyLeft, "Left", "y_plus");
		KeyBind(&keys.freeCam.iFreeCameraKeyRight, "Right", "y_minus");
		KeyBind(&keys.freeCam.iFreeCameraKeyUp, "Up", "z_plus");
		KeyBind(&keys.freeCam.iFreeCameraKeyDown, "Down", "z_minus");

		KeyBind(&keys.freeCam.iFreeCameraKeyReset, "Reset camera", "reset_cam");

		ImGui::Separator();
		ImGui::LabelText("##", "Misc");
		ImGui::Separator();

		KeyBind(&keys.iToggleFreeCameraKey, "Toggle Free Camera", "toggleFcam");
		KeyBind(&keys.iToggleFirstPersonCamKey, "Toggle First Person Camera", "togglePovcam");
		KeyBind(&keys.iToggleFreezeTime, "Toggle Freeze Time", "toggleFreezeTime");
		KeyBind(&keys.iToggleSprinting, "Toggle Sprinting", "toggleSprint");
		KeyBind(&keys.iSprint, "Sprint", "sprint");
		ImGui::Separator();

		if (m_bPressingKey)
		{
			eVKKeyCode result = eKeyboardMan::GetLastKey();

			if (result >= VK_BACKSPACE && result < VK_KEY_NONE)
			{
				*m_pCurrentVarToChange = result;
				m_bPressingKey = false;

				{
					std::lock_guard<std::mutex> lock(SettingsMgr->mtx_keys);
					SettingsMgr->keys = keys;
				}
			}
		}
		break;
	case MISC:
		ImGui::TextWrapped("These settings control udawnhook.ini options. Settings marked with a '*' require a game restart.");
		ImGui::Checkbox("Enable debug console*##enabledebugconsole", &SettingsMgr->bEnableConsoleWindow);
		ImGui::SameLine(); ShowHelpMarker("Enables the Windows CMD debug console for UDawnHook.");
		break;
	default:
		break;
	}

	if (ImGui::Button("Save", { -FLT_MIN, 0 }))
	{
		Notifications->SetNotificationTime(4500);
		Notifications->PushNotification("Settings saved to udawnhook.ini and udawnhook_user.ini!");
		SettingsMgr->SaveSettings();
		if (bMenuSettingsChanged)
		{
			eSettingsManager::MenuCharacter menuChar = {};
			menuChar.bEnableCustomMenuCharacter = bEnableCustomMenuChar;
			snprintf(menuChar.szMenuCharacter, sizeof(menuChar.szMenuCharacter), "%s", szCharacters[iCurrentMenuChar]);
			snprintf(menuChar.szMenuVariant, sizeof(menuChar.szMenuVariant), "%s", characterInfo[iCurrentMenuChar].variants[iCurrentMenuVariant]);
			snprintf(menuChar.szMenuEmotion, sizeof(menuChar.szMenuEmotion), "%s", szEmotions[iCurrentEmotion]);
			SettingsMgr->SaveMenuCharSettings(menuChar);
			bMenuSettingsChanged = false;
		}
	}

	ImGui::EndChild();

	ImGui::End();
}

void Menu::DrawKeyBind(const char* name, int* var)
{
	ImGui::SameLine();

	static char butName[256] = {};
	sprintf(butName, "%s##key%s", eKeyboardMan::KeyToString(*var), name);
	if (ImGui::Button(butName))
	{
		m_bPressingKey = true;
		m_pCurrentVarToChange = var;
	}

}

void Menu::KeyBind(int* var, const char* bindName, const char* name)
{
	ImGui::LabelText("##", bindName);
	DrawKeyBind(name, var);
}