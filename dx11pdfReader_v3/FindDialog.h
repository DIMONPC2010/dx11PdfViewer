#pragma once
#include "Dialog.h"
#include "SearchEngine.h"

class FindDialog : public Dialog
{
public:
	FindDialog(Window *wind);
	HWND GetDlgHWND();
	void Search(bool aState);
	bool IsSearch();
	std::wstring GetSearchString();

private:
	FINDREPLACE m_findreplace;
	wchar_t m_szFindWhat[512];
	HWND m_dlghwnd;
	bool m_search;
};