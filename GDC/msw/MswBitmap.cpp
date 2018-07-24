#include "stdafx.h"
#include "MswBitmap.h"

#include "../GDI/oligdi.h"

#ifdef _DEBUG
    #define new DEBUG_NEW
#endif

CMswBitmap::CMswBitmap(int32_t width, int32_t height)
: m_width(width), m_height(height)
{
    m_pBitmap = new OBitmap();
    OWindowDC dc(nullptr); // screen
    m_pBitmap->CreateCompatibleBitmap(&dc, width, height);
}

CMswBitmap::CMswBitmap(HBITMAP hBitmap)
{
    m_pBitmap = new OBitmap(hBitmap);
    //BITMAP bm;
	//m_pBitmap->GetObject(bm);
	//m_width  = bm.bmWidth;
	//m_height = bm.bmHeight;
}

CMswBitmap::~CMswBitmap()
{
    delete m_pBitmap;
}

HBITMAP CMswBitmap::GetHBITMAP() const
{
    return (HBITMAP)(*m_pBitmap);
}

int32_t CMswBitmap::Width() const 
{
    return m_width;
}

int32_t CMswBitmap::Height() const
{
    return m_height;
}