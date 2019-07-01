#pragma once
#include "Dialog.h"

class OpenDialog : public Dialog
{
public:
	OpenDialog(Window *wind);
	int getOpenFileName();
	std::wstring getFileName();

private:
	OPENFILENAME ofn;
	wchar_t m_szFileName[512];
};