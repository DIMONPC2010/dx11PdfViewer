#pragma once

#include "stdafx.h"
#include "Window.h"
#include "Render.h"
#include "InputMgr.h"
#include "PageBuilder.h"
#include "SearchEngine.h"
#include "BookmarksIO.h"


class Control
{
private:
	typedef std::shared_ptr<Window> Window_t;
	typedef std::shared_ptr<Render> Render_t;
	typedef std::shared_ptr<InputMgr> InputMgr_t;
	typedef std::shared_ptr<PageBuilder> PageBuilder_t;
	typedef std::shared_ptr<SearchEngine> SearchEngine_t;
	typedef std::unique_ptr<BookmarksIO> BookmarkIO_t;

public: 
	Control();
	~Control();

	bool Init();
	void Run();
	void Close();

	void SetRender(Render_t render) { m_render = render; }
	void SetPages();
	void AddInputListener(InputListener *listener);

private:
	bool frame();
	void render_frame(bool &start_render, bool &end_render, DescWindow &desc);

	Window_t m_wnd;
	Render_t m_render;
	InputMgr_t m_input;
	PageBuilder_t m_pages;
	SearchEngine_t m_search;
	BookmarkIO_t m_bookmark;
	bool m_init;

	std::thread *m_render_thread;
	bool m_start_render;
	bool m_end_render;
	DescWindow m_desc;

};