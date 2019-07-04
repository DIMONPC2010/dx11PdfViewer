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

	void SetFrDown(bool aState);
	void SetFrMatchCase(bool aState);
	void SetFrWholeWord(bool aState);

	bool GetFrDown();
	bool GetFrMatchCase();
	bool GetFrWholeWord();

private:
	FINDREPLACE m_findreplace;
	wchar_t m_szFindWhat[512];
	HWND m_dlghwnd;
	bool m_search;
	bool m_fr_down;
	bool m_fr_matchcase;
	bool m_fr_wholeword;
};