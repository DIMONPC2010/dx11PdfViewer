#pragma once
#include "Dialog.h"


class ResolutionDialog : Dialog
{
public:
	static ResolutionDialog* Get() { return m_wndthis; }

	ResolutionDialog(Window *wind, int width, int height);
	~ResolutionDialog();
	BOOL ResolutionDialogProc(HWND hwndDlg, UINT nMsg, WPARAM wParam, LPARAM lParam);
	float GetPercent();
	bool ButtonOKPressed();

private:
	void updateDlgWindowText();
	void sendEmSetSelMessage(HWND hwnd, int value);

	static ResolutionDialog *m_wndthis;
	int m_width;
	int m_height;
	int m_percent;

	const int m_start_width;
	const int m_start_height;
	const int m_start_percent;

	HWND hwndEditHeight;
	HWND hwndEditWidth;
	HWND hwndEditPercent;
	wchar_t m_view_buff[15];
	bool m_text_changed;
	bool m_dialog_ok;
};

static BOOL CALLBACK ResolutionDlgProc(HWND hwndDlg, UINT nMsg, WPARAM wParam, LPARAM lParam);