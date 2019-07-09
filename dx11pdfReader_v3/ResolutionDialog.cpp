#include "ResolutionDialog.h"

ResolutionDialog *ResolutionDialog::m_wndthis = nullptr;

ResolutionDialog::ResolutionDialog(Window * wind, int width, int height) : Dialog(wind), m_width(width), m_height(height), m_percent(100), m_text_changed(false), m_start_height(height), m_start_width(width), m_start_percent(100), m_dialog_ok(false)
{
	if (!m_wndthis)
		m_wndthis = this;
	else
		MessageBox(nullptr, L"ResolutionDialog уже был создан", L"Error", MB_OK);
	DialogBox(NULL, L"IDD_RESOLUTION_DIALOG", this->GetHWND(), ResolutionDlgProc);
}

BOOL ResolutionDialog::ResolutionDialogProc(HWND hwndDlg, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	int new_resolution = 0;
	switch (nMsg)
	{
	case WM_INITDIALOG:
		hwndEditHeight = GetDlgItem(hwndDlg, IDC_EDIT_HEIGHT);
		hwndEditWidth = GetDlgItem(hwndDlg, IDC_EDIT_WIDTH);
		hwndEditPercent = GetDlgItem(hwndDlg, IDC_EDIT_PERCENT);
		updateDlgWindowText();
		return FALSE;
	case WM_CLOSE:
		m_dialog_ok = false;
		m_wndthis = nullptr;
		EndDialog(hwndDlg, 0);
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			m_dialog_ok = true;
			m_wndthis = nullptr;
			EndDialog(hwndDlg, 0);
			return TRUE;
		case IDCANCEL:
			m_dialog_ok = false;
			m_wndthis = nullptr;
			EndDialog(hwndDlg, 0);
			return TRUE;
		case IDC_EDIT_HEIGHT:
			if (HIWORD(wParam) == EN_UPDATE && m_text_changed == false)
			{
				GetDlgItemText(hwndDlg, IDC_EDIT_HEIGHT, m_view_buff, 15);
				new_resolution = _wtoi(m_view_buff);
				m_percent = (int)(((float)new_resolution * 100.0f) / (float)m_start_height);
				m_width = (int)((float)m_start_width * (float)m_percent / 100.0f);
				m_height = new_resolution;

				m_text_changed = true;
				updateDlgWindowText();
				sendEmSetSelMessage(hwndEditHeight, m_height);
				m_text_changed = false;
			}
			return FALSE;
		case IDC_EDIT_WIDTH:
			if (HIWORD(wParam) == EN_UPDATE && m_text_changed == false)
			{
				GetDlgItemText(hwndDlg, IDC_EDIT_WIDTH, m_view_buff, 15);
				new_resolution = _wtoi(m_view_buff);
				m_percent = (int)(((float)new_resolution * 100.0f) / (float)m_start_width);
				m_height = (int)((float)m_start_height * (float)m_percent / 100.0f);
				m_width = new_resolution;

				m_text_changed = true;
				updateDlgWindowText();
				sendEmSetSelMessage(hwndEditWidth, m_width);
				m_text_changed = false;
			}
			return FALSE;
		case IDC_EDIT_PERCENT:
			if (HIWORD(wParam) == EN_UPDATE && m_text_changed == false)
			{
				GetDlgItemText(hwndDlg, IDC_EDIT_PERCENT, m_view_buff, 15);
				m_percent = _wtoi(m_view_buff);
				m_height = (int)((float)m_start_height * (float)m_percent / 100.0f);
				m_width = (int)((float)m_start_width * (float)m_percent / 100.0f);

				m_text_changed = true;
				updateDlgWindowText();
				sendEmSetSelMessage(hwndEditPercent, m_percent);
				m_text_changed = false;
			}
			return FALSE;
		}
		break;
	}
	return FALSE;
}

float ResolutionDialog::GetPercent()
{
	return (float)m_percent;
}

bool ResolutionDialog::ButtonOKPressed()
{
	return m_dialog_ok;
}

void ResolutionDialog::updateDlgWindowText()
{
	swprintf_s(m_view_buff, L"%d", m_height);
	SetWindowText(hwndEditHeight, m_view_buff);
	swprintf_s(m_view_buff, L"%d", m_width);
	SetWindowText(hwndEditWidth, m_view_buff);
	swprintf_s(m_view_buff, L"%d", m_percent);
	SetWindowText(hwndEditPercent, m_view_buff);
}

void ResolutionDialog::sendEmSetSelMessage(HWND hwnd, int value)
{
	SendMessage(hwnd, EM_SETSEL, (WPARAM)((int)(log10(value) + 1)), (LPARAM)((int)(log10(value) + 1)));
}



BOOL CALLBACK ResolutionDlgProc(HWND hwndDlg, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	return ResolutionDialog::Get()->ResolutionDialogProc(hwndDlg, nMsg, wParam, lParam);
}
