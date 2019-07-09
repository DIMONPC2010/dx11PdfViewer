#include "Bookmarks.h"

bool Bookmarks::WriteBookmarks()
{
	return false;
}

void Bookmarks::UpdateBookmarks(std::string filename, int bookmarks_number, int * bookmarks_mas)
{
	m_filename = filename;
	m_bookmarks_number = bookmarks_number;
	m_bookmarks_mas = bookmarks_mas;
}

Bookmarks::~Bookmarks()
{
	delete[] m_bookmarks_mas;
}
