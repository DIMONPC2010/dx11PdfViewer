#pragma once
#include "Dialog.h"

class OpenDialog : public Dialog
{
public:
	OpenDialog(Window *wind, wchar_t *title, wchar_t *filter, DWORD flags);
	int getOpenFileName();
	int getSaveFileName();
	std::wstring getFileName();
	std::string getFileNameString();

private:
	OPENFILENAME ofn;
	wchar_t m_szFileName[512];
};