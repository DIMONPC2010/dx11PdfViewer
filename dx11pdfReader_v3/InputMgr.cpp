#include "InputMgr.h"
#include "InputListener.h"

void InputMgr::Init()
{
	m_mousewheel = m_mousex = m_mousey = 0;
}

void InputMgr::Close()
{
	if (!m_Listener.empty())
		m_Listener.clear();
}

void InputMgr::SetWinRect(const RECT &winrect)
{
	m_windowrect.bottom = winrect.bottom;
	m_windowrect.left = winrect.left;
	m_windowrect.right = winrect.right;
	m_windowrect.top = winrect.top;
}

void InputMgr::AddListener(InputListener *Listener)
{
	m_Listener.push_back(Listener);
}

void InputMgr::Run(UINT &msg, WPARAM wParam, LPARAM lParam)
{
	if (m_Listener.empty())
		return;

	eKeyCodes KeyIndex;
	wchar_t buffer[1];
	BYTE lpKeyState[256];

	eventmove();

	switch (msg)
	{
	case WM_KEYDOWN:
		KeyIndex = static_cast<eKeyCodes>(wParam);
		GetKeyboardState(lpKeyState);
		ToUnicode(wParam, HIWORD(lParam) & 0xFF, lpKeyState, buffer, 1, 0);
		eventkey(KeyIndex, buffer[0], true);
		break;
	case WM_KEYUP:
		KeyIndex = static_cast<eKeyCodes>(wParam);
		GetKeyboardState(lpKeyState);
		ToUnicode(wParam, HIWORD(lParam) & 0xFF, lpKeyState, buffer, 1, 0);
		eventkey(KeyIndex, buffer[0], false);
		break;
	case WM_LBUTTONDOWN:
		eventmouse(MOUSE_LEFT, true);
		break;
	case WM_LBUTTONUP:
		eventmouse(MOUSE_LEFT, false);
		break;
	case WM_RBUTTONDOWN:
		eventmouse(MOUSE_RIGHT, true);
		break;
	case WM_RBUTTONUP:
		eventmouse(MOUSE_RIGHT, false);
		break;
	case WM_MBUTTONDOWN:
		eventmouse(MOUSE_MIDDLE, true);
		break;
	case WM_MBUTTONUP:
		eventmouse(MOUSE_MIDDLE, false);
		break;
	case WM_MOUSEWHEEL:
		mousewheel((short)GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA);
		break;
	}
}

void InputMgr::eventmove()
{
	POINT Position;
	GetCursorPos(&Position);

	Position.x -= m_windowrect.left;
	Position.y -= m_windowrect.right;

	if (m_mousex == Position.x && m_mousey == Position.y)
		return;

	m_mousex = Position.x;
	m_mousey = Position.y;

	for (auto it = m_Listener.begin(); it != m_Listener.end(); ++it)
	{
		if (!(*it))
			continue;
		else if ((*it)->MouseMove(MouseEvent(m_mousex, m_mousey)) == true)
			return;
	}
}

void InputMgr::eventmouse(const eMouseKeyCodes KeyCode, bool press)
{
	for (auto it = m_Listener.begin(); it != m_Listener.end(); ++it)
	{
		if (!(*it))
			continue;
		
		if (press == true)
		{
			if ((*it)->MousePressed(MouseEventClick(KeyCode, m_mousex, m_mousey)) == true)
				return;
		}
		else
		{
			if ((*it)->MouseReleased(MouseEventClick(KeyCode, m_mousex, m_mousey)) == true)
				return;
		}
	}
}

void InputMgr::mousewheel(short value)
{
	if (m_mousewheel == value)
		return;

	m_mousewheel = value;
	for (auto it = m_Listener.begin(); it != m_Listener.end(); ++it)
	{
		if (!(*it))
			continue;
		else if ((*it)->MouseWheel(MouseEventWheel(m_mousewheel, m_mousex, m_mousey)) == true)
			return;
	}
}

void InputMgr::eventkey(const eKeyCodes KeyCode, wchar_t ch, bool press)
{
	for (auto it = m_Listener.begin(); it != m_Listener.end(); ++it)
	{
		if (!(*it))
			continue;
		if (press == true)
		{
			if ((*it)->KeyPressed(KeyEvent(ch, KeyCode)) == true)
				return;
			else
			{
				if ((*it)->KeyReleased(KeyEvent(ch, KeyCode)) == true)
					return;
			}
		}
	}
}