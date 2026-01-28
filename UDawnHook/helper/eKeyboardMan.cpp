#include "pch.h"
#include "eKeyboardMan.h"
#include <iostream>

bool			eKeyboardMan::ms_keyboardBuffer[VK_KEY_MAX];
bool			eKeyboardMan::ms_keyboardBufferJustPressed[VK_KEY_MAX];
int				eKeyboardMan::ms_lastKey = 0;
std::mutex		eKeyboardMan::mtx_keyboard;

eVKKeyCode eKeyboardMan::GetLastKey()
{
	std::lock_guard<std::mutex> lock(mtx_keyboard);
	return (eVKKeyCode)ms_lastKey;
}

const char* eKeyboardMan::KeyToString(int code) 
{
	thread_local static char buff[128] = {};
	UINT key = MapVirtualKey(code, MAPVK_VK_TO_VSC);

	int result = GetKeyNameTextA((key << 16), buff, sizeof(buff));

	switch (code)
	{
		case VK_UP:
			return "Up arrow";
		case VK_DOWN:
			return "Down arrow";
		case VK_LEFT:
			return "Left arrow";
		case VK_RIGHT:
			return "Right arrow";
		case VK_HOME:
			return "Home";
		case VK_INSERT:
			return "Insert";
		case VK_DELETE:
			return "Delete";
		case VK_END:
			return "End";
		case VK_NEXT:
			return "Page down";
		case VK_PRIOR:
			return "Page up";
		default:
			return result ? buff : "Unknown";
	}
}

void eKeyboardMan::SetKeyStatus(int vkKey, bool isDown)
{
	if (!(vkKey >= 0 && vkKey < VK_KEY_MAX))
		return;

	{
		std::lock_guard<std::mutex> lock(mtx_keyboard);

		bool wasKeyDown = ms_keyboardBuffer[vkKey];
		ms_keyboardBuffer[vkKey] = isDown;

		if (isDown && !wasKeyDown)
			ms_keyboardBufferJustPressed[vkKey] = isDown;
	}
}

void eKeyboardMan::SetLastPressedKey(int vkKey)
{
	std::lock_guard<std::mutex> lock(mtx_keyboard);
	ms_lastKey = vkKey;
}

void eKeyboardMan::ResetKeys()
{
	std::lock_guard<std::mutex> lock(mtx_keyboard);
	ZeroMemory(ms_keyboardBuffer, sizeof(ms_keyboardBuffer));
	ZeroMemory(ms_keyboardBufferJustPressed, sizeof(ms_keyboardBufferJustPressed));
}

int eKeyboardMan::GetKeyState(int vkKey)
{
	if (!(vkKey >= 0 && vkKey < VK_KEY_MAX))
		return 0;

	int state = 0x0;

	{
		std::lock_guard<std::mutex> lock(mtx_keyboard);

		if (ms_keyboardBufferJustPressed[vkKey])
		{
			state |= KEY_JUST_PRESSED;
			ms_keyboardBufferJustPressed[vkKey] = false;
		}

		if (ms_keyboardBuffer[vkKey])
			state |= KEY_HELD;
	}

	return state;
}

int eKeyboardMan::GetNumPressedKeys()
{
	int keys = 0;

	{
		std::lock_guard<std::mutex> lock(mtx_keyboard);
		for (int i = 0; i < VK_KEY_MAX; i++)
		{
			if (ms_keyboardBuffer[i])
				keys++;
		}
	}

	return keys;
}

void eKeyboardMan::OnFocusLost()
{
	ResetKeys();
	SetLastPressedKey(0);
}
