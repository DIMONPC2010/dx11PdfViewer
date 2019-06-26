#include "PdfReader.h"
#define C_BTOF 0.00392156862745 

PdfReader::PdfReader() :
	 m_zoom(100)
	,m_rotate(0)
	,m_page_count(0)
{
}

PdfReader::~PdfReader()
{
	fz_drop_pixmap(m_ctx, m_pix);
	fz_drop_document(m_ctx, m_doc);
	fz_drop_context(m_ctx);
}


int PdfReader::GetDocumentSize()
{
	return m_page_count;
}

int PdfReader::width()
{
	return m_pix->w;
}

int PdfReader::height()
{
	return m_pix->h;
}

bool PdfReader::SearchText(int page_num)
{
	int firstpage, page;
	firstpage = 0;
	page = m_page_count - 1;

	//fz_ne

	//fz_new_stext_page(m_ctx,)
	fz_stext_page *page_text;
	fz_cookie cookie = { 0 };

	fz_try(m_ctx)
	{
		fz_page *page_for_s = fz_load_page(m_ctx, m_doc, page_num);
		fz_rect page_bbox = fz_bound_page(m_ctx, page_for_s);
		page_text = fz_new_stext_page(m_ctx, page_bbox);
		fz_device *tdev = fz_new_stext_device(m_ctx, page_text, NULL);
		fz_display_list *d_list = fz_new_display_list(m_ctx, fz_infinite_rect);
		fz_device *mdev = fz_new_list_device(m_ctx, d_list);
		fz_enable_device_hints(m_ctx, mdev, FZ_NO_CACHE);
		fz_run_page_contents(m_ctx, page_for_s, mdev, fz_identity, &cookie);
		fz_close_device(m_ctx, mdev);
		fz_drop_device(m_ctx, mdev);

		fz_run_display_list(m_ctx, d_list, tdev, m_ctm, fz_infinite_rect, &cookie);
		fz_close_device(m_ctx, tdev);
	}
	fz_catch(m_ctx)
	{
		MessageBox(nullptr, L"Не удалось открыть документ", L"Error", MB_OK);
		fz_drop_context(m_ctx);
		return EXIT_FAILURE;
	}

	//do
	//{
		hit_count = fz_search_stext_page(m_ctx, page_text, "Power\0", m_hit_bbox, sizeof(m_hit_bbox) / sizeof(m_hit_bbox[0]));
	//} 
	//while (page != firstpage);
		fz_invert_pixmap_rect(m_ctx, m_pix, fz_round_rect(fz_rect_from_quad(m_hit_bbox[0])));
		//pixmapToDocForRender();

		return true;
}


bool PdfReader::ReadPdfDocument(std::string filename, int page_num)
{
	m_filename = filename.c_str();
	m_ctx = fz_new_context(NULL, NULL, FZ_STORE_UNLIMITED);
	if (!m_ctx)
	{
		MessageBox(nullptr, L"Не удалось создать контекст mupdf", L"Error", MB_OK);
		return EXIT_FAILURE;
	}

	fz_try(m_ctx)
		fz_register_document_handlers(m_ctx);
	fz_catch(m_ctx)
	{
		MessageBox(nullptr, L"Не удалось зарегестрировать хендл документа", L"Error", MB_OK);
		fz_drop_context(m_ctx);
		return EXIT_FAILURE;
	}

	/* Open the document. */
	fz_try(m_ctx)
		m_doc = fz_open_document(m_ctx, m_filename);
	fz_catch(m_ctx)
	{
		MessageBox(nullptr, L"Не удалось открыть документ", L"Error", MB_OK);
		fz_drop_context(m_ctx);
		return EXIT_FAILURE;
	}
	/* Count the number of pages. */
	fz_try(m_ctx)
		m_page_count = fz_count_pages(m_ctx, m_doc);
	fz_catch(m_ctx)
	{
		MessageBox(nullptr, L"Не удалось посчитать количество страниц", L"Error", MB_OK);
		fz_drop_document(m_ctx, m_doc);
		fz_drop_context(m_ctx);
		return EXIT_FAILURE;
	}

	if (page_num < 0 || page_num >= m_page_count)
	{
		MessageBox(nullptr, L"Не допустимый номер страницы", L"Error", MB_OK);
		fz_drop_document(m_ctx, m_doc);
		fz_drop_context(m_ctx);
		return EXIT_FAILURE;
	}

	/* Compute a transformation matrix for the zoom and rotation desired. */
	/* The default resolution without scaling is 72 dpi. */
	m_ctm = fz_scale(m_zoom / 100, m_zoom / 100);
	m_ctm = fz_pre_rotate(m_ctm, m_rotate);

	/* Render page to an RGB pixmap. */
	fz_try(m_ctx)
		m_pix = fz_new_pixmap_from_page_number(m_ctx, m_doc, page_num, m_ctm, fz_device_rgb(m_ctx), NULL);
	fz_catch(m_ctx)
	{
		MessageBox(nullptr, L"Не удалось запомнить страницу", L"Error", MB_OK);
		fz_drop_document(m_ctx, m_doc);
		fz_drop_context(m_ctx);
		return EXIT_FAILURE;
	}

	return true;
}

float PdfReader::aColTofCol(unsigned char color)
{
	int value = color;
	return (float)value * C_BTOF;
}

float *PdfReader::GetFloatPage()
{
	int width = 0;
	int height = 0;
	int c = 0;

	width = m_pix->w;
	height = m_pix->h;

	float *page = new float[width * height * 4];

	for (int y = 0; y < height; ++y)
	{
		unsigned char *p = &m_pix->samples[y * m_pix->stride];
		for (int x = 0; x < width; ++x)
		{
			page[c] = aColTofCol(p[0]);
			page[c + 1] = aColTofCol(p[1]);
			page[c + 2] = aColTofCol(p[2]);
			page[c + 3] = 1.0f;
			c += 4;
			p += m_pix->n;
		}
	}

	return page;
}

/*void PdfReader::pixmapToDocForRender()
{
	int width = 0;
	int height = 0;
	int c = 0;

	m_pagesRGB = new Page[m_page_count + 7];
	int pix_count = 0;

	for (int i = 3; i < m_page_count + 3; ++i)
	{
		m_pagesRGB[i].width = m_pix[pix_count]->w;
		m_pagesRGB[i].height = m_pix[pix_count]->h;
		width = m_pix[pix_count]->w;
		height = m_pix[pix_count]->h;

		m_pagesRGB[i].RGB = new float[width * height * 4];


		c = 0;

		for (int y = 0; y < height; ++y)
		{
			unsigned char *p = &m_pix[pix_count]->samples[y * m_pix[pix_count]->stride];
			for (int x = 0; x < width; ++x)
			{
				m_pagesRGB[i].RGB[c] = aColTofCol(p[0]);
				m_pagesRGB[i].RGB[c + 1] = aColTofCol(p[1]);
				m_pagesRGB[i].RGB[c + 2] = aColTofCol(p[2]);
				m_pagesRGB[i].RGB[c + 3] = 1.0f;
				c += 4;
				p += m_pix[pix_count]->n;
			}
		}
		pix_count++;
	}
	m_pagesRGB[0] = m_pagesRGB[3];
	m_pagesRGB[1] = m_pagesRGB[3];
	m_pagesRGB[2] = m_pagesRGB[3];

	for(int i = 3; i < 7; i++)
		m_pagesRGB[m_page_count + i] = m_pagesRGB[m_page_count + 1];

}*/
