#include "BookmarksIO.h"


BookmarksIO::BookmarksIO(std::string bookmarks_path) : m_bookmarks_path(bookmarks_path)
{
}

bool BookmarksIO::ReadBookmarksFromFile()
{
	Bookmarks tmpBookmark;
	char *filename = new char[MAX_PATH];
	int dataSize = 0;
	int dataSizeAcum = 0;
	int counter = 0;


	std::ifstream data(m_bookmarks_path.c_str(), std::ios::binary | std::ios::in);
	while (true)
	{
		data.seekg(counter + dataSizeAcum * 4 + counter * 4);

		data.read(filename, 1);
		if (data.eof())
			break;

		data.seekg(1 + counter + dataSizeAcum * 4 + counter * 4);
		data.read((char*)&dataSize, 4);
		data.seekg(1 + counter + dataSizeAcum * 4 + counter * 4 + 4);

		int *intdata = new int[dataSize];
		data.read((char*)intdata, dataSize * 4);
		
		tmpBookmark.UpdateBookmarks(filename, dataSize, intdata);

		delete[] intdata;
		counter++;
		dataSizeAcum += dataSize;
	}

	data.close();

	return true;

}

bool BookmarksIO::WriteBookmarksToFile()
{
	return false;
}
