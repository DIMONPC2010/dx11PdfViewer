#include "OpenDialog.h"

OpenDialog::OpenDialog(Window *wind, wchar_t *title, wchar_t *filter, DWORD flags) : Dialog(wind)
{
	memset(&ofn, 0, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = GetHWND();
	ofn.lpstrFile = m_szFileName;
	ofn.nMaxFile = 512;
	ofn.lpstrInitialDir = NULL;
	ofn.lpstrTitle = title;
	ofn.lpstrFilter = filter;
	ofn.Flags = flags;
}

int OpenDialog::getOpenFileName()
{
	return GetOpenFileNameW(&this->ofn);;
}

std::wstring OpenDialog::getFileName()
{
	return m_szFileName;
}
