#include "Control.h"

Control::Control() :  m_wnd(nullptr)
					 ,m_render(nullptr)
					 ,m_input(nullptr)
					 ,m_init(false)
					 ,m_start_render(false)
					 ,m_end_render(false)
{
}

Control::~Control()
{
}

void Control::SetPages()
{
	m_pages = std::make_shared<PageBuilder>(m_wnd->GetFilePath());
	/*if (m_bookmark->BookmarksExist(m_wnd->GetStringFilePath()))
	{
		m_pages->Bookmark
	}*/
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
	m_input = std::make_shared<InputMgr>();
	m_bookmark = std::make_unique<BookmarksIO>("D:\\work\\Projects\\dx11pdfReader_v3\\dx11pdfReader_v3\\bookmarks.bin");
	m_bookmark->ReadBookmarksFromFile();

	if (!m_bookmark->ReadBookmarksFromFile())
	{
		MessageBox(nullptr, L"Не удалось открыть закладки", L"Error", MB_OK);
		return false;
	}

	m_wnd->InitBookmarks(std::move(m_bookmark));

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
	m_desc = desc;
	m_wnd->SetInputMgr(m_input);
	

	if (!m_render->CreateDevice(m_wnd->GetHWND()))
	{
		MessageBox(nullptr, L"Не удалось создать рендер", L"Error", MB_OK);
		return false;
	}
	m_render_thread = new std::thread(&Control::render_frame, this, std::ref(m_start_render), std::ref(m_end_render), std::ref(m_desc));


	m_init = true;
	return true;
}

bool Control::frame()
{

	m_wnd->RunEvent();

	/*if (!m_wnd->IsActive())
		return true;*/


	if (m_wnd->IsExit())
	{
		m_end_render = true;
		m_render_thread->join();
		return false;
	}

	if (m_wnd->IsResized())
	{
	}

	if (m_wnd->IsSearch())
	{
		if(m_search == nullptr)
			m_search = std::make_shared<SearchEngine>(m_wnd, m_render);
		m_search->Run();
	}

	if (m_wnd->IsSaveImage())
	{
		m_wnd->SetSaveImage(false);
		m_pages->SaveImage(m_wnd->GetPercent(), m_wnd->GetSaveFilePath());
	}

	if (m_wnd->IsOpenFile())
	{
		m_wnd->SetOpenFlag(false);
		SetPages();
	}

	if (m_wnd->BookmarkWasAdd())
	{
		m_pages->SetBookmark(true);
		m_wnd->SetBookmarkAdd(false);
	}

	if (m_wnd->BookmarkWasDelete())
	{
		m_pages->SetBookmark(false);
		m_wnd->SetBookmarkDelete(false);
	}

	m_wnd->SetPageResolution(m_pages->NowWidth(), m_pages->NowHeight());
	m_wnd->SetNowPage(m_pages->NowView());


	m_desc = m_wnd->GetDesc();
	m_start_render = true;

	return true;
}

void Control::render_frame(bool & start_render, bool & end_render, DescWindow & desc)
{
	int start_page = 0;
	while (!end_render)
	{
		if (start_render)
		{
			m_render->BeginFrame(desc);
			m_render->Draw();
			m_render->EndFrame();
			if (m_wnd->BookmarkWasSelected())
			{
				//m_wnd->SetBookmarkSelected(false);
				int bookmark_page = m_wnd->GetSelectedPage();
				m_render->ViewBookmark(bookmark_page);
			}
			else
			{
				start_page = m_pages->NowView();
			}
			if (m_wnd->GetMainMenuBookmark() == false &&
				m_wnd->GetGoToBookmark() == false &&
				m_wnd->BookmarkWasSelected() == true)
			{
				m_wnd->SetBookmarkSelected(false);
				m_render->ReturnFromBookmark(start_page);
			}
			if (m_wnd->GetGoToBookmark())
			{
				m_wnd->SetGoToBookmark(false);
				m_wnd->SetBookmarkSelected(false);
				int bookmark_page = m_wnd->GetSelectedPage();
				m_render->ViewBookmark(bookmark_page);
			}
		}
		Sleep(1);
	}
}

void Control::Close()
{
	m_init = false;
}