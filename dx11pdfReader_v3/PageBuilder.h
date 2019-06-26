#pragma once
#include "stdafx.h"
#include "PdfReader.h"

struct Page
{
	float *RGB;
	int width;
	int height;
	int page_num;
};

class PageBuilder
{
private:
	typedef std::unique_ptr<PdfReader> PdfReader_t;

public:
	PageBuilder(std::string filename);
	void GetStartPages();
	float *GetPage(int page_num);
	void SearchOnPage(int page_num);
	int size();
	int width(int page_num);
	int height(int page_num);
	void GetNext();
	void GetPrevious();
	int NowView();

private:
	std::string m_filename;
	std::deque<Page> m_pageDeque;
	int m_size;
protected:
	PdfReader_t m_reader;

};