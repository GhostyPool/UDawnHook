#include "notifications.h"
#include <iostream>
#include <Windows.h>
#include "imgui\imgui.h"
#include "gui_impl.h"
#include "..\plugin\Settings.h"

eNotificationManager* Notifications = new eNotificationManager();

void eNotificationManager::Update()
{
	UpdateAlpha();
}

void eNotificationManager::UpdateAlpha()
{
	float delta = ImGui::GetIO().DeltaTime;

	float alphaSpeed = 1.8f;

	{
		std::lock_guard<std::mutex> lock(mtx_notifications);
		m_fNotifAlpha = max(m_fNotifAlpha - delta * alphaSpeed, 0.0f);


		if (m_fNotifAlpha <= 0)
			m_bIsNotificationActive = false;
	}
}

void eNotificationManager::Draw()
{
	static bool bIsNotificationActive = false;
	static float fNotifAlpha = 1.0f;
	static char szMsgBuffer[2048] = {};
	{
		std::lock_guard<std::mutex> lock(mtx_notifications);
		bIsNotificationActive = m_bIsNotificationActive;
		fNotifAlpha = m_fNotifAlpha;
		memcpy(szMsgBuffer, szMessageBuffer, sizeof(szMessageBuffer));
	}

	if (bIsNotificationActive)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, fNotifAlpha);
		ImGui::SetNextWindowPos(ImVec2(5, 10));
		ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x - 15.0f, 0.0f));
		ImGui::Begin("notif", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoInputs
			| ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing);
		ImGui::TextUnformatted(szMsgBuffer);
		ImGui::End();
		ImGui::PopStyleVar(1);
	}
}

void eNotificationManager::SetNotificationTime(int time)
{
	std::lock_guard<std::mutex> lock(mtx_notifications);
	m_fNotifAlpha = time / 1000.0f;
}

void eNotificationManager::PushNotification(const char* format, ...)
{
	std::lock_guard<std::mutex> lock(mtx_notifications);
	va_list args;
	va_start(args, format);
	vsprintf(szMessageBuffer, format, args);
	va_end(args);
	m_bIsNotificationActive = true;
}
