#ifndef __GDC_H__
#define __GDC_H__
#pragma once

#ifdef _BUILD_GDC
    #ifdef WIN32
        #define GDC_UTIL_API __declspec(dllexport)
    #else
        #define GDC_UTIL_API
    #endif
#else
    #ifdef WIN32
        #define GDC_UTIL_API __declspec(dllimport)

        #ifdef _DEBUG
            #define GDC_UTIL_LIB "GDCD.lib"
        #else
            #define GDC_UTIL_LIB "GDC.lib"
        #endif

        #pragma message("GDC will automatically link with " GDC_UTIL_LIB)
        #pragma comment(lib, GDC_UTIL_LIB)
    #else
        #define GDC_UTIL_API
    #endif
#endif

#include "stdint.h"

class ODC;
class OPen;
class OBrush;
class OFont;

enum GDCRasterType
{
    GDC_R2_NONE = 0,
    GDC_R2_XORPEN = 1,
    GDC_R2_NOTXORPEN = 2,
};

enum GDCStrokeType
{
    GDC_PS_SOLID       = 0,
    GDC_PS_DASH        = 1,       /* -------  */
    GDC_PS_DOT         = 2,       /* .......  */
    GDC_PS_DASHDOT     = 3,       /* _._._._  */
	GDC_PS_DASHDOTDOT  = 4        /* _.._.._  */
};

enum GDCPaintType
{
    GDC_STROKE			= 0,
    GDC_FILL			= 1,
    GDC_FILL_DIAGCROSS	= 2,
	GDC_FILL_HORIZONTAL = 3,
	GDC_FILL_VERTICAL	= 4,
	GDC_FILL_FDIAGONAL	= 5,
	GDC_FILL_BDIAGONAL	= 6,
	GDC_FILL_CROSS		= 7
};

// https://developer.mozilla.org/en-US/docs/Web/SVG/Attribute/font-weight
enum GDCFontWeight
{
	GDC_FW_LIGHT        = 300,
	GDC_FW_NORMAL       = 400, // GDC_FW_REGULAR
	GDC_FW_BOLD         = 700
};

enum GDCFontAlign
{
    GDC_TA_LEFT     = 1,  // |Text
    GDC_TA_RIGHT    = 2,  //  Text|
    GDC_TA_CENTER   = 4,  //  Te|xt
    GDC_TA_BOTTOM   = 8,  // __Text__
    GDC_TA_BASELINE = 16, // In text "allignement" line will be under letter "a" not on "g"
	GDC_TA_TOP      = 32  // _______
						  //  Text
};

enum GDCBackgroundMode
{
    GDC_TRANSPARENT = 1,
    GDC_OPAQUE = 2
};

#include "string"

class wxBitmap;
class CAbsPaint;

class GDC_UTIL_API GDCFontDescr
{
// Construction/Destruction
public:
	GDCFontDescr(const wchar_t *sFontName, GDCFontWeight weight, int32_t nHeight, int32_t nSlant, int32_t nUnderline);
	GDCFontDescr(const GDCFontDescr &font_descr);
    ~GDCFontDescr() { }

    //Operators
public:
    GDCFontDescr &operator=(const GDCFontDescr &x);

// Attributes
public:
    float m_fAngle {0.};
    std::wstring m_sFontName;
    GDCFontWeight m_weight {GDC_FW_NORMAL};
    int32_t m_nHeight;
    int32_t m_nSlant;
	int32_t m_nUnderline;
    int32_t m_nTextAlign;
};

class GDC_UTIL_API GDCPaint
{
// Construction/Destruction
public:
    GDCPaint();
	GDCPaint(const wchar_t *sFontName, GDCFontWeight weight, int32_t nHeight, int32_t nSlant, int32_t nUnderline);
	GDCPaint(const GDCFontDescr &font_descr);
    ~GDCPaint();	

// Operations
public:
    void SetColor(COLORREF color);
    COLORREF GetColor() const;

    void SetBkColor(COLORREF color);
    COLORREF GetBkColor() const;

    void SetAlfa(int32_t nAlfa);
    int32_t GetAlfa() const;

    void SetStrokeWidth(float fWidth);
    float GetStrokeWidth() const { return m_fWidth; }

    void SetStrokeType(GDCStrokeType type);
    GDCStrokeType GetStrokeType() const { return  m_stroke_type; }

    void SetPaintType(GDCPaintType type);
    GDCPaintType GetPaintType() const;

    void SetRasterType(GDCRasterType type);
    GDCRasterType GetRasterType() const { return  m_raster_type; }

    void SetBkMode(GDCBackgroundMode mode);
    GDCBackgroundMode GetBkMode() const;

    void SetTextSize(int32_t nSize); // has to be deleted -> user has to create different paints
    void SetTextAlign(int32_t nAlign);
    void SetTextOrientation(float fAngle);

    const GDCFontDescr *GetFontDescr() const { return m_pFontDescr; }

// Attributes
private:
    friend class ODCInit;
    mutable CAbsPaint *m_pPaint {nullptr};

    COLORREF m_color {RGB(0, 0, 0)};
    COLORREF m_bk_color {RGB(255, 255, 255)};
    int32_t m_nAlfa {-1}; //[0..255]; 0 < uses drawing with alfa transparency (ARGB)
    float m_fWidth  {1.};
    GDCStrokeType m_stroke_type {GDC_PS_SOLID};
    GDCPaintType  m_paint_type  {GDC_STROKE};
    GDCFontDescr *m_pFontDescr  {nullptr};
    GDCRasterType m_raster_type {GDC_R2_NONE};
    GDCBackgroundMode m_bk_mode {GDC_TRANSPARENT};
};

class GDCPoint 
{
// Construction/Destruction
public:
    GDCPoint() { }
    GDCPoint(const GDCPoint &pt) : x(pt.x), y(pt.y) { }
    GDCPoint(int32_t src_x, int32_t src_y) : x(src_x), y(src_y) { }
    ~GDCPoint() { }

// Operators
public:
    template <class TPoint>
    void operator=(const TPoint &pt) {
        x = pt.x;
        y = pt.y;
    }

	bool operator==(const GDCPoint &pt) {
		return x == pt.x && y == pt.y;
		return x == pt.x && y == pt.y;
	}

// Attributes
public:
    int32_t x {0}; // maybe float or double ?
    int32_t y {0}; // maybe float or double ?
};

class GDCSize
{
// Construction/Destruction
public:
    GDCSize() { } 
    GDCSize(int32_t x, int32_t y) : cx(x), cy(y) { }
    ~GDCSize() { }

// Attributes
public:
    int32_t cx {0};
    int32_t cy {0};
};

#include "vector"

template <class TPoint, typename FnConversion>
inline std::vector<GDCPoint> Poly2Gdc(const std::vector<TPoint>& src_poly, FnConversion fnConversion)
{
    std::vector<GDCPoint> gdc_poly;
    gdc_poly.reserve(src_poly.size());
    GDCPoint gdc_pt;
    for (const TPoint &src_pt : src_poly) {
        gdc_pt = fnConversion(src_pt);
        gdc_poly.push_back(gdc_pt);
    }
    return gdc_poly;
}

/*
class GDC_UTIL_API GDCPath
{
public:
    void MoveTo(int32_t x, ..
    void LineTo...

// Attributes
private:
    friend class GDC;
    std::vector<GDCPoint> ..
};
*/

class GDCBitmap;
class GDCSvg;
class CAbsGDC;

class GDC_UTIL_API GDC
{
// Construction/Destruction
public:
    GDC(HDC hDC);
    GDC(GDCBitmap &bitmap, COLORREF background = RGB(255, 255, 255));
    GDC(GDCSvg &svg);
	GDC(HWND hwnd);
    ~GDC();

private:
    GDC(GDC &gdc);

// Operations
public:
    void DrawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const GDCPaint &paint);
    template <class TPoint>
    void DrawLine(const TPoint &pt1, const TPoint &pt2, const GDCPaint &paint) {
        DrawLine(pt1.x, pt1.y, pt2.x, pt2.y, paint);
    }

    void DrawPoint(int32_t x, int32_t y, const GDCPaint &paint);
    template <class TPoint>
    void DrawPoint(const TPoint &point, const GDCPaint &paint) {
        DrawPoint(point.x, point.y, paint);
    }

    // https://stackoverflow.com/questions/13545792/drawing-a-filled-rectangle-with-a-border-in-android
    // special function to keep current code compatibility
    void DrawPolygon(const std::vector<GDCPoint> &points, const GDCPaint &fill_paint, const GDCPaint &stroke_paint);
    void DrawPoly(const std::vector<GDCPoint> &points, const GDCPaint &stroke_paint); // closed line => polygon
    void DrawPolyLine(const std::vector<GDCPoint> &points, const GDCPaint &stroke_paint);
    // special case -> avoid if statements in the general drawings
    void DrawPolygonTransparent(const std::vector<GDCPoint> &points, const GDCPaint &fill_paint);
	void DrawPolygonGradient(const std::vector<GDCPoint> &points, const GDCPaint &paintFrom, const GDCPaint &paintTo);
	void DrawPolygonTexture(const std::vector<GDCPoint> &points, const wchar_t * sTexturePath, double dAngle, float fZoom);
	void DrawPolygonTexture(const std::vector<GDCPoint> &points, const std::vector<GDCPoint> &points_exclude, const wchar_t * sTexturePath, double dAngle, float fZoom);

    void DrawFilledRectangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const GDCPaint &fill_paint);
    template <class TRect>
    void DrawFilledRectangle(const TRect &rect, const GDCPaint &fill_paint) {
		DrawFilledRectangle(rect.left, rect.top, rect.right, rect.bottom, fill_paint);
	}
    void DrawRectangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const GDCPaint &stroke_paint);
    template <class TRect>
    void DrawRectangle(const TRect &rect, const GDCPaint &stroke_paint) {
		DrawRectangle(rect.left, rect.top, rect.right, rect.bottom, stroke_paint);
	}
    template <class TPoint> 
    void DrawRectOnPoint(const TPoint &pt, int32_t nSize, const GDCPaint &paint) {
        DrawRectangle(pt.x - nSize, pt.y - nSize, pt.x + nSize, pt.y + nSize, paint); 
    }

	void DrawEllipse(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const GDCPaint &paint);
	template <class TRect>
	void DrawEllipse(const TRect &rect, const GDCPaint &paint) {
		DrawEllipse(rect.left, rect.top, rect.right, rect.bottom, paint);
	}
	void DrawFilledEllipse(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const GDCPaint &paint);
	template <class TRect>
	void DrawFilledEllipse(const TRect &rect, const GDCPaint &paint) {
		DrawFilledEllipse(rect.left, rect.top, rect.right, rect.bottom, paint);
	}
	// HollowOval http://structx.com/Shape_Formulas_035.html
	void DrawHollowOval(int32_t xCenter, int32_t yCenter, int32_t rx, int32_t ry, int32_t h, const GDCPaint &fill_paint);

	void DrawArc(int32_t x, int32_t y, const int32_t nRadius, const float fStartAngle, const float fSweepAngle, const GDCPaint &paint);
	template <class TPoint>
	void DrawArc(const TPoint &point, const int32_t nRadius, const float fStartAngle, const float fSweepAngle, const GDCPaint &paint) {
		DrawArc(point.x, point.y, nRadius, fStartAngle, fSweepAngle, paint);
	}

    void DrawBitmap(const GDCBitmap &bitmap, int32_t x, int32_t y); // temporal solution: HBITMAP must be replaced with the GDCBitmap
        
    void TextOut(const wchar_t *sText, int32_t x, int32_t y, const GDCPaint &paint);
    void TextOutRect(const wchar_t *sText, const RECT &rect, const GDCPaint &paint);
    void DrawText(const wchar_t *sText, const RECT &rect, const GDCPaint &paint);

    void DrawTextByEllipse(double dCenterAngle, int32_t nRadiusX, int32_t nRadiusY, int32_t xCenter, int32_t yCenter, const wchar_t *sText, bool bAllignBottom, double dEllipseAngleRad, const GDCPaint &paint);
    void DrawTextByCircle(double dCenterAngle, int32_t nRadius, int32_t nCX, int32_t nCY, const wchar_t *sText, bool bAllignBottom, bool bRevertTextDir, const GDCPaint &paint);

    // The height (ascent + descent) of characters.
    int32_t GetTextHeight(const GDCPaint &paint) const;
    // Computes the width and height of a line of text, using the provided paint.
	GDCSize GetTextExtent(const wchar_t *sText, size_t nCount, const GDCPaint &paint) const;
	GDCSize GetTextExtent(const wchar_t *sText, const GDCPaint &paint) const {
        return GetTextExtent(sText, ::wcslen(sText), paint);
    }

	void SetViewportOrg(int32_t x, int32_t y);
    template <class TPoint>
    void SetViewportOrg(const TPoint &pt) {
        SetViewportOrg(pt.x, pt.y);
    }
	GDCPoint GetViewportOrg() const;

    HDC GetHDC(); // platform specific (must be used only for the transitional code)

    void BeginGroup(const char *sGroupAttributes);
    void EndGroup();

// Attributes
private:
    CAbsGDC *m_pDC;
};

class CAbsBitmap;
class GDC_UTIL_API GDCBitmap
{
// Construction/Destruction
public:
	GDCBitmap(int32_t width, int32_t height);
    GDCBitmap(HBITMAP hBitmap);
    ~GDCBitmap();

// Operations
public:
    HBITMAP GetHBITMAP() const; // platform specific
    int32_t Width() const;
    int32_t Height() const;

// Attributes
private:
    CAbsBitmap *m_pBitmap;
};

class GDC_UTIL_API GDCSvg
{
// Construction/Destruction
public:
    // if bAutoSize -> true  -> image size: 100%x100%
    // if bAutoSize -> false -> image size: width x height
    // width and height are used for the viewbox
    GDCSvg(const wchar_t *sFilePath, int32_t width, int32_t height, bool bAutoSize);
    GDCSvg(std::string *pBuffer, int32_t width, int32_t height, bool bAutoSize);
    ~GDCSvg();

// Operations
public:
    int32_t Width() const  { return m_nWidth;  }
    int32_t Height() const { return m_nHeight; }
    const wchar_t *GetFilePath() const { return m_sFilePath.c_str(); }
    // if multiple svg images are going to be shown in the one html page
    // gradient id's must be unique
    void SetPrefix(const char *sPrefix);

// Attributes
private:
    friend class GDC;
    int32_t m_nWidth;
    int32_t m_nHeight;
    bool m_bAutoSize {false};
    std::wstring m_sFilePath;
    std::string *m_pBuffer {nullptr};
    std::string m_sPrefix;
};

#endif