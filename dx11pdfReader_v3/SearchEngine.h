#pragma once

#include "stdafx.h"
#include "InputListener.h"
#include "PageBuilder.h"

class SearchEngine : public InputListener
{
private:
	typedef std::unique_ptr<PdfReader> PdfReader_t;

public:
	bool KeyPressed(const KeyEvent &arg);

	SearchEngine();

private:
	bool m_search;
};