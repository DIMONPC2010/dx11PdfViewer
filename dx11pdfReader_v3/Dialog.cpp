#include "Dialog.h"

Dialog::Dialog(Window *wind)
{
	m_pwind = wind;
	GetWindowRect(m_pwind->GetHWND(), &m_window_rt);
}

HWND Dialog::GetHWND()
{
	return m_pwind->GetHWND();
}

DescWindow Dialog::GetDESC()
{
	return m_pwind->GetDesc();
}

int Dialog::GetPosX()
{
	return m_window_rt.left;
}

int Dialog::GetPosY()
{
	return m_window_rt.top;
}

int Dialog::GetPosCX()
{
	return m_window_rt.right - m_window_rt.left;
}

int Dialog::GetPosCY()
{
	return m_window_rt.bottom - m_window_rt.top;
}
