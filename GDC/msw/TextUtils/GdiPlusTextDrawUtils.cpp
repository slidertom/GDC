#include "stdafx.h"
#include "GdiPlusTextDrawUtils.h"

#include "../gdi_plus_inc/GdiPlus.h"
#include "../../GDC.h"

#include "vector"
#include "algorithm"
#include "memory"

#ifdef _DEBUG
    #define new DEBUG_NEW
#endif

static const int32_t g_dAngleOffsetDeg = 1;

const double GDC_PI    = 3.1415926535897932384626433832795;
const double gdc_sqeps = 0.00000001;

template <typename value_type> // we do accept double, float, int values
inline double gdc_deg_to_rad(value_type deg) {
    return deg / 180.0 * GDC_PI;	
}
template <typename value_type> // we do accept double, float, int values
inline double gdc_rad_to_deg(value_type deg) {
    return deg / GDC_PI * 180.0;
}

template <class Type>
inline Type gdc_sqr(Type x) {
    return x * x;
}
inline int32_t gdc_iround(double x) {
    if (x >= -0.5) {
        return (int32_t)(x + 0.5);
    }
    else {
        return (int32_t)(x - 0.5);
    }
}
inline bool gdc_eqx(double x, double y, double precision) {
    return (x < y) ? (y - x < precision) : (x - y < precision);
}
inline bool gdc_eqx(double x, double y) {
    return ::gdc_eqx(x, y, gdc_sqeps);
}
inline bool gdc_more(double x, double y) {
    return (x > y && !::gdc_eqx(x, y));
}
inline bool gdc_more_or_eq(double x, double y) {
    return (x > y || ::gdc_eqx(x, y));
}
inline bool gdc_less_or_eq(double x, double y) {
    return (x < y || ::gdc_eqx(x, y));
}
template <class T> // T ->double, float, int32_t, int64_t, ...
inline bool gdc_is_strictly_between(T x, T x1, T x2) {
    if (::gdc_more_or_eq(x1, x2) && ::gdc_less_or_eq(x, x1) && ::gdc_more_or_eq(x, x2)) {
        return true;
    }

    if (::gdc_less_or_eq(x1, x2) && ::gdc_more_or_eq(x, x1) && ::gdc_less_or_eq(x, x2)) {
        return true;
    }

    return false;
}

// wykiboki
template <typename T>
inline void gdc_intersection_point(T x0, T y0, T x1, T y1,
                                   T cx, T cy, double dRadius,
                                   T &pt1x, T &pt1y, T &pt2x, T &pt2y)
{
    T a =  ::gdc_sqr(x1 - x0) + ::gdc_sqr(y1 - y0);

    T b =  T(2.0) * ((x1 - x0)*(x0 - cx) +
                    (y1 - y0)*(y0 - cy));

    T c = ::gdc_sqr(cx) + ::gdc_sqr(cy) +
          ::gdc_sqr(x0) + ::gdc_sqr(y0) -
          T(2.0)  * (cx * x0 + cy * y0) - ::gdc_sqr(dRadius);

    T det =  b * b - T(4.0) * a * c;
    if (det < T(0.0)) {
        return;
    }
    else if (gdc_eqx(det,T(0.0)))
    {
        T delta = -b / (T(2.0) * a);
        pt1x = x0 + delta * (x1 - x0);
        pt1y = y0 + delta * (y1 - y0);
        pt2x = pt1x;
        pt2y = pt1y;
        return;
    }
    else if (det > T(0.0))
    {
        T sqrt_det = sqrt(det);
        T delta = (-b + sqrt_det) / (T(2.0) * a);
        pt1x = x0 + delta * (x1 - x0);
        pt1y = y0 + delta * (y1 - y0);

        delta = (-b - sqrt_det) / (T(2.0) * a);
        pt2x = x0 + delta * (x1 - x0);
        pt2y = y0 + delta * (y1 - y0);
        return;
    }
}

// Sample from: http://www.codeproject.com/Articles/30090/Text-On-A-Path-in-WPF
// Source transfered from the C# project.
namespace draw_by_path_utils
{
    // pointsOnFlattenedPath - aray of shape points. More points - better quality.
    // segmentLengths		 - segments lengths (it can be letters widths)
    // intersectionPoints	 - place, where objects are placed
    template <class TPointVect, class TSegVect>
    void GetCircleIntersectionPointsWithSegments(TPointVect &pointsOnFlattenedPath, 
                                                 const TSegVect &segmentLengths, TPointVect &intersectionPoints)
    {
        if (pointsOnFlattenedPath.empty()) {
            return;
        }

        TPointVect::value_type ptCurr = pointsOnFlattenedPath[0];
        intersectionPoints.push_back(ptCurr);

        // find point on flattened path that is segment length away from current point

        ptrdiff_t nFlattedPathIndex(0);
        ptrdiff_t nSegmentIndex = 1;

        const ptrdiff_t nEndFlat = pointsOnFlattenedPath.size() - 1;
        const ptrdiff_t nEndSeg  = segmentLengths.size() + 1;

        while (nFlattedPathIndex < nEndFlat && nSegmentIndex < nEndSeg) {
            TPointVect::value_type ptIntersection;
            const double x1 = pointsOnFlattenedPath[nFlattedPathIndex].x;
            const double y1 = pointsOnFlattenedPath[nFlattedPathIndex].y;
            const double x2 = pointsOnFlattenedPath[nFlattedPathIndex + 1].x;
            const double y2 = pointsOnFlattenedPath[nFlattedPathIndex + 1].y;
            const double cr = segmentLengths[nSegmentIndex - 1];

            double pt1x;
            double pt1y;
            double pt2x;
            double pt2y;

            gdc_intersection_point(x1, y1, x2, y2, (double)ptCurr.x, (double)ptCurr.y, cr, pt1x, pt1y, pt2x, pt2y);
            
            bool bValidRes(false);
            // Do we have valid result if point is between two segment points
            if ( ::gdc_is_strictly_between(pt1x, x1, x2) &&
                 ::gdc_is_strictly_between(pt1y, y1, y2) ) {
                ptIntersection.x = (LONG)pt1x;
                ptIntersection.y = (LONG)pt1y;
                bValidRes = true; // found solution
            }
            else if (::gdc_is_strictly_between(pt2x, x1, x2) &&
                     ::gdc_is_strictly_between(pt2y, y1, y2) ) {
                    ptIntersection.x = (LONG)pt2x;
                    ptIntersection.y = (LONG)pt2y;
                    bValidRes = true; // found solution
            }

            if (!bValidRes) {
                ++nFlattedPathIndex;
            }
            else {
                intersectionPoints.push_back(ptIntersection);
                ptCurr = ptIntersection;
                pointsOnFlattenedPath[nFlattedPathIndex] = ptCurr;
                ++nSegmentIndex;
            }
        }
    }
};

namespace internal
{
    static inline Gdiplus::Rect GetGdiPlusTextExtent(Gdiplus::Graphics &gp, 
                                                     const Gdiplus::Font &font, 
                                                     const wchar_t *sText, size_t nSize)
    {
        Gdiplus::PointF point_origin(0.0f, 0.0f);
        Gdiplus::RectF rect;
        gp.MeasureString(sText, (INT)nSize, &font, point_origin, &rect);

        Gdiplus::RectF layoutRect(rect);
        Gdiplus::Region region;
        Gdiplus::StringFormat strFormat(Gdiplus::StringFormatFlagsMeasureTrailingSpaces|Gdiplus::StringFormatFlagsDirectionRightToLeft);
        Gdiplus::CharacterRange charRange(0, (INT)nSize);
        strFormat.SetMeasurableCharacterRanges(1, &charRange);
        const int count = strFormat.GetMeasurableCharacterRangeCount();
        gp.MeasureCharacterRanges(sText, (INT)nSize, &font, layoutRect, &strFormat, count, &region);

        Gdiplus::Rect ret_rect;
        region.GetBounds(&ret_rect, &gp);

        return ret_rect;
    }

    class CLetterItem
    {
    public:
        wchar_t m_letter;
        double m_dWidthLP;
        double m_dHeightLP;
    };

    static void DrawCharByAngle(Gdiplus::Graphics &gp, const Gdiplus::Font &font, float dAngle, 
                                const POINT &ptPos, const CLetterItem &char_item, bool bAllignBottom, bool bRevertTextDir)
    {
        Gdiplus::PointF ptPosF((float)ptPos.x, (float)ptPos.y);

        Gdiplus::SolidBrush  brush(Gdiplus::Color(255, 0, 0, 0));

        Gdiplus::Matrix matrix;
        gp.GetTransform(&matrix);

        Gdiplus::Point ptOrigin(0, 0);
        gp.GetRenderingOrigin(&ptOrigin.X, &ptOrigin.Y);

        gp.TranslateTransform(ptPosF.X, ptPosF.Y);
        gp.RotateTransform(dAngle);

        float fPosY(0.0f);

        if (bAllignBottom) {
            fPosY = bRevertTextDir? 0.0f : (float)char_item.m_dHeightLP;
        }
        else {
            fPosY = (float)char_item.m_dHeightLP * 0.5f;
        }

        gp.SetRenderingOrigin((int32_t)ptPosF.X, ::gdc_iround(fPosY));
        gp.DrawString(&char_item.m_letter, 1, &font, Gdiplus::PointF(0.0f, -fPosY), &brush);
        gp.SetRenderingOrigin(ptOrigin.X, ptOrigin.Y);
        gp.SetTransform(&matrix);
    }

#ifdef _DEBUG
    template <class TPointVect>
    void DrawLines(Gdiplus::Graphics &gp, TPointVect &path_points)
    {
        Gdiplus::Pen pen(Gdiplus::Color(), 2.0);

        auto it_end = path_points.end();
        auto it = path_points.begin();

        POINT ptStart;
        ptStart.x = (LONG)(*it).x;
        ptStart.y = (LONG)(*it).y;

        for (; it != it_end; ++it)
        {
            POINT ptEnd;
            ptEnd.x = (LONG)(*it).x;
            ptEnd.y = (LONG)(*it).y;

            gp.DrawLine(&pen, ptStart.x, ptStart.y, ptEnd.x, ptEnd.y);

            ptStart = ptEnd;
        }
    }
#endif

    template <class TPointVect>
    void DrawTextByPath(Gdiplus::Graphics &gp, const Gdiplus::Font &font, TPointVect &path_points, const std::vector<int32_t> &lettersLenghts, const std::vector<CLetterItem> &letters, bool bAllignBottom, bool bRevertTextDir)
    {
        TPointVect intersectionPoints;
        draw_by_path_utils::GetCircleIntersectionPointsWithSegments(path_points, lettersLenghts, intersectionPoints);
        
        if (intersectionPoints.empty()) {
            return;
        }

#ifdef _DEBUG
        //DrawLines(gp, path_points);
#endif

        const size_t nCharCount = letters.size();
        const size_t nEnd = intersectionPoints.size() - 1;
        for (size_t i = 0; i < nEnd; ++i)
        {
            if (nCharCount <= i) {
                break;
            }

            internal::CLetterItem item = letters[i];

            double dOppositeLen = ::sqrt(pow(intersectionPoints[i].x + item.m_dWidthLP - intersectionPoints[i + 1].x, 2.0) + pow(intersectionPoints[i].y - intersectionPoints[i + 1].y, 2.0)) / 2.0;
            double dHypLen      = ::sqrt(pow(intersectionPoints[i].x - intersectionPoints[i + 1].x, 2.0) + pow(intersectionPoints[i].y - intersectionPoints[i + 1].y, 2.0));

            double dRatio = dOppositeLen / dHypLen;

            if (dRatio > 1.0)
                dRatio = 1.0;
            else if (dRatio < -1.0)
                dRatio = -1.0;

            double dAngle = -(2.0 * ::asin(dRatio)) * 180.0 / GDC_PI;

            // adjust sign on angle
            if ((intersectionPoints[i].x + item.m_dWidthLP) > intersectionPoints[i].x) {
                if (intersectionPoints[i + 1].y > intersectionPoints[i].y)
                    dAngle = -dAngle;
            }
            else {
                if (intersectionPoints[i + 1].y < intersectionPoints[i].y)
                    dAngle = -dAngle;
            }

            POINT ptPos;
            ptPos.x = (LONG)intersectionPoints[i].x;
            ptPos.y = (LONG)intersectionPoints[i].y;

            internal::DrawCharByAngle(gp, font, (float)dAngle, ptPos, item, bAllignBottom, bRevertTextDir);
        }
    }

    static void GetTextCharsInfo(Gdiplus::Graphics &gp, const Gdiplus::Font &font, const wchar_t *sWText, std::vector<int32_t> &charsLenghts, int32_t &nAllTextLength, std::vector<CLetterItem> &chars)
    {
        int32_t nSizeAll(0);
        const size_t nSize = ::wcslen(sWText);
        for (size_t k = 0; k < nSize; ++k)
        {
            internal::CLetterItem item;
            item.m_letter = sWText[k];

            Gdiplus::Rect rect_char = GetGdiPlusTextExtent(gp, font, &sWText[k], 1);
            charsLenghts.push_back(rect_char.Width);
            nSizeAll += rect_char.Width;

            item.m_dWidthLP  = rect_char.Width;
            item.m_dHeightLP = rect_char.Height;

            chars.push_back(item);
        }

        const Gdiplus::Rect all_text_size = GetGdiPlusTextExtent(gp, font, sWText, nSize);
        nAllTextLength = all_text_size.Width;
    }

    static void GetCircleStartEndAngles(int32_t nAllTextLength, double dCenterAngle, 
                                        double dRadius, double &dStartAngle, double &dEndAngle)
    {
        const double dSectorAngle = nAllTextLength * 360 / (2.0 * GDC_PI * dRadius);
        dStartAngle = dCenterAngle - (dSectorAngle * 0.5);
        dEndAngle   = dCenterAngle + (dSectorAngle * 0.5) + 1;
    }

    // angles have to be provided in the degrees
    template <class TPointVect, class TPoint>
    inline void LoacalConvertEllipticArcToPoints(TPointVect &points, const TPoint &ptCenter,
                                           double dStartAngle, double dEndAngle, 
                                           double dRadiusX, double dRadiusY, 
                                           double dAngleOffset)
    {
        const double dToRadians = GDC_PI/180.0;
        if ( ::gdc_more(dStartAngle, dEndAngle) ) {
            dEndAngle += 360.0;
        }

        double dCurrentAngle(dStartAngle);

        bool bBreak(false);
        TPoint pt1, pt2;
        double pt1x;
        double pt2x;
        double pt1y;
        double pt2y;

        double dCurrentRad = dCurrentAngle * dToRadians;
        pt1x = ptCenter.x + dRadiusX * cos(dCurrentRad);
        pt1y = ptCenter.y + dRadiusY * sin(dCurrentRad);
        
        pt1.x = (LONG)pt1x;
        pt1.y = (LONG)pt1y;
        points.push_back(pt1);

        while (true) {	
            dCurrentAngle += dAngleOffset;
            if ( gdc_more(dCurrentAngle, dEndAngle) ) {
                dCurrentAngle = dEndAngle;
                bBreak = true;
            }

            dCurrentRad = dCurrentAngle * dToRadians;
            pt2x = ptCenter.x + dRadiusX * cos(dCurrentRad);
            pt2y = ptCenter.y + dRadiusY * sin(dCurrentRad);
            
            if ( pt1x == pt2x && pt1y == pt2y) {
                return;
            }
            
            pt2.x = (LONG)pt2x;
            pt2.y = (LONG)pt2y;
            points.push_back(pt2);

            if ( bBreak ) {
                return;
            }

            pt1x = pt2x;
            pt1y = pt2y;
        }
    }

    template <class TPoint>
    inline double SqDistBetweenPoints_(const TPoint &pt1, const TPoint &pt2)
    {
        const double dX = pt1.x - pt2.x;
        const double dY = pt1.y - pt2.y;
        const double dD = dX * dX + dY * dY;
        return dD;
    }

    // calculates distance of point pt to line (pt1, pt2)
    template <class TPoint>
    inline double DistBetweenPoints_(const TPoint &pt1, const TPoint &pt2)
    {
        return ::sqrt(SqDistBetweenPoints_(pt1, pt2));
    }

    static void GetEllipseStartEndAngles(int32_t nAllTextLength, double dCenterAngle, int32_t nRadiusX, int32_t nRadiusY, double &dStartAngle, double &dEndAngle)
    {
        const double dHalfTextSize = nAllTextLength * 0.5;

        dStartAngle = dCenterAngle - 90.0;
        dEndAngle = dCenterAngle + 90.0;
        
        std::vector<POINT> ellipse_points;
        POINT ptCenter;
        ptCenter.x = 0;
        ptCenter.y = 0;
        LoacalConvertEllipticArcToPoints(ellipse_points, ptCenter, dStartAngle, dEndAngle, nRadiusX, nRadiusY, g_dAngleOffsetDeg);
        
        const ptrdiff_t nMiddle = (ptrdiff_t)(ellipse_points.size() * 0.5);

        POINT ptTo = ellipse_points[nMiddle];
        POINT ptFrom;

        double dLenght(0.0);
        for (ptrdiff_t i = nMiddle - 1; i >= 0; --i) {
            ptFrom = ellipse_points[i];
            dLenght += DistBetweenPoints_(ptFrom, ptTo);

            if (::gdc_more_or_eq(dLenght, dHalfTextSize)) {
                break;
            }

            ptTo = ptFrom;
        }

        // X = a*cos(t)
        const double dAngle = ::gdc_rad_to_deg(acos((double)ptFrom.x/nRadiusX)) - 90.0;

        dStartAngle = dCenterAngle - dAngle;
        dEndAngle = dCenterAngle + dAngle;
    }
}

void CGdiPlusTextDrawUtils::DrawTextByCircle(HDC hDC, const GDCPaint &paint, double dCenterAngle, int32_t nRadius, 
                                             int32_t nCX, int32_t nCY, const wchar_t *sText, bool bAllignBottom, bool bRevertTextDir)
{
    ASSERT(::wcslen(sText) > 0);

    std::unique_ptr<LOGFONT> pLF(new LOGFONT);
    ::memset(pLF.get(), 0, sizeof(LOGFONT));
    const GDCFontDescr *pFontDescr = paint.GetFontDescr();

    ::wcscpy(pLF->lfFaceName, pFontDescr->m_sFontName.c_str());
    pLF->lfHeight      = pFontDescr->m_nHeight;
    pLF->lfWeight      = pFontDescr->m_weight;
    pLF->lfItalic      = pFontDescr->m_nSlant;
    pLF->lfUnderline   = pFontDescr->m_nUnderline;
    pLF->lfOrientation = pLF->lfEscapement = (LONG)pFontDescr->m_fAngle;

    Gdiplus::Graphics    gp(hDC);
    gp.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
    Gdiplus::Font        font(hDC, pLF.get());

    std::vector<int32_t> charsLenghts;
    std::vector<internal::CLetterItem> chars;
    int32_t nAllTextLength;
    internal::GetTextCharsInfo(gp, font, sText, charsLenghts, nAllTextLength, chars);

    double dStartAngle(0.0), dEndAngle(0.0);
    internal::GetCircleStartEndAngles(nAllTextLength, dCenterAngle, nRadius, dStartAngle, dEndAngle);

    std::vector<POINT> path_points;
    POINT ptCenter;
    ptCenter.x = nCX;
    ptCenter.y = nCY;
    internal::LoacalConvertEllipticArcToPoints(path_points, ptCenter, dStartAngle, dEndAngle, nRadius, nRadius, g_dAngleOffsetDeg);

    //Revert text order for better reading.
    if (bRevertTextDir) {
        std::reverse(path_points.begin(), path_points.end());
    }

    internal::DrawTextByPath(gp, font, path_points, charsLenghts, chars, bAllignBottom, bRevertTextDir);
}

namespace ell_to_points
{
    static void CalcRotatedEllipsePoint(double x0, double y0, 
                                               double dRadiusX, double dRadiusY, 
                                               double dEllipseAngleRad, double dSweepAngleRad, 
                                               LONG &x, LONG &y)
    {
        const double x1 = dRadiusX*::cos(dSweepAngleRad)+x0;
        const double y1 = dRadiusY*::sin(dSweepAngleRad)+y0;

        x  = (LONG)((x1-x0)*::cos(dEllipseAngleRad) - (y1-y0)*::sin(dEllipseAngleRad) + x0);
        y  = (LONG)((y1-y0)*::cos(dEllipseAngleRad) + (x1-x0)*::sin(dEllipseAngleRad) + y0);
    }

    template <class TPointVect, class TPoint>
    inline void ConvertEllipticArcToPoints(TPointVect &points, 
                                           const TPoint &ptCenter,
                                           double dStartAngleRad, 
                                           double dEndAndgleRad,
                                           double dRadiusX, 
                                           double dRadiusY, 
                                           double dEllipseAngleRad,
                                           double dAngleOffsetDeg)
    {
        const double dRadD   = dAngleOffsetDeg / 180.0;
        const double dEndRad = dEndAndgleRad-dRadD;

        TPoint pt;
        for (double dRad = dStartAngleRad; dRad < dEndRad; dRad += dRadD)
        {
            CalcRotatedEllipsePoint(ptCenter.x, ptCenter.y, dRadiusX, dRadiusY, dEllipseAngleRad, dRad, pt.x, pt.y);
            points.push_back(pt);
        }

        CalcRotatedEllipsePoint(ptCenter.x, ptCenter.y, dRadiusX, dRadiusY, dEllipseAngleRad, dEndAndgleRad, pt.x, pt.y);
        points.push_back(pt);
    }
}

void CGdiPlusTextDrawUtils::DrawTextByEllipse(HDC hDC, const GDCPaint &paint, double dCenterAngle, int32_t nRadiusX, int32_t nRadiusY, int32_t nCX, int32_t nCY, const wchar_t *sText, bool bAllignBottom, double dEllipseAngleRad)
{
    ASSERT(::wcslen(sText) > 0);

    std::unique_ptr<LOGFONT> pLF(new LOGFONT);
    ::memset(pLF.get(), 0, sizeof(LOGFONT));

    const GDCFontDescr *pFontDescr = paint.GetFontDescr();

    ::wcscpy(pLF->lfFaceName, pFontDescr->m_sFontName.c_str());
    pLF->lfHeight      = pFontDescr->m_nHeight;
    pLF->lfWeight      = pFontDescr->m_weight;
    pLF->lfItalic      = pFontDescr->m_nSlant;
    pLF->lfUnderline   = pFontDescr->m_nUnderline;
    pLF->lfOrientation = pLF->lfEscapement = (LONG)pFontDescr->m_fAngle;

    Gdiplus::Graphics gp(hDC);
    gp.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
    Gdiplus::Font font(hDC, pLF.get());

    std::vector<int32_t> charsLenghts;
    std::vector<internal::CLetterItem> chars;
    int32_t nAllTextLength;
    internal::GetTextCharsInfo(gp, font, sText, charsLenghts, nAllTextLength, chars);

    double dStartAngle(0.0), dEndAngle(0.0);
    internal::GetEllipseStartEndAngles(nAllTextLength, dCenterAngle, nRadiusX, nRadiusY, dStartAngle, dEndAngle);
        
    std::vector<POINT> path_points;
    POINT ptCenter;
    ptCenter.x = nCX;
    ptCenter.y = nCY;
    ell_to_points::ConvertEllipticArcToPoints(path_points, ptCenter, ::gdc_deg_to_rad(dStartAngle), ::gdc_deg_to_rad(dEndAngle), 
                                              nRadiusX, nRadiusY, dEllipseAngleRad, g_dAngleOffsetDeg);

    const bool bRevertTextDir = false;
    internal::DrawTextByPath(gp, font, path_points, charsLenghts, chars, bAllignBottom, bRevertTextDir);
}