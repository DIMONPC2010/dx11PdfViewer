#include "FindDialog.h"

FindDialog::FindDialog(Window *wind) : Dialog(wind)
{
	memset(&m_findreplace, 0, sizeof(FINDREPLACE));
	m_findreplace.lStructSize = sizeof(FINDREPLACE);
	m_findreplace.hwndOwner = GetHWND();;
	m_findreplace.lpstrFindWhat = m_szFindWhat;
	m_findreplace.wFindWhatLen = 512;
	m_findreplace.Flags = 0;
	m_dlghwnd = FindTextW(&m_findreplace);
	m_search = false;
}

HWND FindDialog::GetDlgHWND()
{
	return m_dlghwnd;
}

void FindDialog::Search(bool aState)
{
	m_search = aState;
}

bool FindDialog::IsSearch()
{
	return m_search;
}

std::wstring FindDialog::GetSearchString()
{
	return m_szFindWhat;
}
