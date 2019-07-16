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

bool BookmarksIO::AddBookmark(std::string filename, int page_num)
{
	Bookmarks *tmpBookMark = new Bookmarks(filename, 0);
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
		tmpBookMark->AddNewBookmark(page_num);
		m_bookmarks.push_back(*tmpBookMark);
		return false;
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
			return false;
		}
		else
		{
			MessageBox(nullptr, L"Закладка уже существует", L"Info", MB_OK);
			return true;
		}
	}
}

bool BookmarksIO::DeleteBookmark(std::string filename, int page_num)
{
	if (m_bookmarks.empty())
	{
		MessageBox(nullptr, L"Закладка не существует", L"Info", MB_OK);
		return true;
	}

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
		MessageBox(nullptr, L"Нет закладок для файла", L"Info", MB_OK);
		return true;
	}
	else
	{
		exist = false;
		int index = 0;
		for (int i = 0; i < it->GetBookmarksNumber(); i++)
		{
			if (it->GetBookmark(i) == page_num)
			{
				exist = true;
				index = i;
				break;
			}
		}
		if (!exist)
		{
			MessageBox(nullptr, L"Нет закладки на открытой странице", L"Info", MB_OK);
			return true;
		}
		else
		{
			it->DeleteBookmark(index);
			
			return false;
		}
	}
}

bool BookmarksIO::BookmarksExist(std::string filename)
{
	for (m_now_bookmark = m_bookmarks.begin(); m_now_bookmark != m_bookmarks.end(); ++m_now_bookmark)
	{
		if (m_now_bookmark->GetFilename() == filename)
			return true;
	}
	return false;
}

int BookmarksIO::GetBookmark(int i)
{
	return m_now_bookmark->GetBookmark(i);
}

int BookmarksIO::NowBookmarksSize()
{
	return m_now_bookmark->GetBookmarksNumber();
}
