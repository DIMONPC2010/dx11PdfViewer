#pragma once
#include "Window.h"

class Dialog
{
public:
	Dialog(Window *wind);

protected:
	HWND GetHWND();
	DescWindow GetDESC();
	int GetPosX();
	int GetPosY();
	int GetPosCX();
	int GetPosCY();

private:
	Window *m_pwind;
	RECT m_window_rt;
};