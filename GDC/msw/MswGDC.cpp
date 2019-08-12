#include "stdafx.h"
#include "MswGDC.h"

#include "../GDI/oligdi.h"
#include "../GDC.h" 

#include "MswBitmap.h"
#include "MswPaint.h"

#include "gdi_plus_util.h"
#include "TextUtils/GdiPlusTextDrawUtils.h"

#include "../../GDC/msw/gdi_plus_inc//GdiPlus.h"

#include "memory"

#ifdef _DEBUG
    #define new DEBUG_NEW
#endif

namespace internal
{
    static inline LOGFONT *CreateDefLogFont()
    {
        LOGFONT *pLF = new LOGFONT;
        memset(pLF, 0, sizeof(LOGFONT));
        ::wcscpy(pLF->lfFaceName, L"Arial");
        pLF->lfWeight = FW_NORMAL;
        return pLF;
    }

    static Gdiplus::Point *GdcPoly2GdiPlus(const std::vector<GDCPoint> &mx_poly)
    {
        const size_t nCnt = mx_poly.size();
        ASSERT(nCnt > 0);
        Gdiplus::Point *points = new Gdiplus::Point[nCnt];
        for (size_t i1 = 0; i1 < nCnt; ++i1) {
            points[i1].X = mx_poly[i1].x;
            points[i1].Y = mx_poly[i1].y;
        }

        return points;
    }
};

class ODCInit
{
    static CMswPaint *GetMswPaint(const GDCPaint &fill_paint)
    {
        CMswPaint *pPaint = nullptr;
        if ( !fill_paint.m_pPaint ) {
            pPaint = new CMswPaint;
            fill_paint.m_pPaint = pPaint;
        }
        else {
            pPaint = (CMswPaint *)fill_paint.m_pPaint;
        }
        return pPaint;
    }

// Static operations
public:
    static void SelectStrokePaint(ODC *pDC, const GDCPaint &stroke_paint)
    {
        CMswPaint *pPaint = GetMswPaint(stroke_paint);

        ASSERT(stroke_paint.m_paint_type == GDC_STROKE);
        if ( !pPaint->m_pPen ) {
            pPaint->m_pPen = new OPen;  
            int32_t nStyle = PS_SOLID;
            switch (stroke_paint.m_stroke_type)
            {
            case GDC_PS_SOLID:
                nStyle = PS_SOLID;
            break;
            case GDC_PS_DASH:
                nStyle = PS_DASH;
            break;
            case GDC_PS_DOT:
                nStyle = PS_DOT;
            break;
            case GDC_PS_DASHDOT:
                nStyle = PS_DASHDOT;
            break;
            case GDC_PS_DASHDOTDOT:
                nStyle = PS_DASHDOTDOT;
            break;
            default:
                ASSERT(FALSE); // unsuported type
            break;
            }
            pPaint->m_pPen->CreatePen(nStyle, (int32_t)stroke_paint.m_fWidth, stroke_paint.m_color);
        } 

        if ( stroke_paint.m_stroke_type != GDC_PS_SOLID ) {
            pDC->SetBkMode(TRANSPARENT);
        }
        else if ( pDC->GetBkMode() != OPAQUE ) {
            //pDC->SetBkMode(OPAQUE); // do we need this?
        }
        pDC->SelectObject(pPaint->m_pPen);
    }

    static OBrush *SelectFillPaint(ODC *pDC, const GDCPaint &fill_paint)
    {
        CMswPaint *pPaint = GetMswPaint(fill_paint);
        
        GDCPaintType paint_type = fill_paint.GetPaintType();
        ASSERT(paint_type >= GDC_FILL);
        switch (paint_type)
        {
        case GDC_FILL:
            {
                if ( !pPaint->m_pBrush ) {
                    pPaint->m_pBrush = new OBrush(fill_paint.m_color);
                }
            }
            break;
        case GDC_FILL_DIAGCROSS:
            {
                if ( !pPaint->m_pBrush ) {
                    pPaint->m_pBrush = new OBrush(fill_paint.m_color, HS_DIAGCROSS);
                }
                pDC->SetBkMode(TRANSPARENT);
                //pDC->SelectStockObject(HOLLOW_BRUSH);
            }
            break;
        case GDC_FILL_VERTICAL:
            {
                if (!pPaint->m_pBrush) {
                    pPaint->m_pBrush = new OBrush(fill_paint.m_color, HS_VERTICAL);
                }
                pDC->SetBkMode(TRANSPARENT);
            }
            break;
        case GDC_FILL_HORIZONTAL:
            {
                if (!pPaint->m_pBrush) {
                    pPaint->m_pBrush = new OBrush(fill_paint.m_color, HS_HORIZONTAL);
                }
                pDC->SetBkMode(TRANSPARENT);
            }
            break;
        case GDC_FILL_FDIAGONAL:
            {
                if (!pPaint->m_pBrush) {
                    pPaint->m_pBrush = new OBrush(fill_paint.m_color, HS_FDIAGONAL);
                }
                pDC->SetBkMode(TRANSPARENT);
            }
            break;
        case GDC_FILL_BDIAGONAL:
            {
                if (!pPaint->m_pBrush) {
                    pPaint->m_pBrush = new OBrush(fill_paint.m_color, HS_BDIAGONAL);
                }
                pDC->SetBkMode(TRANSPARENT);
            }
            break;
        case GDC_FILL_CROSS:
            {
                if (!pPaint->m_pBrush) {
                    pPaint->m_pBrush = new OBrush(fill_paint.m_color, HS_CROSS);
                }
                pDC->SetBkMode(TRANSPARENT);
            }
            break;
        default:
            ASSERT(FALSE);
            return nullptr;
            break;
        }
        pDC->SelectObject(pPaint->m_pBrush);

        ASSERT(pPaint->m_pBrush);

        return pPaint->m_pBrush;
    }

    static void SelectFont(ODC *pDC, const GDCPaint &paint)
    {
        CMswPaint *pPaint = nullptr;
        if ( !paint.m_pPaint ) {
            pPaint = new CMswPaint(paint.m_pFontDescr);
            paint.m_pPaint = pPaint;
        }
        else {
            pPaint = (CMswPaint *)paint.m_pPaint;
        }

        if ( !pPaint->m_pFont ) {
            pPaint->m_pFont = new OFont;
            if ( !pPaint->m_pLF ) {
                pPaint->m_pLF = internal::CreateDefLogFont();
            }
            VERIFY(pPaint->m_pFont->CreateFontIndirect(pPaint->m_pLF));
        }
        pDC->SelectObject(*pPaint->m_pFont);

        int32_t nAllign(0);

        // Transfer Horizontal
        if (paint.m_pFontDescr->m_nTextAlign & GDC_TA_LEFT) {
            nAllign |= TA_LEFT;
        }
        if (paint.m_pFontDescr->m_nTextAlign & GDC_TA_RIGHT ) {
            nAllign |= TA_RIGHT;
        }
        if ( paint.m_pFontDescr->m_nTextAlign & GDC_TA_CENTER) {
            nAllign |= TA_CENTER;
        }

        // Transfer Vertical
        if (paint.m_pFontDescr->m_nTextAlign & GDC_TA_BOTTOM) {
            nAllign |= TA_BOTTOM;
        }
        if (paint.m_pFontDescr->m_nTextAlign & GDC_TA_BASELINE) {
            nAllign |= TA_BASELINE;
        }
        if (paint.m_pFontDescr->m_nTextAlign & GDC_TA_TOP) {
            nAllign |= TA_TOP;
        }

        pDC->SetTextAlign(nAllign);
        if (paint.m_bk_mode == GDC_TRANSPARENT) {
            pDC->SetBkMode(TRANSPARENT);
        } else {
            pDC->SetBkColor(paint.m_bk_color);
            pDC->SetBkMode(OPAQUE);
        }
        pDC->SetTextColor(paint.m_color);
    }

// Binary raster class
public:
    class CBinaryRaster
    {
    public:
        CBinaryRaster(ODC *pDC, const GDCPaint &paint)
        :m_pDC(pDC)
        {
            switch (paint.GetRasterType())
            {
                case GDC_R2_NOTXORPEN:
                    m_nOldDrawMode = pDC->SetROP2(R2_NOTXORPEN);
                    break;
                case GDC_R2_XORPEN:
                    m_nOldDrawMode = pDC->SetROP2(R2_XORPEN);
                    break;
                case GDC_R2_NONE:
                default:
                    break;
            }
        }

        ~CBinaryRaster()
        {
            if (m_nOldDrawMode != -1) {
                m_pDC->SetROP2(m_nOldDrawMode);
            }
        }

    private:
        int32_t m_nOldDrawMode {-1};
        ODC *m_pDC;
    };

};

namespace ODCToGDCPlusUtils
{
    static void DrawLineWithGDCPlus(HDC hDC, int32_t x1, int32_t y1, int32_t x2, int32_t y2, GDCStrokeType eStrokeType, double dWidth, COLORREF color)
    {
        switch (eStrokeType)
        {
        case GDC_PS_DASH:
        {
            CGdiPlusUtil::DrawLineDash(hDC, x1, y1, x2, y2,
                                       (unsigned char)GetRValue(color), (unsigned char)GetGValue(color), (unsigned char)GetBValue(color),
                                       dWidth);
            return;
        }
        break;
        case GDC_PS_DOT:
        {
            CGdiPlusUtil::DrawLineDot(hDC, x1, y1, x2, y2,
                                     (unsigned char)GetRValue(color), (unsigned char)GetGValue(color), (unsigned char)GetBValue(color),
                                     dWidth);
            return;
        }
        break;
        case GDC_PS_DASHDOT:
        {
            CGdiPlusUtil::DrawLineDashDot(hDC, x1, y1, x2, y2,
                                         (unsigned char)GetRValue(color), (unsigned char)GetGValue(color), (unsigned char)GetBValue(color),
                                          dWidth);
            return;
        }
        case GDC_PS_DASHDOTDOT:
        {
            CGdiPlusUtil::DrawLineDashDotDot(hDC, x1, y1, x2, y2,
                                             (unsigned char)GetRValue(color), (unsigned char)GetGValue(color), (unsigned char)GetBValue(color),
                                             dWidth);
            return;
        }
        break;
        default:
            // Solid line must be drawn with ODC.
            ASSERT(false);
            break;
        }
    }
}

CMswGDC::CMswGDC(HDC hDC)
{
    m_pDC = new ODC(hDC);
}

CMswGDC::~CMswGDC()
{
    delete m_pDC;
}

// Speed comparison off GDI+ vs. CDC RELEASE		
//			        1px	    3px
//		CDC|DASH	0.17	0.289	
//		CDC|SOLID	0.127	0.29	
//		GDI+|DASH	1.24	1.73	Much denser pattern than CDC
//		GDI+|SOLID	0.78	1.08	
// CDC is faster than GDO+:			
//			         1px	3px	
//		CDC|DASH	7.29	5.99	
//		CDC|SOLID	6.14	3.72	
// Test Code:
//    for (int i = 0; i < 10000; ++i) {
//        Gdiplus::Pen pen(Gdiplus::Color(255, 0, 0), 3.0);
//        pen.SetDashStyle(Gdiplus::DashStyleSolid);
//        dc.DrawLine(&pen, pt1.x, pt1.y, pt2.x, pt2.y);
//    }
//
//    for (int i = 0; i < 10000; ++i) {
//        CPen pen(PS_SOLID, 3, RGB(255, 0, 0));
//        CPen *pOld = pDC->SelectObject(&pen);
//          pDC->MoveTo(pt1);
//          pDC->LineTo(pt2);
//        pDC->SelectObject(pOld);
//    }
void CMswGDC::DrawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const GDCPaint &paint)
{
    const double dWidth = paint.GetStrokeWidth();
    const GDCStrokeType eStrokeType = paint.GetStrokeType();

    if (GDC_PS_SOLID != eStrokeType && dWidth > 1.0) {
        // Draw non solid line with width more than 1.0 by using GDI+
        // Because DC draws that type of lines incorrectly.
        COLORREF color = paint.GetColor();
        ODCToGDCPlusUtils::DrawLineWithGDCPlus(m_pDC->GetSafeHdc(), x1, y1, x2, y2, eStrokeType, dWidth, color);
        return;
    }

    ODCInit::SelectStrokePaint(m_pDC, paint);
    ODCInit::CBinaryRaster rop2(m_pDC, paint);

    m_pDC->MoveTo(x1, y1);
    m_pDC->LineTo(x2, y2);
}

void CMswGDC::DrawPolygon(const std::vector<GDCPoint> &points, const GDCPaint &fill_paint, const GDCPaint &stroke_paint)
{
    ODCInit::SelectStrokePaint(m_pDC, stroke_paint);
    ODCInit::SelectFillPaint(m_pDC, fill_paint);
    ODCInit::CBinaryRaster rop2(m_pDC, fill_paint);

    int32_t nCnt = (int32_t)points.size();
    POINT *pPoints = new POINT[points.size()];
    int32_t i1 = 0;

    for (const auto &pt : points) {
        POINT &point = pPoints[i1];
        point.x = pt.x;
        point.y = pt.y;
        ++i1;
    }
    m_pDC->Polygon(pPoints, nCnt);
    delete [] pPoints;
}

void CMswGDC::DrawPoly(const std::vector<GDCPoint> &points, const GDCPaint &stroke_paint)
{
    const size_t cnt = points.size();

    ODCInit::SelectStrokePaint(m_pDC, stroke_paint);
    ODCInit::CBinaryRaster rop2(m_pDC, stroke_paint);

    const GDCPoint &pt = points.front();
    m_pDC->MoveTo(pt);
    for (auto i1 = 0; i1 < cnt; ++i1) {
        m_pDC->LineTo(points[i1]);	
    }
    m_pDC->LineTo(pt); // do close 
}

void CMswGDC::DrawPolygonTransparent(const std::vector<GDCPoint> &points, const GDCPaint &fill_paint)
{
    ASSERT(fill_paint.GetAlfa() != -1);
    unsigned char alfa = (unsigned char)fill_paint.GetAlfa();
    COLORREF color = fill_paint.GetColor();
    std::vector<POINT> points2;
    points2.reserve(points.size());
    POINT point;
    for (const GDCPoint &pt : points) {
        point.x = pt.x;
        point.y = pt.y;
        points2.push_back(point);
    }

    CGdiPlusUtil::DrawPolygonTransparent(m_pDC->GetSafeHdc(), points2, alfa, 
                                         (unsigned char)GetRValue(color), (unsigned char)GetGValue(color), (unsigned char)GetBValue(color)); 

    // does not work as HWND required!!! TODO: wxwidgets must be asked -> HDC should be allowed
    /*
    wxTempDC -> wxMemoryDC -> SelectBitmap? CreateContext
    std::unique_ptr<wxGraphicsContext> gc(wxGraphicsContext::CreateFromNativeWindow(m_pDC->GetSafeHdc()));
    //std::unique_ptr<wxGraphicsContext> gc(wxGraphicsContext::CreateFromNative(m_pDC->GetSafeHdc()));
    
    wxGraphicsPath path = gc->CreatePath();
    GDCPoint pt0 = points[0];
    path.MoveToPoint(pt0.x, pt0.y);
    const size_t nCnt = points.size();
    for (size_t i1 = 1; i1 < nCnt; ++i1) {
        GDCPoint pt = points[i1];
        path.AddLineToPoint(pt.x, pt.y);
    }
    path.CloseSubpath();
    gc->SetBrush(wxColor((unsigned char)GetRValue(color), (unsigned char)GetGValue(color), (unsigned char)GetBValue(color), alfa));
    gc->FillPath(path);
    */
}

void CMswGDC::DrawPolygonGradient(const std::vector<GDCPoint> &points, const GDCPaint &paintFrom, const GDCPaint &paintTo)
{
    COLORREF colorFrom = paintFrom.GetColor();
    COLORREF colorTo   = paintTo.GetColor();

    std::vector<POINT> points2;
    points2.reserve(points.size());
    POINT point;
    for (const GDCPoint &pt : points) {
        point.x = pt.x;
        point.y = pt.y;
        points2.push_back(point);
    }

    CGdiPlusUtil::DrawPolygon(m_pDC->GetSafeHdc(), points2, 
                             (unsigned char)GetRValue(colorFrom), (unsigned char)GetGValue(colorFrom), (unsigned char)GetBValue(colorFrom), 
                             (unsigned char)GetRValue(colorTo),   (unsigned char)GetGValue(colorTo),   (unsigned char)GetBValue(colorTo));
}

void CMswGDC::DrawPolygonTexture(const std::vector<GDCPoint> &points, const wchar_t *sTexturePath, double dAngle, float fZoom)
{
    const BOOL useEmbeddedColorManagement = FALSE;
    std::unique_ptr<Gdiplus::Image> pImage(Gdiplus::Image::FromFile(sTexturePath, useEmbeddedColorManagement));
    if (pImage && pImage->GetLastStatus() != Gdiplus::Ok) {
        return;
    }

    const Gdiplus::Point *gdi_points = internal::GdcPoly2GdiPlus(points);

    Gdiplus::TextureBrush brush(pImage.get(), Gdiplus::WrapModeTile);
    brush.ScaleTransform(fZoom, fZoom);
    brush.RotateTransform((float) dAngle);
    brush.TranslateTransform((float) gdi_points[0].X, (float) gdi_points[0].Y, Gdiplus::MatrixOrderAppend);

    const size_t nCnt = points.size();
    Gdiplus::Graphics dc(GetHDC());
    dc.FillPolygon(&brush, gdi_points, (int32_t)nCnt); 

    delete[] gdi_points;
}

void CMswGDC::DrawPolygonTexture(const std::vector<GDCPoint> &points, const std::vector<GDCPoint> &points_exclude, 
                                 const wchar_t *sTexturePath, double dAngle, float fZoom)
{
    const BOOL useEmbeddedColorManagement = FALSE;
    std::unique_ptr<Gdiplus::Image> pImage(Gdiplus::Image::FromFile(sTexturePath, useEmbeddedColorManagement));
    if (pImage && pImage->GetLastStatus() != Gdiplus::Ok) {
        return;
    }

    const Gdiplus::Point *gdi_points_include = internal::GdcPoly2GdiPlus(points);
    const Gdiplus::Point *gdi_points_exclude = internal::GdcPoly2GdiPlus(points_exclude);

    Gdiplus::TextureBrush brush(pImage.get(), Gdiplus::WrapModeTile);
    brush.ScaleTransform(fZoom, fZoom);
    brush.RotateTransform((float)dAngle);
    brush.TranslateTransform((float)gdi_points_include[0].X, (float)gdi_points_include[0].Y, Gdiplus::MatrixOrderAppend);

    Gdiplus::GraphicsPath path_include, path_exclude;
    path_include.AddPolygon(gdi_points_include, (int32_t)points.size());
    path_exclude.AddPolygon(gdi_points_exclude, (int32_t)points.size());

    Gdiplus::Region region(&path_include);
    region.Exclude(&path_exclude);

    Gdiplus::Graphics dc(GetHDC());
    dc.FillRegion(&brush, &region);

    delete[] gdi_points_include;
    delete[] gdi_points_exclude;
}

void CMswGDC::DrawPolyLine(const std::vector<GDCPoint> &points, const GDCPaint &stroke_paint)
{
    ODCInit::SelectStrokePaint(m_pDC, stroke_paint);
    ODCInit::CBinaryRaster rop2(m_pDC, stroke_paint);

    const GDCPoint &pt = points.front();
    m_pDC->MoveTo(pt);
    const size_t cnt = points.size();
    for (auto i1 = 0; i1 < cnt; ++i1) {
        m_pDC->LineTo(points[i1]);	
    }
}

void CMswGDC::DrawPoint(int32_t x, int32_t y, const GDCPaint &paint)
{
    // not correct implementation -> Ellipse has to be used(!)
    ODCInit::SelectStrokePaint(m_pDC, paint);
    ODCInit::CBinaryRaster rop2(m_pDC, paint);
    m_pDC->MoveTo(x, y);
    m_pDC->LineTo(x, y);
}

void CMswGDC::DrawFilledRectangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const GDCPaint &fill_paint)
{
    ODCInit::SelectFillPaint(m_pDC, fill_paint);
    ODCInit::CBinaryRaster rop2(m_pDC, fill_paint);
    m_pDC->Rectangle(x1, y1, x2, y2);
}

void CMswGDC::DrawRectangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const GDCPaint &stroke_paint)
{
    ODCInit::SelectStrokePaint(m_pDC, stroke_paint);
    //Force to draw only line, no fill.
    OBrush brush(NULL_BRUSH);
    m_pDC->SelectObject(&brush);

    ODCInit::CBinaryRaster rop2(m_pDC, stroke_paint);

    m_pDC->Rectangle(x1, y1, x2, y2);
}

void CMswGDC::DrawEllipse(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const GDCPaint &paint)
{
    ODCInit::SelectStrokePaint(m_pDC, paint);

    //Force to draw only line, no fill.
    OBrush brush(NULL_BRUSH);
    m_pDC->SelectObject(&brush);

    ODCInit::CBinaryRaster rop2(m_pDC, paint);
    
    m_pDC->Ellipse(x1, y1, x2, y2);
}

void CMswGDC::DrawFilledEllipse(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const GDCPaint &paint)
{
    ODCInit::SelectFillPaint(m_pDC, paint);
    ODCInit::CBinaryRaster rop2(m_pDC, paint);
    m_pDC->Ellipse(x1, y1, x2, y2);
}

void CMswGDC::DrawHollowOval(int32_t xCenter, int32_t yCenter, int32_t rx, int32_t ry, int32_t h, const GDCPaint &fill_paint)
{
    OBrush *pBrush = ODCInit::SelectFillPaint(m_pDC, fill_paint);
    ODCInit::CBinaryRaster rop2(m_pDC, fill_paint);

    if (pBrush) {
        ORgn rgnO;
        VERIFY(rgnO.CreateEllipticRgn(xCenter - rx, yCenter - ry, xCenter + rx, yCenter + ry));
        ORgn rgnI;
        VERIFY(rgnI.CreateEllipticRgn(xCenter - (rx - h), yCenter - (ry - h), xCenter + (rx - h), yCenter + (ry - h)));
            
        VERIFY(rgnO.CombineRgn(&rgnO, &rgnI, RGN_DIFF));
        m_pDC->FillRgn(&rgnO, pBrush);
    }
}

void CMswGDC::DrawArc(int32_t x, int32_t y, const int32_t nRadius, const float fStartAngle, const float fSweepAngle, const GDCPaint &paint)
{
    ODCInit::SelectStrokePaint(m_pDC, paint);
    ODCInit::CBinaryRaster rop2(m_pDC, paint);

    m_pDC->MoveTo(x, y);
    ::AngleArc(m_pDC->GetSafeHdc(), x, y, nRadius, fStartAngle, fSweepAngle);
    m_pDC->LineTo(x, y);
}

void CMswGDC::TextOut(const wchar_t *sText, int32_t x, int32_t y, const GDCPaint &paint)
{
    ODCInit::SelectFont(m_pDC, paint);
    m_pDC->TextOut(x, y, sText);	
}

void CMswGDC::DrawText(const wchar_t *sText, const RECT &rect, const GDCPaint &paint)
{
    ODCInit::SelectFont(m_pDC, paint);
    //Reset Font Alignment and use Alignment in the rect
    m_pDC->SetTextAlign(TA_LEFT|TA_TOP|TA_NOUPDATECP);
    const UINT nAlignInRect = paint.GetFontDescr()->m_nTextAlign|DT_NOCLIP|DT_SINGLELINE;
    const int32_t nSize = (int32_t)::wcslen(sText);
    m_pDC->DrawText(sText, nSize, &rect, nAlignInRect);
}

int32_t CMswGDC::GetTextHeight(const GDCPaint &paint) const
{
    ODCInit::SelectFont(m_pDC, paint);

    TEXTMETRIC tmText;
    m_pDC->GetTextMetrics(&tmText);
    return tmText.tmHeight;
}

GDCSize CMswGDC::GetTextExtent(const wchar_t *sText, size_t nCount, const GDCPaint &paint) const
{
    ODCInit::SelectFont(m_pDC, paint);
    const SIZE sz = m_pDC->GetTextExtent(sText, (int32_t)nCount);
    return GDCSize(sz.cx, sz.cy);
}

void CMswGDC::SetViewportOrg(int32_t x, int32_t y)
{
    m_pDC->SetViewportOrg(x, y);
}

GDCPoint CMswGDC::GetViewportOrg() const
{
    POINT pt = m_pDC->GetViewportOrg();
    return GDCPoint(pt.x, pt.y);
}

void CMswGDC::DrawBitmap(HBITMAP hBitmap, int32_t x, int32_t y)
{
    OBitmap obmp(hBitmap);
    OBitmapUtil::DrawBitmap(&obmp, m_pDC, x, y);
}

HDC CMswGDC::GetHDC()
{
    ASSERT(m_pDC);
    return m_pDC->GetSafeHdc();
}

CMswGDC::CMswGDC(GDCBitmap &bitmap, COLORREF background)
{
    const int32_t width  = bitmap.Width();
    const int32_t height = bitmap.Height();
    ASSERT(width > 0);
    ASSERT(height > 0);
    OWindowDC dc(nullptr); // screen
    //  m_pDC->CreateCompatibleDC(&dc); does not work correctly on the delete operator exception is thrown
    // sample:
    // {
    // OBitmapDC bitmap_dc(500, 500);
    //bitmap_dc; // works correctly
    // }
    // {
    //  OBitmapDC *bitmap_dc = new OBitmapDC(500, 500);
    //  delete bitmap_dc; // fails!
    // }
    HDC hDC = ::CreateCompatibleDC(dc.GetSafeHdc()); 
    m_pDC = new ODC(hDC, TRUE);
    m_pDC->SelectObject(bitmap.GetHBITMAP());
    m_pDC->SetMapMode(dc.GetMapMode());
    m_pDC->FillSolidRect(0, 0, width, height, background); // paint background in bitmap
}

CMswGDC::CMswGDC(HWND hwnd)
{
    OWindowDC dc(nullptr); // screen
    HDC hDC = ::CreateCompatibleDC(dc.GetSafeHdc()); 
    m_pDC = new ODC(hDC, TRUE);
}

void CMswGDC::DrawTextByEllipse(double dCenterAngle, int32_t nRadiusX, int32_t nRadiusY, int32_t xCenter, int32_t yCenter, 
                                const wchar_t *sText, double dEllipseAngleRad, const GDCPaint &paint)
{
    HDC hDC = m_pDC->GetSafeHdc();
    CGdiPlusTextDrawUtils::DrawTextByEllipse(hDC, paint, dCenterAngle, nRadiusX, nRadiusY, xCenter, yCenter, sText, dEllipseAngleRad);
}

void CMswGDC::DrawTextByCircle(double dCenterAngle, int32_t nRadius, int32_t nCX, int32_t nCY, 
                               const wchar_t *sText, bool bRevertTextDir, const GDCPaint &paint)
{
    HDC hDC = m_pDC->GetSafeHdc();
    CGdiPlusTextDrawUtils::DrawTextByCircle(hDC, paint, dCenterAngle, nRadius, nCX, nCY, sText, bRevertTextDir);
}