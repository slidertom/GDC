#include "stdafx.h"
#include "gdi_plus_util.h"

#include "gdi_plus_inc/GdiPlus.h" // PSDK include

#ifdef _WIN64
    #define GDI_PLUS_LIB "GdiPlus64b.lib"
#else
    #define GDI_PLUS_LIB "GdiPlus.lib"
#endif

#pragma message("GDC will automatically link with " GDI_PLUS_LIB)
#pragma comment(lib, GDI_PLUS_LIB)  

#ifdef _DEBUG
    #define new DEBUG_NEW
#endif

namespace gdi_plus_internal
{
    static void DoDrawPolygonTransparent(HDC hDC, Gdiplus::Point *points, int32_t nSize,
                                         unsigned char alfa, unsigned char r, unsigned char g, unsigned char b)
    {
        Gdiplus::GraphicsPath path;
        path.AddPolygon(points, nSize);
        Gdiplus::Region region(&path);
        Gdiplus::Color color(alfa, r, g, b);
        Gdiplus::SolidBrush brush(color);
        Gdiplus::Graphics dc(hDC);
        dc.SetCompositingQuality(Gdiplus::CompositingQualityHighQuality); // CompositingQualityGammaCorrected
        dc.FillRegion(&brush, &region);
    }

    static Gdiplus::Point *Poly2GdiPlus2(const std::vector<POINT> &poly)
    {
        const size_t nCnt = poly.size();
        ASSERT(nCnt > 0);

        Gdiplus::Point *points = new Gdiplus::Point[nCnt];
        POINT pt;
        for (size_t i1 = 0; i1 < nCnt; ++i1)
        {
            pt = poly[i1];
            points[i1].X = pt.x;
            points[i1].Y = pt.y;
        }

        return points;
    }

    static void DrawLine(HDC hDC, int32_t x1, int32_t y1, int32_t x2, int32_t y2, unsigned char init_r, unsigned char init_g, unsigned char init_b,
                         double dWidth, const Gdiplus::REAL* dashArray, int32_t cntPattern)
    {
        Gdiplus::Graphics dc(hDC);
        Gdiplus::Pen pen(Gdiplus::Color(init_r, init_g, init_b), (Gdiplus::REAL)dWidth);
        pen.SetDashPattern(dashArray, cntPattern);
        dc.DrawLine(&pen, x1, y1, x2, y2);
    }
};

void CGdiPlusUtil::DrawPolygonTransparent(HDC hDC, const std::vector<POINT> &poly,
                                          unsigned char alfa, unsigned char r, unsigned char g, unsigned char b)
{
    Gdiplus::Point *points = gdi_plus_internal::Poly2GdiPlus2(poly);
    gdi_plus_internal::DoDrawPolygonTransparent(hDC, points, (int32_t)poly.size(), alfa, r, g, b);
    delete [] points;
}

void CGdiPlusUtil::DrawLineDash(HDC hDC, int32_t x1, int32_t y1, int32_t x2, int32_t y2, unsigned char init_r, unsigned char init_g, unsigned char init_b, double dWidth)
{
    Gdiplus::REAL dashValues[2] = { 18, 6 };
    gdi_plus_internal::DrawLine(hDC, x1, y1, x2, y2, init_r, init_g, init_b, dWidth, dashValues, 2);
}

void CGdiPlusUtil::DrawLineDot(HDC hDC, int32_t x1, int32_t y1, int32_t x2, int32_t y2, unsigned char init_r, unsigned char init_g, unsigned char init_b, double dWidth)
{
    Gdiplus::REAL dashValues[2] = { 3, 3 };
    gdi_plus_internal::DrawLine(hDC, x1, y1, x2, y2, init_r, init_g, init_b, dWidth, dashValues, 2);
}

void CGdiPlusUtil::DrawLineDashDot(HDC hDC, int32_t x1, int32_t y1, int32_t x2, int32_t y2, unsigned char init_r, unsigned char init_g, unsigned char init_b, double dWidth)
{
    Gdiplus::REAL dashValues[4] = { 9, 6, 3, 6 };
    gdi_plus_internal::DrawLine(hDC, x1, y1, x2, y2, init_r, init_g, init_b, dWidth, dashValues, 4);
}

void CGdiPlusUtil::DrawLineDashDotDot(HDC hDC, int32_t x1, int32_t y1, int32_t x2, int32_t y2, unsigned char init_r, unsigned char init_g, unsigned char init_b, double dWidth)
{
    Gdiplus::REAL dashValues[6] = { 9, 3, 3, 3, 3, 3 };
    gdi_plus_internal::DrawLine(hDC, x1, y1, x2, y2, init_r, init_g, init_b, dWidth, dashValues, 6);
}

void CGdiPlusUtil::DrawPolygon(HDC hDC, const std::vector<POINT> &poly,
                               unsigned char init_r, unsigned char init_g, unsigned char init_b,
                               unsigned char dest_r, unsigned char dest_g, unsigned char dest_b)
{
    const size_t nCnt = poly.size();
    ASSERT(nCnt > 0);

    Gdiplus::Point *points = new Gdiplus::Point[nCnt];

    points[0].X = poly[0].x;
    points[0].Y = poly[0].y;

    Gdiplus::Point pt_max(points[0]);
    Gdiplus::Point pt_min(points[0]);

    for (size_t i1 = 1; i1 < nCnt; ++i1)
    {
        points[i1].X = poly[i1].x;
        points[i1].Y = poly[i1].y;

        if (pt_max.X < poly[i1].x) {
            pt_max.X = poly[i1].x;
        }
        if (pt_max.Y < poly[i1].y) {
            pt_max.Y = poly[i1].y;
        }

        if (pt_min.X > poly[i1].x) {
            pt_min.X = poly[i1].x;
        }
        if (pt_min.Y > poly[i1].y) {
            pt_min.Y = poly[i1].y;
        }
    }

    pt_min.X -= 1; // required -> drawing does not have invalid border
    pt_max.X += 1; // required -> drawing does not have invalid border

                   // use this code if you want to achieve: horizontal color change
    pt_min.Y = int((pt_min.Y + pt_max.Y) / 2.);
    pt_max.Y = pt_min.Y;

    Gdiplus::Graphics dc(hDC);

    Gdiplus::LinearGradientBrush linGrBrush(pt_min, pt_max,
        Gdiplus::Color(255, init_r, init_g, init_b),
        Gdiplus::Color(255, dest_r, dest_g, dest_b));
    dc.FillPolygon(&linGrBrush, points, (int)nCnt);

    delete[] points;
}
