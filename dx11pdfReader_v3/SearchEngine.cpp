#include "SearchEngine.h"

bool SearchEngine::KeyPressed(const KeyEvent & arg)
{
	if (arg.code == KEY_F)
	{
		m_search = true;
	}
}

