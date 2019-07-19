#pragma once

#include "stdafx.h"
#include "resource.h"
#include "BookmarksIO.h"

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

class InputMgr;
class OpenDialog;
class FindDialog;
class ResolutionDialog;

struct DescWindow
{
	DescWindow() :
		caption(L"dx11pdfReader"),
		width(1067),
		height(600),
		posx(200),
		posy(20),
		resizing(true)
	{}

	int posx;
	int posy;
	std::wstring caption;
	int width;
	int height;
	bool resizing;
};

class Window
{
private:
	typedef std::shared_ptr<InputMgr> InputMgr_t;

public:
	Window();

	static Window* Get() { return m_wndthis; }

	bool Create(const DescWindow &desc);
	void RunEvent();
	void Close();
	void SetInputMgr(InputMgr_t inputmgr);

	HWND GetHWND() const { return m_hwnd; }
	int Window::GetLeft() const { return m_desc.posx; }
	int Window::GetTop() const { return m_desc.posy; }
	int Window::GetWidth() const { return m_desc.width; }
	int Window::GetHeight() const { return m_desc.height; }
	const std::wstring& GetCaption() const { return m_desc.caption; }

	bool IsExit() const { return m_isexit; }
	bool IsActive() const { return m_active; }
	bool IsResized()
	{
		bool ret = m_isresized;
		m_isresized = false;
		return ret;
	}
	bool IsSearch();
	void SetSearch();
	std::wstring SearchString();
	bool GetFindDlgFlagDown();
	bool GetFindDlgFlagMachCase();
	bool GetFindDlgFlagWholeWord();

	void SetPageResolution(int width, int height);

	DescWindow GetWindowSize();
	std::wstring GetFilePath();
	std::string GetStringFilePath();

	bool IsSaveImage();
	void SetSaveImage(bool aState);
	std::wstring GetSaveFilePath();
	float GetPercent();

	bool IsOpenFile();
	void SetOpenFlag(bool aState);

	void InitBookmarks(std::unique_ptr<BookmarksIO> bookmarks);
	void SetNowPage(int page_num);
	bool BookmarkWasSelected();
	void SetBookmarkSelected(bool aState);
	int GetSelectedPage();
	bool GetGoToBookmark();
	void SetGoToBookmark(bool aState);
	bool GetMainMenuBookmark();
	bool GetBookmarksUpdate();
	void SetBookmarksUpdate(bool aState);
	std::unique_ptr<Bookmarks> GetBookmarksForDocument();

	DescWindow GetDesc();

	LRESULT WndProc(HWND hwnd, UINT nMsg, WPARAM wParam, LPARAM lParam);
private:
	void updateWindowState();
	void readbookmarks();
	void deletebookmarks(HWND hwnd, std::string filename);

	static Window *m_wndthis;

	DescWindow m_desc;
	InputMgr_t m_inputmgr;
	HWND m_hwnd;
	bool m_isexit;
	bool m_active;
	bool m_minimized;
	bool m_maximized;
	bool m_isresized;


	OpenDialog *m_open;
	wchar_t *m_dlgtitle;
	wchar_t *m_dlgfilter;
	DWORD m_dlgflags;
	bool m_open_flag;

	OpenDialog *m_save;
	float m_percent_value;
	bool m_save_flag;

	FindDialog *m_find;
	ResolutionDialog *m_resolution;
	int m_page_width, m_page_height;
	HWND m_dlghwnd;
	UINT m_uFindReplaceMsg;

	bool m_day_flag;

	HMENU hmenu;

	std::unique_ptr<BookmarksIO> m_bookmarks;
	int m_now_page;
	bool m_bookmark_was_selected;
	int m_bookmark_selected_page;
	bool m_goto_bookmark;
	bool m_main_bookmark_selected;
	bool m_bookmarks_update;
	std::list<int> m_bookmarks_id;

};

static LRESULT CALLBACK StaticWndProc(HWND hwnd, UINT nMsg, WPARAM wParam, LPARAM lParam);;