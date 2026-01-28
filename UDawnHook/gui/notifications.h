#pragma once
#include <mutex>

class eNotificationManager {
public:
	void Update();
	void UpdateAlpha();
	void Draw();
	void SetNotificationTime(int time);
	void PushNotification(const char* format, ...);

private:
	char szMessageBuffer[2048] = {};
	bool m_bIsNotificationActive = false;
	float m_fNotifAlpha = 1.0f;
	std::mutex mtx_notifications;
};

extern eNotificationManager* Notifications;