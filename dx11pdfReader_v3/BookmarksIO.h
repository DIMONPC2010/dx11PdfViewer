#pragma once
#include "Bookmarks.h"

class BookmarksIO
{
public:
	BookmarksIO(std::string bookmarks_path);
	bool ReadBookmarksFromFile();
	bool WriteBookmarksToFile();

private:
	std::string m_bookmarks_path;
	std::list<Bookmarks> m_bookmarks;
};
