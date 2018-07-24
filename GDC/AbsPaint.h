#ifndef __ABS_PAINT_H__
#define __ABS_PAINT_H__
#pragma once

enum GDCFontWeight;
enum GDCStrokeType;
enum GDCPaintType;

class CAbsPaint
{
// Construction/Destruction
public:
    CAbsPaint() { }
    virtual ~CAbsPaint() { }

// Overrides
public:
    virtual void SetColor(COLORREF color)            = 0;
    virtual void SetAlfa(int32_t nAlfa)              = 0;

    virtual void SetStrokeWidth(double dWidth)       = 0;
    virtual void SetStrokeType(GDCStrokeType type)   = 0;
    virtual void SetPaintType(GDCPaintType type)     = 0;

    virtual void SetTextSize(int32_t nSize)          = 0;
    virtual void SetTextAlign(int32_t nAlign)        = 0;
    virtual void SetTextOrientation(double dAngle)   = 0;
};

#endif