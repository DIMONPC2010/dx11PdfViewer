#pragma once
#include "stdafx.h"
#include "PdfReader.h"

struct Page
{
	unsigned char *RGB;
	int width;
	int height;
	int page_num;
};

class PageBuilder
{
private:
	typedef std::shared_ptr<PdfReader> PdfReader_t;

public:
	PageBuilder(std::wstring filename);
	void GetStartPages();
	unsigned char *GetPage(int page_num);
	bool SearchOnPageForward(int page_num, std::wstring searchtext);
	bool SearchOnPageBackward(int page_num, std::wstring searchtext);
	void SetCaseSensitiveSearch(bool aState);
	int size();
	int width(int page_num);
	int height(int page_num);
	void GetNext();
	void GetPrevious();
	int NowView();

private:
	std::wstring m_filename;
	std::deque<Page> m_pageDeque;
	int m_size;
protected:
	PdfReader_t m_reader;

};