#include "Window.h"
#include "InputMgr.h"
#include "OpenDialog.h"
#include "FindDialog.h"

Window *Window::m_wndthis = nullptr;

Window::Window(void) :
	m_inputmgr(nullptr),
	m_hwnd(0),
	m_isexit(false),
	m_active(true),
	m_minimized(false),
	m_maximized(false),
	m_isresized(false)
{
	if (!m_wndthis)
		m_wndthis = this;
	else
		MessageBox(nullptr, L"Window уже был создан", L"Error", MB_OK);
}

bool Window::Create(const DescWindow &desc)
{
	m_desc = desc;

	WNDCLASSEXW wnd;

	wnd.cbSize = sizeof(WNDCLASSEXW);
	wnd.style = CS_HREDRAW | CS_VREDRAW;
	wnd.lpfnWndProc = StaticWndProc;
	wnd.cbClsExtra = 0;
	wnd.cbWndExtra = 0;
	wnd.hInstance = 0;
	wnd.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wnd.hIconSm = wnd.hIcon;
	wnd.hCursor = LoadCursor(0, IDC_ARROW);
	wnd.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wnd.lpszMenuName = NULL;
	wnd.lpszClassName = L"D3D11F";
	wnd.cbSize = sizeof(WNDCLASSEX);

	if (!RegisterClassEx(&wnd))
	{
		MessageBox(nullptr, L"Ќе удалось зарегистрировать окно", L"Error", MB_OK);
		return false;
	}

	RECT rect = { 0, 0, m_desc.width, m_desc.height };
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW | WS_VISIBLE, FALSE);

	long lwidth = rect.right - rect.left;
	long lheight = rect.bottom - rect.top;

	long lleft = (long)m_desc.posx;
	long ltop = (long)m_desc.posy;

	m_hwnd = CreateWindowEx(NULL, L"D3D11F", m_desc.caption.c_str(), WS_OVERLAPPEDWINDOW | WS_VISIBLE, lleft, ltop, lwidth, lheight, NULL, NULL, NULL, NULL);
	


	if (!m_hwnd)
	{
		MessageBox(nullptr, L"Ќе удалось создать окно", L"Error", MB_OK);
		return false;
	}

	ShowWindow(m_hwnd, SW_SHOW);
	UpdateWindow(m_hwnd);

	m_uFindReplaceMsg = RegisterWindowMessage(FINDMSGSTRING);
	return true;
}

void Window::RunEvent()
{
	MSG msg;			
	while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
	{
		if (!IsDialogMessage(m_dlghwnd, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}

void Window::Close()
{
	if (m_hwnd)
		DestroyWindow(m_hwnd);
	m_hwnd = nullptr;
}

bool Window::IsSearch()
{
	if (m_find == nullptr)
		return false;
	else
		return m_find->IsSearch();
}

DescWindow Window::GetWindowSize()
{
	return m_desc;
}

std::wstring Window::GetFilePath()
{
	return m_open->getFileName();
}

LRESULT Window::WndProc(HWND hwnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	LPFINDREPLACE fr;

	if (nMsg == m_uFindReplaceMsg)
	{
		fr = (LPFINDREPLACE)lParam;
		switch (fr->Flags)
		{
		case FR_FINDNEXT:
			m_find->Search(true);
			return 0;
		case FR_DIALOGTERM:
			m_find->Search(false);
		}
	}

	switch (nMsg)
	{
	case WM_CREATE:
		m_open = new OpenDialog(this);;
		if (!m_open->getOpenFileName())
			exit(0);
		return 0;
	case WM_CLOSE:
		m_isexit = true;
		return 0;
	case WM_ACTIVATE:
		if (LOWORD(wParam) != WA_INACTIVE)
			m_active = true;
		else
			m_active = false;
		return 0;
	case WM_MOVE:
		m_desc.posx = LOWORD(lParam);
		m_desc.posy = HIWORD(lParam);
		updateWindowState();
		return 0;
	case WM_SIZE:
		if (!m_desc.resizing)
			return 0;
		m_desc.width = LOWORD(lParam);
		m_desc.height = HIWORD(lParam);
		m_isresized = true;
		if (wParam == SIZE_MINIMIZED)
		{
			m_active = false;
			m_minimized = true;
			m_maximized = false;
		}
		else if (wParam == SIZE_MAXIMIZED)
		{
			m_active = true;
			m_minimized = false;
			m_maximized = true;
		}
		else if (wParam == SIZE_RESTORED)
		{
			if (m_minimized)
			{
				m_active = true;
				m_minimized = false;
			}
			else if (m_maximized)
			{
				m_active = true;
				m_maximized = false;
			}
		}
		updateWindowState();
		return 0;
	case WM_MOUSEMOVE: case WM_LBUTTONUP: case WM_LBUTTONDOWN: case WM_MBUTTONUP: case WM_MBUTTONDOWN: case WM_RBUTTONUP: case WM_RBUTTONDOWN: case WM_MOUSEWHEEL: case WM_KEYDOWN: case WM_KEYUP:
		if (wParam == KEY_F)
		{
			m_find = new FindDialog(this);
			m_dlghwnd = m_find->GetDlgHWND();
		}
		else if (m_inputmgr)
			m_inputmgr->Run(nMsg, wParam, lParam);
		return 0;
	}

	return DefWindowProcW(hwnd, nMsg, wParam, lParam);
}

void Window::SetInputMgr(InputMgr_t inputmgr)
{
	m_inputmgr = inputmgr;
	updateWindowState();
}

void Window::updateWindowState()
{
	RECT ClientRect;
	ClientRect.left = m_desc.posx;
	ClientRect.top = m_desc.posy;
	ClientRect.right = m_desc.width;
	ClientRect.bottom = m_desc.height;
	if (m_inputmgr)
		m_inputmgr->SetWinRect(ClientRect);
}

/*void Window::winsearch()
{
	memset(&m_fr, 0, sizeof(FINDREPLACE));
	m_fr.lStructSize = sizeof(FINDREPLACE);
	m_fr.hwndOwner = m_hwnd;
	m_fr.lpstrFindWhat = m_szFindWhat;
	m_fr.wFindWhatLen = 512;
	m_fr.Flags = 0;
	m_dlghwnd =	FindTextW(&m_fr);

}*/

LRESULT CALLBACK StaticWndProc(HWND hwnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	return Window::Get()->WndProc(hwnd, nMsg, wParam, lParam);
}
