#include "Dialog.h"

Dialog::Dialog(Window *wind)
{
	m_pwind = wind;
}

HWND Dialog::GetHWND()
{
	return m_pwind->GetHWND();
}
