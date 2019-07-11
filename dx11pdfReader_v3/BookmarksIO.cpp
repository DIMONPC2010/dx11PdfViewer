#include "BookmarksIO.h"


BookmarksIO::BookmarksIO(std::string bookmarks_path) : m_bookmarks_path(bookmarks_path)
{
	std::ofstream file;
	file.open(bookmarks_path.c_str(), std::ios::out | std::ios::app);
	if (!file)
	{
		std::ofstream create(bookmarks_path.c_str());
		create.close();
	}
	file.close();
}

bool BookmarksIO::ReadBookmarksFromFile()
{
	Bookmarks tmpBookmark;
	//char *filename = new char[MAX_PATH];
	int filenameSize = 0;
	int dataSize = 0;
	int dataSizeAcum = 0;
	int counter = 0;


	std::ifstream data(m_bookmarks_path.c_str(), std::ios::binary | std::ios::in);

	if (!data)
	{
		MessageBox(nullptr, L"Не удалось открыть закладки.", L"Error", MB_OK);
		return false;
	}

	while (true)
	{
		data.seekg(dataSizeAcum);

		data.read((char*)&filenameSize, sizeof(int));
		if (data.eof())
			break;
		dataSizeAcum += sizeof(int);
		data.seekg(dataSizeAcum);

		char *filename = new char[filenameSize + 1];
		data.read(filename, filenameSize * sizeof(char));
		filename[filenameSize] = '\0';
		dataSizeAcum += filenameSize * sizeof(char);
		data.seekg(dataSizeAcum);

		data.read((char*)&dataSize, sizeof(int));
		dataSizeAcum += sizeof(int);
		data.seekg(dataSizeAcum);

		int *intdata = new int[dataSize];
		data.read((char*)intdata, dataSize * sizeof(int));
		dataSizeAcum += dataSize * sizeof(int);
		
		tmpBookmark.UpdateBookmarks(filename, dataSize, intdata);
		m_bookmarks.push_back(tmpBookmark);

		//delete[] intdata;
		delete[] filename;
	}

	data.close();

	return true;

}

bool BookmarksIO::WriteBookmarksToFile()
{
	std::ofstream data(m_bookmarks_path.c_str(), std::ios::binary | std::ios::out);
	if (!data)
	{
		MessageBox(nullptr, L"Не удалось записать закладки.", L"Error", MB_OK);
		return false;
	}

	int filenameSize = 0;
	int bookmarksnumber = 0;
	

	for (std::list<Bookmarks>::iterator it = m_bookmarks.begin(); it != m_bookmarks.end(); ++it)
	{
		filenameSize = it->GetFilename().size();
		data.write((char*)&filenameSize, sizeof(int));

		data.write(it->GetFilename().c_str(), it->GetFilename().size() * sizeof(char));

		bookmarksnumber = it->GetBookmarksNumber();
		data.write((char*)&bookmarksnumber, sizeof(int));

		int * intmas = it->GetBookmarksMas();
		data.write((char*)intmas, sizeof(int) * it->GetBookmarksNumber());
	}

	data.close();
	return true;
}

void BookmarksIO::AddBookmark(std::string filename, int page_num)
{
	Bookmarks *tmpBookMark = new Bookmarks();
	bool exist = false;
	std::list<Bookmarks>::iterator it = m_bookmarks.begin();
	for (it; it != m_bookmarks.end(); ++it)
	{
		if (it->GetFilename() == filename)
		{
			exist = true;
			break;
		}
	}
	if (!exist)
	{
		int *mas = new int [1];
		mas[0] = page_num;
		tmpBookMark->UpdateBookmarks(filename, 1, mas);
		delete[] mas;
		m_bookmarks.push_back(*tmpBookMark);
	}
	else
	{
		exist = false;
		for (int i = 0; i < it->GetBookmarksNumber(); i++)
		{
			if (it->GetBookmark(i) == page_num)
			{
				exist = true;
				break;
			}
		}
		if (!exist)
		{
			it->AddNewBookmark(page_num);
		}

	}
}