#ifndef _SVG_GDC_H__
#define _SVG_GDC_H__
#pragma once

#ifndef __ABS_GDC_H__
    #include "../AbsGDC.h"
#endif

#include "unordered_map"

class CSvgFileAbs;

class SvgGDC : public CAbsGDC
{
// Construction/Destruction
public:
    SvgGDC(const wchar_t *sFilePath, int32_t nWidth, int32_t nHeight);
    SvgGDC(std::string *pBuffer, int32_t nWidth, int32_t nHeight);
    virtual ~SvgGDC();

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
    virtual void DrawRectangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const GDCPaint &paint) override;
    
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

    virtual void BeginGroup(const char *sGroupAttributes) override;
    virtual void EndGroup() override; 

private:
    const char *enter_elem() const { return "\n"; }
    void line(const char *val1);
    void line(const char *val1, const char *val2, const char *val3);
    void line(const char *val1, const char *val2, const char *val3, const char *val4);
    void line(const char *val1, const char *val2, const char *val3, const char *val4, const char *val5);
    void line(const char *val1, const char *val2, const char *val3, const char *val4, const char *val5, const char *val6);
    void line(const char *val1, const char *val2, const char *val3, const char *val4, const char *val5, const char *val6, const char *val7);
    void line(const char *val1, const char *val2, const char *val3, const char *val4, const char *val5, const char *val6, const char *val7, const char *val8);
    void line(const char *val1, const char *val2, const char *val3, const char *val4, const char *val5, const char *val6, const char *val7, const char *val8, const char *val9);

private:
	std::string GetPattern(const GDCPaint &fill_paint);
	std::string GetFill(const GDCPaint &fill_paint);
    
// Attributes
private:
    CSvgFileAbs *m_pFile;
    int32_t m_nWidth;
    int32_t m_nHeight;
    std::unordered_map<std::string, std::string> m_horizontal_grads;
	std::vector<std::string> m_patterns;
};

#endif