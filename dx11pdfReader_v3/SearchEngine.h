#pragma once

#include "stdafx.h"
#include "InputListener.h"
#include "PageBuilder.h"

class SearchEngine
{
private:
	typedef std::unique_ptr<PageBuilder> PageBuilder_t;

public:
	//SearchEngine();
	//void SetString(wchar_t *searchstring);
	//void Run();
	//void UpdateRender();

private:
	bool m_search;
};