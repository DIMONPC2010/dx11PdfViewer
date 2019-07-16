#include "Bookmarks.h"

Bookmarks::Bookmarks(std::string filename, int bookmarks_number)
{
	m_filename = filename;
	m_bookmarks_number = bookmarks_number;
	m_bookmarks_mas = nullptr;
}

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
	if (page_num > 0)
	{
		int *tmpmas = new int[m_bookmarks_number];
		memcpy(tmpmas, m_bookmarks_mas, sizeof(int) * m_bookmarks_number);
		m_bookmarks_mas = new int[m_bookmarks_number + 1];
		memcpy(m_bookmarks_mas, tmpmas, sizeof(int) * m_bookmarks_number);
		m_bookmarks_mas[m_bookmarks_number] = page_num;
		m_bookmarks_number++;
		delete[] tmpmas;
	}
	else if (page_num == 0)
	{
		m_bookmarks_mas = new int[m_bookmarks_number + 1];
		m_bookmarks_mas[m_bookmarks_number] = page_num;
		m_bookmarks_number++;
	}
}

void Bookmarks::DeleteBookmark(int index)
{
	for (int i = index; i < m_bookmarks_number - 1; i++)
	{
		m_bookmarks_mas[index] = m_bookmarks_mas[index + 1];
	}

	m_bookmarks_number--;
	int *tmpmas = new int[m_bookmarks_number];
	memcpy(tmpmas, m_bookmarks_mas, sizeof(int) * m_bookmarks_number);
	m_bookmarks_mas = new int[m_bookmarks_number];
	memcpy(m_bookmarks_mas, tmpmas, sizeof(int) * m_bookmarks_number);
	delete[] tmpmas;

	/*if (m_bookmarks_number == 1)
	{
		m_filename.clear();
		m_bookmarks_number = 0;
		delete[] m_bookmarks_mas;
	}
	else if (m_bookmarks_number == index + 1)
	{
		int *first_part = new int[index];
		m_bookmarks_number--;
		m_bookmarks_mas = new int[m_bookmarks_number];
		memcpy(first_part, m_bookmarks_mas, sizeof(int) * m_bookmarks_number);
		delete[]first_part;
	}
	else if (index == 0)
	{
		int *second_part = new int[m_bookmarks_number - 1];
		memcpy(second_part, m_bookmarks_mas + 1, sizeof(int) * (m_bookmarks_number - 1));
		m_bookmarks_number--;
		m_bookmarks_mas = new int[m_bookmarks_number];
		memcpy(second_part, m_bookmarks_mas, sizeof(int) * m_bookmarks_number);
		delete[]second_part;
	}
	else if (index == 1)
	{
		int first = m_bookmarks_mas[0];
		int *second_part = new int[m_bookmarks_number - 2];
		memcpy(second_part, m_bookmarks_mas + 2, sizeof(int) * (m_bookmarks_number - 2));
		m_bookmarks_number--;
		m_bookmarks_mas = new int[m_bookmarks_number];
		memcpy(second_part, m_bookmarks_mas + 1, sizeof(int) * m_bookmarks_number + 1);
		m_bookmarks_mas[0] = first;
		delete[]second_part;
	}
	else if (index == m_bookmarks_number - 2)
	{
		int last = m_bookmarks_mas[m_bookmarks_number - 1];
		int *first_part = new int[index];
		m_bookmarks_number--;
		m_bookmarks_mas = new int[m_bookmarks_number];
		memcpy(first_part, m_bookmarks_mas, sizeof(int) * index);
		m_bookmarks_mas[m_bookmarks_number - 1] = last;
		delete[]first_part;
	}
	else
	{
		int *first_part = new int[index - 1];
		memcpy(first_part, m_bookmarks_mas, sizeof(int) * index - 1);
		int *second_part = new int[m_bookmarks_number - (index)];
		memcpy(second_part, m_bookmarks_mas + index, sizeof(int) * (m_bookmarks_number - (index)));

		m_bookmarks_number--;
		m_bookmarks_mas = new int[m_bookmarks_number];
		memcpy(m_bookmarks_mas, first_part, sizeof(int) * index - 1);
		memcpy(m_bookmarks_mas + index - 1, second_part, sizeof(int) * (m_bookmarks_number - index + 1));
		delete[]first_part;
		delete[]second_part;
	}*/
}

Bookmarks::~Bookmarks()
{
	//delete[] m_bookmarks_mas;
}
