#include "stdafx.h"
#include "GDC.h"

#include "msw/MswGDC.h"
#include "msw/MswBitmap.h"
#include "svg/svgGDC.h"
#include "AbsPaint.h"

#ifdef _DEBUG
    #define new DEBUG_NEW
#endif

GDCFontDescr::GDCFontDescr(const wchar_t *sFontName, GDCFontWeight weight, int32_t nHeight, int32_t nSlant, int32_t nUnderline) {
    m_sFontName  = sFontName;
    m_weight     = weight;
    m_nHeight    = nHeight;
    m_nSlant     = nSlant;
	m_nUnderline = nUnderline;
    m_nTextAlign = GDC_TA_LEFT;
}

GDCFontDescr::GDCFontDescr(const GDCFontDescr &font_descr) {
    m_fAngle     = font_descr.m_fAngle;
    m_sFontName  = font_descr.m_sFontName;
    m_weight     = font_descr.m_weight;
    m_nHeight    = font_descr.m_nHeight;
    m_nSlant     = font_descr.m_nSlant;
	m_nUnderline = font_descr.m_nUnderline;
    m_nTextAlign = font_descr.m_nTextAlign;
}

GDCFontDescr &GDCFontDescr::operator=(const GDCFontDescr &x)
{
    m_fAngle     = x.m_fAngle;
    m_sFontName  = x.m_sFontName;
    m_weight     = x.m_weight;
    m_nHeight    = x.m_nHeight;
    m_nSlant     = x.m_nSlant;
	m_nUnderline = x.m_nUnderline;
    m_nTextAlign = x.m_nTextAlign;
    return *this;
}

GDCPaint::GDCPaint()
{
    
}

GDCPaint::GDCPaint(const wchar_t *sFontName, GDCFontWeight weight, int32_t nHeight, int32_t nSlant, int32_t nUnderline)
{
    m_pFontDescr  = new GDCFontDescr(sFontName, weight, nHeight, nSlant, nUnderline);
}

GDCPaint::GDCPaint(const GDCFontDescr &font_descr)
{
	m_pFontDescr  = new GDCFontDescr(font_descr);
}

GDCPaint::~GDCPaint()
{
    delete m_pPaint;
    delete m_pFontDescr;
}

void GDCPaint::SetAlfa(int32_t nAlfa)
{
    m_nAlfa = nAlfa;
    //m_pPaint->SetAlfa(nAlfa);
}

void GDCPaint::SetColor(COLORREF color)
{
    m_color = color;
    //m_pPaint->SetColor(color);
}

void GDCPaint::SetBkColor(COLORREF color)
{
    m_bk_color = color;
}

void GDCPaint::SetStrokeWidth(float fWidth)
{
    m_fWidth = fWidth;
    //m_pPaint->SetStrokeWidth(dWidth);
}

void GDCPaint::SetStrokeType(GDCStrokeType type)
{
    m_stroke_type = type;
    //m_pPaint->SetStrokeType(type);
}

void GDCPaint::SetPaintType(GDCPaintType type)
{
    m_paint_type = type;
    //m_pPaint->SetPaintType(type);
}

void GDCPaint::SetBkMode(GDCBackgroundMode mode)
{
    m_bk_mode = mode;
}

void GDCPaint::SetRasterType(GDCRasterType type)
{
    m_raster_type = type;
}

void GDCPaint::SetTextAlign(int32_t nAlign)
{
    if ( !m_pFontDescr ) {
        m_pFontDescr = new GDCFontDescr(L"Arial", GDC_FW_NORMAL, 12, 0, 0);
    }
    m_pFontDescr->m_nTextAlign = nAlign;
    //m_pPaint->SetTextAlign(nAlign);
}

void GDCPaint::SetTextOrientation(float fAngle)
{
    if ( !m_pFontDescr ) {
        m_pFontDescr = new GDCFontDescr(L"Arial", GDC_FW_NORMAL, 12, 0, 0);
        m_pFontDescr->m_fAngle = fAngle;
    }

    if ( m_pPaint ) {
        m_pPaint->SetTextOrientation(fAngle);
    }
    m_pFontDescr->m_fAngle = fAngle;
}

void GDCPaint::SetTextSize(int32_t nSize)
{
    if ( !m_pFontDescr ) {
        m_pFontDescr = new GDCFontDescr(L"Arial", GDC_FW_NORMAL, nSize, 0, 0);
    }
    if ( m_pPaint ) {
        m_pPaint->SetTextSize(nSize);
    }
	m_pFontDescr->m_nHeight = nSize;
}

GDCPaintType GDCPaint::GetPaintType() const { return m_paint_type; }
GDCBackgroundMode GDCPaint::GetBkMode() const { return m_bk_mode; }
int32_t GDCPaint::GetAlfa() const   { return m_nAlfa; }
COLORREF GDCPaint::GetColor() const { return m_color; }
COLORREF GDCPaint::GetBkColor() const { return m_bk_color; }

GDC::GDC(HDC hDC)
{
    m_pDC = new CMswGDC(hDC);
}

GDC::~GDC()
{
    delete m_pDC;
}

void GDC::DrawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const GDCPaint &paint)
{
    m_pDC->DrawLine(x1, y1, x2, y2, paint);
}

void GDC::DrawPolygon(const std::vector<GDCPoint> &points, const GDCPaint &fill_paint, const GDCPaint &stroke_paint)
{
    m_pDC->DrawPolygon(points, fill_paint, stroke_paint);
}

void GDC::DrawPoly(const std::vector<GDCPoint> &points, const GDCPaint &stroke_paint)
{
    const size_t cnt = points.size();
    if ( cnt < 2 ) {
        return; // expected at least 3 points
    }
    m_pDC->DrawPoly(points, stroke_paint);
}

void GDC::DrawPolygonTransparent(const std::vector<GDCPoint> &points, const GDCPaint &fill_paint)
{
    m_pDC->DrawPolygonTransparent(points, fill_paint);
}

void GDC::DrawPolygonGradient(const std::vector<GDCPoint> &points, const GDCPaint &paintFrom, const GDCPaint &paintTo)
{
    m_pDC->DrawPolygonGradient(points, paintFrom, paintTo);
}

void GDC::DrawPolygonTexture(const std::vector<GDCPoint> &points, const wchar_t * sTexturePath, double dAngle, float fZoom)
{
    m_pDC->DrawPolygonTexture(points, sTexturePath, dAngle, fZoom);
}

void GDC::DrawPolygonTexture(const std::vector<GDCPoint> &points, const std::vector<GDCPoint> &points_exclude, const wchar_t * sTexturePath, double dAngle, float fZoom)
{
    m_pDC->DrawPolygonTexture(points, points_exclude, sTexturePath, dAngle, fZoom);
}

void GDC::DrawPolyLine(const std::vector<GDCPoint> &points, const GDCPaint &stroke_paint)
{
    m_pDC->DrawPolyLine(points, stroke_paint);
}

void GDC::DrawPoint(int32_t x, int32_t y, const GDCPaint &paint)
{
    m_pDC->DrawPoint(x, y, paint);
}

void GDC::DrawFilledRectangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const GDCPaint &fill_paint)
{
    m_pDC->DrawFilledRectangle(x1, y1, x2, y2, fill_paint);
}

void GDC::DrawRectangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const GDCPaint &stroke_paint)
{
    m_pDC->DrawRectangle(x1, y1, x2, y2, stroke_paint);
}

void GDC::DrawEllipse(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const GDCPaint &paint)
{
    m_pDC->DrawEllipse(x1, y1, x2, y2, paint);
}

void GDC::DrawFilledEllipse(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const GDCPaint &paint)
{
	m_pDC->DrawFilledEllipse(x1, y1, x2, y2, paint);
}

void GDC::DrawHollowOval(int32_t xCenter, int32_t yCenter, int32_t rx, int32_t ry, int32_t h, const GDCPaint &fill_paint)
{
	m_pDC->DrawHollowOval(xCenter, yCenter, rx, ry, h, fill_paint);
}

void GDC::DrawArc(int32_t x, int32_t y, const int32_t nRadius, const float fStartAngle, const float fSweepAngle, const GDCPaint &paint)
{
    m_pDC->DrawArc(x, y, nRadius, fStartAngle, fSweepAngle, paint);
}

void GDC::TextOut(const wchar_t *sText, int32_t x, int32_t y, const GDCPaint &paint)
{
    m_pDC->TextOut(sText, x, y, paint);
}

void GDC::TextOutRect(const wchar_t *sText, const RECT &rect, const GDCPaint &paint)
{
    const GDCSize sz = GetTextExtent(sText, ::wcslen(sText), paint);
    const int32_t center_x = int32_t((rect.left   + rect.right)*0.5);
    const int32_t center_y = int32_t((rect.bottom + rect.top)*0.5);
    const int32_t half_cx = int32_t(sz.cx * 0.5);
    const int32_t half_cy = int32_t(sz.cy * 0.5);
    const int32_t pos_x = center_x - half_cx;
    const int32_t pos_y = center_y - half_cy;
    
    m_pDC->TextOut(sText, pos_x, pos_y, paint);
}

void GDC::DrawText(const wchar_t *sText, const RECT &rect, const GDCPaint &paint)
{
    m_pDC->DrawText(sText, rect, paint);
}

void GDC::DrawTextByEllipse(double dCenterAngle, int32_t nRadiusX, int32_t nRadiusY, int32_t xCenter, int32_t yCenter, const wchar_t *sText, bool bAllignBottom, double dEllipseAngleRad, const GDCPaint &paint)
{
    m_pDC->DrawTextByEllipse(dCenterAngle, nRadiusX, nRadiusY, xCenter, yCenter, sText, bAllignBottom, dEllipseAngleRad, paint);
}

void GDC::DrawTextByCircle(double dCenterAngle, int32_t nRadius, int32_t nCX, int32_t nCY, const wchar_t *sText, bool bAllignBottom, bool bRevertTextDir, const GDCPaint &paint)
{
    m_pDC->DrawTextByCircle(dCenterAngle, nRadius, nCX, nCY, sText, bAllignBottom, bRevertTextDir, paint);
}

int32_t GDC::GetTextHeight(const GDCPaint &paint) const
{
    return m_pDC->GetTextHeight(paint);
}

GDCSize GDC::GetTextExtent(const wchar_t *sText, size_t nCount, const GDCPaint &paint) const
{
    return m_pDC->GetTextExtent(sText, nCount, paint);
}

void GDC::SetViewportOrg(int32_t x, int32_t y)
{
    m_pDC->SetViewportOrg(x, y);
}

GDCPoint GDC::GetViewportOrg() const
{
    return m_pDC->GetViewportOrg();
}

void GDC::DrawBitmap(const GDCBitmap &bitmap, int32_t x, int32_t y)
{
    m_pDC->DrawBitmap(bitmap.GetHBITMAP(), x, y);
}

HDC GDC::GetHDC()
{
    return m_pDC->GetHDC();
}

void GDC::BeginGroup(const char *sGroupAttributes)
{
    m_pDC->BeginGroup(sGroupAttributes);
}

void GDC::EndGroup()
{
    m_pDC->EndGroup();
}

GDCBitmap::GDCBitmap(int32_t width, int32_t height)
{
    m_pBitmap = new CMswBitmap(width, height);
}

GDCBitmap::~GDCBitmap()
{
    delete m_pBitmap;
}

GDCBitmap::GDCBitmap(HBITMAP hBitmap)
{
    m_pBitmap = new CMswBitmap(hBitmap);
}

HBITMAP GDCBitmap::GetHBITMAP() const
{
    return m_pBitmap->GetHBITMAP();
}

int32_t GDCBitmap::Width() const
{
    return m_pBitmap->Width();
}

int32_t GDCBitmap::Height() const
{
    return m_pBitmap->Height();
}

GDC::GDC(GDCBitmap &bitmap, COLORREF background /* = RGB(255, 255, 255) */)
{
    m_pDC = new CMswGDC(bitmap, background);
}

GDC::GDC(HWND hwnd)
{
	m_pDC = new CMswGDC(hwnd);
}

GDC::GDC(GDCSvg &svg)
{
    if ( svg.m_pBuffer ) {
        m_pDC = new SvgGDC(svg.m_pBuffer, svg.Width(), svg.Height(), svg.m_bAutoSize, svg.m_sPrefix.c_str());
    }
    else {
        m_pDC = new SvgGDC(svg.GetFilePath(), svg.Width(), svg.Height(), svg.m_bAutoSize, svg.m_sPrefix.c_str());
    }
}

GDCSvg::GDCSvg(std::string *pBuffer, int32_t width, int32_t height, bool bAutoSize)
{
    m_pBuffer   = pBuffer;
    m_nWidth    = width;
    m_nHeight   = height;
    m_bAutoSize = bAutoSize;
}

GDCSvg::GDCSvg(const wchar_t *sFilePath, int32_t width, int32_t height, bool bAutoSize)
{
    m_pBuffer   = nullptr;
    m_sFilePath = sFilePath;
    m_nWidth    = width;
    m_nHeight   = height;
    m_bAutoSize = bAutoSize;
}

GDCSvg::~GDCSvg()
{
    
}

void GDCSvg::SetPrefix(const char *sPrefix)
{
    m_sPrefix = sPrefix;
}