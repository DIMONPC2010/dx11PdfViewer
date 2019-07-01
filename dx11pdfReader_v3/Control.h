#pragma once

#include "Window.h"
#include "Render.h"
#include "InputMgr.h"
#include "PageBuilder.h"
#include "SearchEngine.h"


class Control
{
private:
	typedef std::unique_ptr<Window> Window_t;
	typedef std::shared_ptr<Render> Render_t;
	typedef std::shared_ptr<InputMgr> InputMgr_t;
	typedef std::shared_ptr<PageBuilder> PageBuilder_t;
	typedef std::unique_ptr<SearchEngine> SearchEngin_t;

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

	Window_t m_wnd;
	Render_t m_render;
	InputMgr_t m_input;
	PageBuilder_t m_pages;
	SearchEngin_t m_search;
	bool m_init;

};