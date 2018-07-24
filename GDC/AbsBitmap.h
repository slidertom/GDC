#ifndef __ABS_BITMAP_H__
#define __ABS_BITMAP_H__
#pragma once

class CAbsBitmap
{
// Construction/Destruction
public:
    CAbsBitmap() { }
    virtual ~CAbsBitmap() { }

// Overrides
public:
    virtual HBITMAP GetHBITMAP()  const = 0; // platform specific
    virtual int32_t Width()       const = 0;
    virtual int32_t Height()      const = 0;
};

#endif