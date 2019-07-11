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

std::string Bookmarks::GetFilename()
{
	return m_filename;
}

int Bookmarks::GetBookmarksNumber()
{
	return m_bookmarks_number;
}

int * Bookmarks::GetBookmarksMas()
{
	return m_bookmarks_mas;
}

int Bookmarks::GetBookmark(int i)
{
	return m_bookmarks_mas[i];
}

void Bookmarks::AddNewBookmark(int page_num)
{
	int *tmpmas = new int[m_bookmarks_number];
	memcpy(tmpmas, m_bookmarks_mas, sizeof(int) * m_bookmarks_number);
	m_bookmarks_mas = new int[m_bookmarks_number + 1];
	memcpy(m_bookmarks_mas, tmpmas, sizeof(int) * m_bookmarks_number);
	m_bookmarks_mas[m_bookmarks_number] = page_num;
	m_bookmarks_number++;
}

Bookmarks::~Bookmarks()
{
	delete[] m_bookmarks_mas;
}
