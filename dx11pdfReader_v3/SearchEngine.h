#pragma once

#include "stdafx.h"
#include "Render.h"
#include "Window.h"

class Render;
class Window;
class PageBuilder;

class SearchEngine
{
private:
	typedef std::shared_ptr<Render> Render_t;
	typedef std::shared_ptr<PageBuilder> PageBuilder_t;
	typedef std::shared_ptr<Window> Window_t;

public:
	SearchEngine(Window_t wnd, Render_t render);
	void Run();
	void InitFlags();
	void ForwardSearch();
	void BackwardSearch();

private:
	Window_t m_wnd;
	PageBuilder_t m_pages;
	Render_t m_render;
	std::wstring searchstr;
	int m_search_page;
	std::wstring prevSearch;
	bool m_not_found;
	bool m_last;
	bool m_first;
	bool m_forward_search;
};