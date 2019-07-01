#pragma once
#include "Window.h"

class Dialog
{
public:
	Dialog(Window *wind);

protected:
	HWND GetHWND();

private:
	Window *m_pwind;
	
};