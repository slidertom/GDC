#ifndef __MSW_BITMAP_H__
#define __MSW_BITMAP_H__
#pragma once

#ifndef __ABS_BITMAP_H__
    #include "../AbsBitmap.h"
#endif

class OBitmap;

class CMswBitmap : public CAbsBitmap
{
// Construction/Destruction
public:
    CMswBitmap(int32_t width, int32_t height);
    CMswBitmap(HBITMAP hBitmap);
    virtual ~CMswBitmap();

// Overrides
public:
    virtual HBITMAP GetHBITMAP()  const override; // platform specific
    virtual int32_t Width()       const override;
    virtual int32_t Height()      const override;

// Attributes
private:
    friend class CMswGDC;
    OBitmap *m_pBitmap;
    int32_t m_width;
    int32_t m_height;
};

#endif