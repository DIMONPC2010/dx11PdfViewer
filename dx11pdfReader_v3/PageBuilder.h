#pragma once
#include "stdafx.h"
#include "PdfReader.h"

struct Page
{
	Page() : bookmark(false) {};
	unsigned char *RGB;
	int width;
	int height;
	int page_num;
	bool bookmark;
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
	int NowWidth();
	int NowHeight();
	void GetNext();
	void GetPrevious();
	int NowView();
	bool Bookmark(int page_num);
	void SetBookmark(bool aState);
	void SetPageBeforeBookmark();
	void BookmarkView(int page_num);
	void BookmarkReturn();

	void SaveImage(float percent, std::wstring path);

private:
	std::wstring m_filename;
	std::deque<Page> m_pageDeque;
	int m_size;
	int m_page_before_bookmark;
protected:
	PdfReader_t m_reader;

};