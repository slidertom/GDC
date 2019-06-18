#include "stdafx.h"
#include "MswPaint.h"

#include "../GDI/oligdi.h"
#include "../GDC.h"

#ifdef _DEBUG
    #define new DEBUG_NEW
#endif

static inline LOGFONT *CreateDefLogFont()
{
    LOGFONT *pLF = new LOGFONT;
    ::memset(pLF, 0, sizeof(LOGFONT));
	::wcscpy(pLF->lfFaceName, L"Arial");
    pLF->lfWeight = FW_NORMAL;
    return pLF;
}

CMswPaint::CMswPaint(const GDCFontDescr *pFontDescr)
{
    m_pLF = new LOGFONT;
    ::memset(m_pLF, 0, sizeof(LOGFONT));

	::wcscpy(m_pLF->lfFaceName, pFontDescr->m_sFontName.c_str());
	m_pLF->lfHeight      = pFontDescr->m_nHeight;
	m_pLF->lfWeight      = pFontDescr->m_weight;
    m_pLF->lfItalic      = pFontDescr->m_nSlant;
	m_pLF->lfUnderline   = pFontDescr->m_nUnderline;
    m_pLF->lfOrientation = m_pLF->lfEscapement = (LONG)pFontDescr->m_fAngle;
}

CMswPaint::~CMswPaint()
{
    delete m_pPen;
    delete m_pBrush;
    delete m_pLF;
    delete m_pFont;
}

void CMswPaint::SetAlfa(int32_t nAlfa)
{
   // TODO delete current objects
}

void CMswPaint::SetColor(COLORREF color)
{
    // TODO delete current objects
}

void CMswPaint::SetStrokeWidth(double dWidth)
{
    // TODO delete current objects
}

void CMswPaint::SetStrokeType(GDCStrokeType type)
{
    // TODO delete current objects
}

void CMswPaint::SetPaintType(GDCPaintType type)
{
    // TODO delete current objects
}

void CMswPaint::SetTextAlign(int32_t nAlign)
{
    // TODO delete current objects
}

void CMswPaint::SetTextOrientation(double dAngle)
{
    if ( !m_pLF ) {
        m_pLF = CreateDefLogFont();
    }
    else {
        if ( m_pLF->lfOrientation != (LONG)dAngle ) {
            delete m_pFont;
            m_pFont = nullptr;
        }
    }
    m_pLF->lfOrientation = m_pLF->lfEscapement = (LONG)dAngle;
}

void CMswPaint::SetTextSize(int32_t nSize)
{
    if ( !m_pLF ) {
        m_pLF = CreateDefLogFont();
    }
    else  {
        if ( m_pLF->lfHeight != nSize ) {
            delete m_pFont;
            m_pFont = nullptr;
        }
    }

    m_pLF->lfHeight = nSize;
}

