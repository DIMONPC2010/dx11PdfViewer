#pragma once
#include "Bookmarks.h"
#include "PageBuilder.h"

class BookmarksIO
{
public:
	BookmarksIO(std::string bookmarks_path);
	~BookmarksIO();
	bool ReadBookmarksFromFile();
	bool WriteBookmarksToFile();
	bool AddBookmark(std::string filename, int page_num);
	bool DeleteBookmark(std::string filename, int page_num);
	bool BookmarksExist(std::string filename);
	int GetBookmark(int i);
	Bookmarks GetBookmarks();
	int NowBookmarksSize();

private:
	std::string m_bookmarks_path;
	std::list<Bookmarks> m_bookmarks;
	std::list<Bookmarks>::iterator m_now_bookmark;
	std::shared_ptr<PageBuilder> m_pages;
};
