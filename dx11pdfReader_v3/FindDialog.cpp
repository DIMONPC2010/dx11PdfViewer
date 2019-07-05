#include "FindDialog.h"
#include <dlgs.h>

FindDialog::FindDialog(Window *wind) : Dialog(wind)
			,m_fr_down(false)
			,m_fr_matchcase(false)
			,m_fr_wholeword(false)
{
	memset(&m_findreplace, 0, sizeof(FINDREPLACE));
	m_findreplace.lStructSize = sizeof(FINDREPLACE);
	m_findreplace.hwndOwner = GetHWND();;
	m_findreplace.lpstrFindWhat = m_szFindWhat;
	m_findreplace.wFindWhatLen = 512;
	m_findreplace.Flags = 0;
	m_dlghwnd = FindTextW(&m_findreplace);
	m_search = false;

	
	HWND hwndRadioButton = GetDlgItem(m_dlghwnd, rad1);
	SetWindowText(hwndRadioButton, L"Вперед");
	hwndRadioButton = GetDlgItem(m_dlghwnd, rad2);
	SetWindowText(hwndRadioButton, L"Назад");
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

void FindDialog::SetFrMatchCase(bool aState)
{
	m_fr_matchcase = aState;
}

void FindDialog::SetFrWholeWord(bool aState)
{
	m_fr_wholeword = aState;
}

bool FindDialog::GetFrDown()
{
	return m_fr_down;
}

bool FindDialog::GetFrMatchCase()
{
	return m_fr_matchcase;
}

bool FindDialog::GetFrWholeWord()
{
	return m_fr_wholeword;
}

void FindDialog::SetFrDown(bool aState)
{
	m_fr_down = aState;
}
