#ifndef __MSW_PAINT_H__
#define __MSW_PAINT_H__
#pragma once

#ifndef __ABS_PAINT_H__
    #include "../AbsPaint.h"
#endif

class OPen;
class OBrush;
class OFont;
class GDCFontDescr;

class CMswPaint : public CAbsPaint
{
// Construction/Destruction
public:
    CMswPaint() { }
    CMswPaint(const GDCFontDescr *pFontDescr);
    virtual ~CMswPaint();

// Overrides
public:
    virtual void SetColor(COLORREF color) override;
    virtual void SetAlfa(int32_t nAlfa) override;

    virtual void SetStrokeWidth(double dWidth) override;
    virtual void SetStrokeType(GDCStrokeType type) override;
    virtual void SetPaintType(GDCPaintType type) override;

    virtual void SetTextSize(int32_t nSize) override;
    virtual void SetTextAlign(int32_t nAlign) override;
    virtual void SetTextOrientation(double dAngle) override;
    
// Attributes
private:
    friend class ODCInit;
    mutable OPen    *m_pPen   {nullptr};
    mutable OBrush  *m_pBrush {nullptr};
    mutable LOGFONT *m_pLF    {nullptr};
    mutable OFont   *m_pFont  {nullptr};
};

#endif