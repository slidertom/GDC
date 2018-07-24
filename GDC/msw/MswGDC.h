#ifndef __MSW_GDC_H__
#define __MSW_GDC_H__
#pragma once

#ifndef __ABS_GDC_H__
    #include "../AbsGDC.h"
#endif

class ODC;
class GDCBitmap;

class CMswGDC : public CAbsGDC
{
// Construction/Destruction
public:
    CMswGDC(HDC hDC);
    CMswGDC(GDCBitmap &bitmap, COLORREF background);
	CMswGDC(HWND hwnd);
    virtual ~CMswGDC();

private:
    CMswGDC(CMswGDC &gdc);

// Overrides
public:
    virtual void DrawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const GDCPaint &paint) override;
    virtual void DrawPoint(int32_t x, int32_t y, const GDCPaint &paint) override;
    
    virtual void DrawPolygon(const std::vector<GDCPoint> &points, const GDCPaint &fill_paint, const GDCPaint &stroke_paint) override;
    virtual void DrawPoly(const std::vector<GDCPoint> &points, const GDCPaint &stroke_paint) override; // closed line => polygon
    virtual void DrawPolyLine(const std::vector<GDCPoint> &points, const GDCPaint &stroke_paint) override;
    
    virtual void DrawPolygonTransparent(const std::vector<GDCPoint> &points, const GDCPaint &fill_paint) override;
	virtual void DrawPolygonGradient(const std::vector<GDCPoint> &points, const GDCPaint &paintFrom, const GDCPaint &paintTo) override;

    virtual void DrawFilledRectangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const GDCPaint &fill_paint) override;
    virtual void DrawRectangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const GDCPaint &stroke_paint) override;
    
	virtual void DrawEllipse(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const GDCPaint &paint) override;
	virtual void DrawFilledEllipse(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const GDCPaint &paint) override;
	virtual void DrawHollowOval(int32_t xCenter, int32_t yCenter, int32_t rx, int32_t ry, int32_t h, const GDCPaint &fill_paint) override;
	virtual void DrawArc(int32_t x, int32_t y, const int32_t nRadius, const float fStartAngle, const float fSweepAngle, const GDCPaint &paint) override;
	
    virtual void DrawBitmap(HBITMAP hBitmap, int32_t x, int32_t y) override; 
        
    virtual void TextOut(const wchar_t *sText, int32_t x, int32_t y, const GDCPaint &paint) override;
    virtual void DrawTextByEllipse(double dRadiusX, double dRadiusY, const GDCPoint &ptCenter, const wchar_t *sText, double dEllipseAngleRad, const GDCPaint &paint) override;

    // The height (ascent + descent) of characters.
    virtual int32_t GetTextHeight(const GDCPaint &paint) const override;
    // Computes the width and height of a line of text, using the provided paint.
	virtual GDCSize GetTextExtent(const wchar_t *sText, size_t nCount, const GDCPaint &paint) const override;

	virtual void SetViewportOrg(int32_t x, int32_t y) override;
    virtual GDCPoint GetViewportOrg() const override;

    virtual HDC GetHDC() override; // platform specific (must be used only for the transitional code)

    virtual void BeginGroup(const char *sGroupAttributes) override { }
    virtual void EndGroup() override { } 

// Attributes
private:
    ODC *m_pDC;
};

#endif