#include "Window.h"
#include "InputMgr.h"
#include "OpenDialog.h"
#include "FindDialog.h"
#include "ResolutionDialog.h"

#define ID_BOOKMARK 40500

Window *Window::m_wndthis = nullptr;

Window::Window(void) :
	 m_inputmgr(nullptr)
	,m_hwnd(0)
	,m_isexit(false)
	,m_active(true)
	,m_minimized(false)
	,m_maximized(false)
	,m_isresized(false)
	,m_dlgflags(OFN_FILEMUSTEXIST | OFN_HIDEREADONLY)
	,m_save_flag(false)
	,m_open_flag(false)
	,m_day_flag(true)
	,m_bookmark_was_selected(false)
	,m_goto_bookmark(false)
	,m_main_bookmark_selected(false)

{
	if (!m_wndthis)
	{
		m_wndthis = this;
		m_dlgtitle = L"Open PDF file";
		m_dlgfilter = L"Documents (*.pdf;*.xps;*.cbz;*.epub;*.fb2;*.zip;*.png;*.jpeg;*.tiff)\0*.zip;*.cbz;*.xps;*.epub;*.fb2;*.pdf;*.jpe;*.jpg;*.jpeg;*.jfif;*.tif;*.tiff\0PDF Files (*.pdf)\0*.pdf\0XPS Files (*.xps)\0*.xps\0CBZ Files (*.cbz;*.zip)\0*.zip;*.cbz\0EPUB Files (*.epub)\0*.epub\0FictionBook 2 Files (*.fb2)\0*.fb2\0Image Files (*.png;*.jpeg;*.tiff)\0*.png;*.jpg;*.jpe;*.jpeg;*.jfif;*.tif;*.tiff\0All Files\0*\0\0";
	}
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
		MessageBox(nullptr, L"Не удалось зарегистрировать окно", L"Error", MB_OK);
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
		MessageBox(nullptr, L"Не удалось создать окно", L"Error", MB_OK);
		return false;
	}

	hmenu = LoadMenu(NULL, MAKEINTRESOURCE(IDR_MAINMENU));
	SetMenu(m_hwnd, hmenu);

	readbookmarks();
	SetBookmarksUpdate(true);


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

void Window::SetSearch()
{
	m_find->Search(false);
}

std::wstring Window::SearchString()
{
	return m_find->GetSearchString();
}

bool Window::GetFindDlgFlagDown()
{
	return m_find->GetFrDown();
}

bool Window::GetFindDlgFlagMachCase()
{
	return m_find->GetFrMatchCase();
}

bool Window::GetFindDlgFlagWholeWord()
{
	return m_find->GetFrWholeWord();
}

void Window::SetPageResolution(int width, int height)
{
	m_page_width = width;
	m_page_height = height;
}

DescWindow Window::GetWindowSize()
{
	return m_desc;
}

std::wstring Window::GetFilePath()
{
	return m_open->getFileName();
}

std::string Window::GetStringFilePath()
{
	return m_open->getFileNameString();
}

bool Window::IsSaveImage()
{
	return m_save_flag;
}

void Window::SetSaveImage(bool aState)
{
	m_save_flag = aState;
}

std::wstring Window::GetSaveFilePath()
{
	return m_save->getFileName();
}

float Window::GetPercent()
{
	return m_percent_value;
}

bool Window::IsOpenFile()
{
	return m_open_flag;
}

void Window::SetOpenFlag(bool aState)
{
	m_open_flag = aState;
}

void Window::InitBookmarks(std::unique_ptr<BookmarksIO> bookmarks)
{
	m_bookmarks = std::move(bookmarks);
}

void Window::SetNowPage(int page_num)
{
	m_now_page = page_num;
}

bool Window::BookmarkWasSelected()
{
	return m_bookmark_was_selected;
}

void Window::SetBookmarkSelected(bool aState)
{
	m_bookmark_was_selected = aState;
}

int Window::GetSelectedPage()
{
	return m_bookmark_selected_page;
}

bool Window::GetGoToBookmark()
{
	return m_goto_bookmark;
}

void Window::SetGoToBookmark(bool aState)
{
	m_goto_bookmark = aState;
}

bool Window::GetMainMenuBookmark()
{
	return m_main_bookmark_selected;
}

bool Window::GetBookmarksUpdate()
{
	return m_bookmarks_update;
}

void Window::SetBookmarksUpdate(bool aState)
{
	m_bookmarks_update = aState;
}

std::unique_ptr<Bookmarks> Window::GetBookmarksForDocument()
{
	if (m_bookmarks->BookmarksExist(GetStringFilePath()))
	{
		std::unique_ptr<Bookmarks> tmp = std::make_unique<Bookmarks>(m_bookmarks->GetBookmarks());
		return tmp;
	}
	else
	{
		std::unique_ptr<Bookmarks> tmp = std::make_unique<Bookmarks>(GetStringFilePath(), 0);
		return tmp;
	}
}

DescWindow Window::GetDesc()
{
	return m_desc;
}

LRESULT Window::WndProc(HWND hwnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	LPFINDREPLACE fr;

	if (nMsg == m_uFindReplaceMsg)
	{
		fr = (LPFINDREPLACE)lParam;
		if (fr->Flags & FR_FINDNEXT)
		{
			m_find->Search(true);
			m_find->SetFrDown((bool)(fr->Flags & FR_DOWN));
			m_find->SetFrWholeWord((bool)(fr->Flags & FR_WHOLEWORD));
			m_find->SetFrMatchCase((bool)(fr->Flags & FR_MATCHCASE));
		}
		if (fr->Flags & FR_DIALOGTERM)
		{
			m_find->Search(false);
		}
	}
	
	switch (nMsg)
	{
	case WM_CREATE:
		m_open = new OpenDialog(this, m_dlgtitle, m_dlgfilter, m_dlgflags);
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
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case FILE_OPEN:
			deletebookmarks(hwnd, GetStringFilePath());
			m_open = new OpenDialog(this, m_dlgtitle, m_dlgfilter, m_dlgflags);
			m_open_flag = true;
			if (!m_open->getOpenFileName())
				exit(0);
			readbookmarks();
			SetBookmarksUpdate(true);
			return 0;
		case FILE_SAVE_AS:
			m_resolution = new ResolutionDialog(this, m_page_width, m_page_height);
			if (m_resolution->ButtonOKPressed())
			{
				m_percent_value = m_resolution->GetPercent();
				m_save = new OpenDialog(this, L"Save page", L"PNG Files (*.png)\0*.png\0All Files\0*\0\0", OFN_OVERWRITEPROMPT);
				m_save->DefaultExt(L".png");
				if(m_save->getSaveFileName())
					m_save_flag = true;

			}
			break;
		case FILE_SEARCH:
			SendMessage(hwnd, WM_KEYDOWN, KEY_F, 0);
			break;
		case FILE_DAY_NIGHT:
			if (m_day_flag)
			{
				m_day_flag = false;
				SendMessage(hwnd, WM_KEYDOWN, KEY_N, 0);
			}
			else
			{
				m_day_flag = true;
				SendMessage(hwnd, WM_KEYDOWN, KEY_D, 0);
			}
			break;
		case BOOKMARK_ADD:
			if (!m_bookmarks->AddBookmark(m_open->getFileNameString(), m_now_page))
			{
				HMENU subMenu = GetSubMenu(hmenu, 2);
				std::wstring str = L"Страница ";
				wchar_t page[64];
				swprintf_s(page, L"%d", m_now_page + 1);
				str += page;

				AppendMenu(subMenu, MF_STRING, ID_BOOKMARK + m_now_page, str.c_str());
				m_bookmarks_id.push_back(ID_BOOKMARK + m_now_page);
				SetMenu(hwnd, subMenu);
				m_bookmarks->WriteBookmarksToFile();
				SetBookmarksUpdate(true);
			}
			break;
		case BOOKMARK_DELETE:
			if (!m_bookmarks->DeleteBookmark(m_open->getFileNameString(), m_now_page))
			{
				HMENU subMenu = GetSubMenu(hmenu, 2);
				DeleteMenu(subMenu, ID_BOOKMARK + m_now_page, MF_BYCOMMAND);

				std::list<int>::iterator it;
				for (it = m_bookmarks_id.begin(); *it != ID_BOOKMARK + m_now_page; ++it);
				m_bookmarks_id.erase(it);

				SetMenu(hwnd, subMenu);
				m_bookmarks->WriteBookmarksToFile();
				SetBookmarksUpdate(true);
			}
			break;
		}
		if (LOWORD(wParam) == (ID_BOOKMARK + m_bookmark_selected_page))
			m_goto_bookmark = true;
		return 0;
	case WM_MENUSELECT:
		if (LOWORD(wParam) >= ID_BOOKMARK)
		{
			m_bookmark_selected_page = (int)(LOWORD(wParam) - ID_BOOKMARK);
			m_bookmark_was_selected = true;
			//OutputDebugStringA("m_bookmark_was_selected = true;\n");
		}
		return 0;
	case WM_INITMENUPOPUP:
		if (LOWORD(lParam) == 2)
			m_main_bookmark_selected = true;
		return 0;
	case WM_UNINITMENUPOPUP:
		if ((HMENU)wParam == GetSubMenu(hmenu, 2))
			m_main_bookmark_selected = false;
		return 0;
	case WM_MOUSEMOVE: case WM_LBUTTONUP: case WM_LBUTTONDOWN: case WM_MBUTTONUP: case WM_MBUTTONDOWN: case WM_RBUTTONUP: case WM_RBUTTONDOWN: case WM_MOUSEWHEEL: case WM_KEYDOWN: case WM_KEYUP:
		if (wParam == KEY_F)
		{
			m_find = new FindDialog(this);
			m_dlghwnd = m_find->GetDlgHWND();
		}
		if (m_inputmgr)
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

void Window::readbookmarks()
{
	if (m_bookmarks->BookmarksExist(GetStringFilePath()))
	{
		HMENU subMenu = GetSubMenu(hmenu, 2);
		std::wstring str = L"Страница ";
		wchar_t page[64];
		std::wstring strtowrite = L"Страница ";
		for (int i = 0; i < m_bookmarks->NowBookmarksSize(); i++)
		{

			swprintf_s(page, L"%d", m_bookmarks->GetBookmark(i) + 1);
			strtowrite = str + page;

			AppendMenu(subMenu, MF_STRING, ID_BOOKMARK + m_bookmarks->GetBookmark(i), strtowrite.c_str());
			m_bookmarks_id.push_back( ID_BOOKMARK + m_bookmarks->GetBookmark(i));
			SetMenu(m_hwnd, subMenu);
		}
	}
}

void Window::deletebookmarks(HWND hwnd, std::string filename)
{
	HMENU subMenu = GetSubMenu(hmenu, 2);
	if (m_bookmarks->BookmarksExist(GetStringFilePath()))
	{
		for (std::list<int>::iterator it = m_bookmarks_id.begin(); it != m_bookmarks_id.end(); ++it)
			DeleteMenu(subMenu, *it, MF_BYCOMMAND);
	}
	m_bookmarks_id.clear();
	SetMenu(hwnd, subMenu);
}

LRESULT CALLBACK StaticWndProc(HWND hwnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	return Window::Get()->WndProc(hwnd, nMsg, wParam, lParam);
}
