#pragma once
#include "stdafx.h"


class Bookmarks
{
public:
	bool WriteBookmarks();
	void UpdateBookmarks(std::string filename, int bookmarks_number, int *bookmarks_mas);
	~Bookmarks();

private:
	std::string m_filename;
	int m_bookmarks_number;
	int *m_bookmarks_mas;
};