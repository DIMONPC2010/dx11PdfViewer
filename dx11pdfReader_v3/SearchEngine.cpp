#include "SearchEngine.h"
#include <algorithm>

SearchEngine::SearchEngine(Window_t wnd, Render_t render) : 
		  m_wnd(wnd) 
		, m_render(render)
		, m_search_page(0)
		, m_not_found(true)
		, m_last(false)
		, m_first(true)
		, m_forward_search(false)
{
	searchstr = wnd->SearchString();
	m_pages = render->GetDocument();
}

void SearchEngine::Run()
{
	searchstr = m_wnd->SearchString();
	InitFlags();
	if (m_wnd->GetFindDlgFlagDown() == false)
	{
		ForwardSearch();
	}
	else
	{
		BackwardSearch();
	}
	prevSearch = searchstr;
}

void SearchEngine::InitFlags()
{
	if (m_wnd->GetFindDlgFlagWholeWord())
	{
		std::wstring spacestr = L" ";
		searchstr = spacestr + searchstr + spacestr;
	}

	if (m_wnd->GetFindDlgFlagMachCase())
	{
		m_pages->SetCaseSensitiveSearch(true);
	}
	else
	{
		m_pages->SetCaseSensitiveSearch(false);
		std::transform(searchstr.begin(), searchstr.end(), searchstr.begin(), ::tolower);
	}
}

void SearchEngine::ForwardSearch()
{
	if (m_forward_search == false)
		m_search_page++;
	m_forward_search = true;
	if (searchstr != prevSearch && (m_search_page != 0 || m_last == true))
	{
		m_search_page = 0;
		m_last = false;
		m_pages = std::make_shared<PageBuilder>(m_wnd->GetFilePath());
		m_pages->SetCaseSensitiveSearch(m_wnd->GetFindDlgFlagMachCase());
		if (m_pages->SearchOnPageForward(m_search_page, searchstr))
		{
			m_render->SetSearchPages(m_pages);
			m_wnd->SetSearch();
			m_not_found = false;
		}
		m_search_page++;
		m_first = false;
	}
	else if (searchstr == prevSearch && m_search_page == 0)
	{
		m_pages = std::make_shared<PageBuilder>(m_wnd->GetFilePath());
		m_pages->SetCaseSensitiveSearch(m_wnd->GetFindDlgFlagMachCase());
		if (m_pages->SearchOnPageForward(m_search_page, searchstr))
		{
			m_render->SetSearchPages(m_pages);
			m_wnd->SetSearch();
			m_not_found = false;
		}
		m_search_page++;
		m_first = false;
		if (m_not_found)
		{
			m_search_page = 0;
			m_wnd->SetSearch();
			std::wstring message = L"Не найдено " + searchstr;
			MessageBox(nullptr, message.c_str(), L"Поиск", MB_OK);
		}
	}
	else if (m_search_page != m_pages->size() - 1)
	{
		if (m_pages->SearchOnPageForward(m_search_page, searchstr))
		{
			m_render->SetSearchPages(m_pages);
			m_wnd->SetSearch();
			m_not_found = false;
		}
		m_search_page++;
		m_first = false;
	}
	else if (m_search_page == m_pages->size() - 1)
	{
		if (m_pages->SearchOnPageForward(m_search_page, searchstr))
		{
			m_render->SetSearchPages(m_pages);
			m_wnd->SetSearch();
			m_not_found = false;
		}
		m_search_page = 0;
		m_last = true;
		m_first = true;
	}
	else
	{
		m_search_page++;
		m_first = false;
	}
}

void SearchEngine::BackwardSearch()
{
	if (m_forward_search)
	{
		m_search_page--;
		m_forward_search = false;
		if (m_search_page == -1)
		{
			m_search_page = m_pages->size() - 1;
			m_last = false;
			m_first = false;
		}
	}
	if (m_search_page == 0 && m_first == true)
	{
		MessageBox(nullptr, L"Достигнуто начало документа.", L"Поиск", MB_OK);
		m_wnd->SetSearch();
		if (m_pages->SearchOnPageBackward(m_search_page, searchstr))
		{
			m_render->SetSearchPages(m_pages);
			m_not_found = false;
		}
		if (m_not_found)
		{
			m_search_page = 0;
			std::wstring message = L"Не найдено " + searchstr;
			MessageBox(nullptr, message.c_str(), L"Поиск", MB_OK);
		}
	}
	else if (m_search_page != 0)
	{
		if (m_pages->SearchOnPageBackward(m_search_page, searchstr))
		{
			m_render->SetSearchPages(m_pages);
			m_wnd->SetSearch();
			m_not_found = false;
		}
		m_search_page--;
		m_last = false;
	}
	else if (m_search_page == 0 && m_first == false)
	{
		if (m_pages->SearchOnPageBackward(m_search_page, searchstr))
		{
			m_render->SetSearchPages(m_pages);
			m_wnd->SetSearch();
			m_not_found = false;
		}
		m_search_page = 0;
		m_last = false;
		m_first = true;
	}
	else
		m_search_page--;
}
