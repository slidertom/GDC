#ifndef __ABS_GDC_H__
#define __ABS_GDC_H__
#pragma once

#include "vector"

class GDCSize;
class GDCPoint;
class GDCPaint;
class wxBitmap;

class CAbsGDC
{
// Construction/Destruction
public:
    CAbsGDC() { }
    virtual ~CAbsGDC() { }

// Overrides
public:
    virtual void DrawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const GDCPaint &paint) = 0;
    virtual void DrawPoint(int32_t x, int32_t y, const GDCPaint &paint) = 0;
    
    // https://stackoverflow.com/questions/13545792/drawing-a-filled-rectangle-with-a-border-in-android
    // special function to keep current code compatibility
    virtual void DrawPolygon(const std::vector<GDCPoint> &points, const GDCPaint &fill_paint, const GDCPaint &stroke_paint) = 0;
    virtual void DrawPoly(const std::vector<GDCPoint> &points, const GDCPaint &stroke_paint)     = 0; // closed line => polygon
    virtual void DrawPolyLine(const std::vector<GDCPoint> &points, const GDCPaint &stroke_paint) = 0;
    // special case -> avoid if statements in the general drawings
    virtual void DrawPolygonTransparent(const std::vector<GDCPoint> &points, const GDCPaint &fill_paint)                      = 0;
	virtual void DrawPolygonGradient(const std::vector<GDCPoint> &points, const GDCPaint &paintFrom, const GDCPaint &paintTo) = 0;
	virtual void DrawPolygonTexture(const std::vector<GDCPoint> &points, const wchar_t * sTexturePath, double dAngle, float fZoom) = 0;
	virtual void DrawPolygonTexture(const std::vector<GDCPoint> &points, const std::vector<GDCPoint> &points_exclude, const wchar_t * sTexturePath, double dAngle, float fZoom) = 0;

    virtual void DrawFilledRectangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const GDCPaint &fill_paint) = 0;
    virtual void DrawRectangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const GDCPaint &paint)            = 0;
    
    virtual void DrawEllipse(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const GDCPaint &paint) = 0;
    virtual void DrawFilledEllipse(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const GDCPaint &paint) = 0;
    virtual void DrawHollowOval(int32_t xCenter, int32_t yCenter, int32_t rx, int32_t ry, int32_t h, const GDCPaint &fill_paint) = 0;
    virtual void DrawArc(int32_t x, int32_t y, const int32_t nRadius, const float fStartAngle, const float fSweepAngle, const GDCPaint &paint) = 0;
	
    virtual void DrawBitmap(HBITMAP hBitmap, int32_t x, int32_t y) = 0; // temporal solution: HBITMAP must be replaced with the GDCBitmap
        
    virtual void TextOut(const wchar_t *sText, int32_t x, int32_t y, const GDCPaint &paint) = 0;
    virtual void DrawTextByEllipse(double dCenterAngle, int32_t nRadiusX, int32_t nRadiusY, int32_t xCenter, int32_t yCenter, const wchar_t *sText, bool bAllignBottom, double dEllipseAngleRad, const GDCPaint &paint) = 0;
    virtual void DrawTextByCircle(double dCenterAngle, int32_t nRadius, int32_t nCX, int32_t nCY, const wchar_t *sText, bool bAllignBottom, bool bRevertTextDir, const GDCPaint &paint) = 0;
    
    // The height (ascent + descent) of characters.
    virtual int32_t GetTextHeight(const GDCPaint &paint) const = 0;
    // Computes the width and height of a line of text, using the provided paint.
	virtual GDCSize GetTextExtent(const wchar_t *sText, size_t nCount, const GDCPaint &paint) const = 0;

	virtual void SetViewportOrg(int32_t x, int32_t y) = 0;
    virtual GDCPoint GetViewportOrg() const = 0;

    // sGroupAttrbutes sample: id="bird"
    virtual void BeginGroup(const char *sGroupAttrbutes) = 0; // opengl list or svg group
    virtual void EndGroup() = 0; 

    virtual HDC GetHDC() = 0; // platform specific (must be used only for the transitional code)
};

#endif