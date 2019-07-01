#include "OpenDialog.h"

OpenDialog::OpenDialog(Window *wind) : Dialog(wind)
{
	memset(&ofn, 0, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = GetHWND();
	ofn.lpstrFile = m_szFileName;
	ofn.nMaxFile = 512;
	ofn.lpstrInitialDir = NULL;
	ofn.lpstrTitle = L"Open PDF file";
	ofn.lpstrFilter = L"Documents (*.pdf;*.xps;*.cbz;*.epub;*.fb2;*.zip;*.png;*.jpeg;*.tiff)\0*.zip;*.cbz;*.xps;*.epub;*.fb2;*.pdf;*.jpe;*.jpg;*.jpeg;*.jfif;*.tif;*.tiff\0PDF Files (*.pdf)\0*.pdf\0XPS Files (*.xps)\0*.xps\0CBZ Files (*.cbz;*.zip)\0*.zip;*.cbz\0EPUB Files (*.epub)\0*.epub\0FictionBook 2 Files (*.fb2)\0*.fb2\0Image Files (*.png;*.jpeg;*.tiff)\0*.png;*.jpg;*.jpe;*.jpeg;*.jfif;*.tif;*.tiff\0All Files\0*\0\0";
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
}

int OpenDialog::getOpenFileName()
{
	return GetOpenFileNameW(&this->ofn);;
}

std::wstring OpenDialog::getFileName()
{
	return m_szFileName;
}
