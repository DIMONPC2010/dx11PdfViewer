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

int OpenDialog::getSaveFileName()
{
	return GetSaveFileNameW(&this->ofn);
}

std::wstring OpenDialog::getFileName()
{
	return m_szFileName;
}

std::string OpenDialog::getFileNameString()
{
	char filename[512];

	int code = WideCharToMultiByte(CP_UTF8, 0, m_szFileName, -1, filename, MAX_PATH, NULL, NULL);
	if (code == 0)
		MessageBox(nullptr, L"Не удалось конвертировать в utf-8", L"Error", MB_OK);
	return filename;
}

void OpenDialog::DefaultExt(wchar_t * ext)
{
	ofn.lpstrDefExt = ext;
}
