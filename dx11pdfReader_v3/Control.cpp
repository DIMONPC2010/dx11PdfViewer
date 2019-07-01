#include "stdafx.h"
#include "Control.h"

Control::Control() :  m_wnd(nullptr)
					 ,m_render(nullptr)
					 ,m_input(nullptr)
					 ,m_init(false)
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
		m_pages->SearchOnPage(0, m_wnd->SearchString());
		m_render->SetSearchPages(m_pages);
		m_wnd->SetSearch();
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