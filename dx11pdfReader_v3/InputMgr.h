#pragma once

#include "stdafx.h"
#include "InputCodes.h"

class InputListener;

class InputMgr
{
public:
	void Init();
	void Close();

	void Run(UINT &msg, WPARAM wParam, LPARAM lParam);
	void AddListener(InputListener *Listener);
	void SetWinRect(const RECT &winrect);

private:
	void eventmove();
	void eventmouse(const eMouseKeyCodes KeyCode, bool press);
	void mousewheel(short value);
	void eventkey(const eKeyCodes KeyCode, wchar_t ch, bool press);

	std::list<InputListener*> m_Listener;

	RECT m_windowrect;
	float m_mousex;
	float m_mousey;
	int m_mousewheel;
};