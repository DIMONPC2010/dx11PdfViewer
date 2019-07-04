#include "stdafx.h"
#include "Control.h"
#include <algorithm>

Control::Control() :  m_wnd(nullptr)
					 ,m_render(nullptr)
					 ,m_input(nullptr)
					 ,m_init(false)
					 ,m_search_page(0)
					 ,m_not_found(true)
					 ,m_last(false)
					 ,m_first(true)
					 ,m_forward_search(false)
{
}

Control::~Control()
{
}

void Control::SetPages()
{
	m_pages = std::make_shared<PageBuilder>(m_wnd->GetFilePath());
	m_render->RenderDocument(m_pages);	
	
	//m_render->SetSearchPages(m_pages);
}

void Control::AddInputListener(InputListener *listener)
{
	if (m_input)
		m_input->AddListener(listener);
}

void Control::Run()
{
	if (m_init)
		while (frame());
}

bool Control::Init()
{
	m_wnd = std::make_unique<Window>();
	m_input = std::make_unique<InputMgr>();

	if (!m_wnd || !m_input)
	{
		MessageBox(nullptr, L"Не удалось выделить память", L"Error", MB_OK);
		return false;
	}

	m_input->Init();

	DescWindow desc;
	if (!m_wnd->Create(desc))
	{
		MessageBox(nullptr, L"Не удалось создать окно", L"Error", MB_OK);
		return false;
	}
	m_wnd->SetInputMgr(m_input);

	if (!m_render->CreateDevice(m_wnd->GetHWND()))
	{
		MessageBox(nullptr, L"Не удалось создать рендер", L"Error", MB_OK);
		return false;
	}

	m_init = true;
	return true;
}

bool Control::frame()
{

	m_wnd->RunEvent();

	/*if (!m_wnd->IsActive())
		return true;*/


	if (m_wnd->IsExit())
		return false;

	if (m_wnd->IsResized())
	{
	}

	if (m_wnd->IsSearch())
	{
		std::wstring searchstr = m_wnd->SearchString();
		
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

		if (m_wnd->GetFindDlgFlagDown() == false)
		{
			if(m_forward_search == false)
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
		else
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
		prevSearch = searchstr;
		
	}

	m_render->BeginFrame(m_wnd->GetWindowSize());
	if (!m_render->Draw())
		return false;
	m_render->EndFrame();

	return true;
}

void Control::Close()
{
	m_init = false;
}