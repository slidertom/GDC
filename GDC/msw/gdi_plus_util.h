#ifndef __GDI_PLUS_UTIL_H__
#define __GDI_PLUS_UTIL_H__
#pragma once

#include "vector"

class CGdiPlusUtil
{
// Static operations
public:
    static void DrawPolygon(HDC hDC, const std::vector<POINT> &poly,
							unsigned char init_r, unsigned char init_g, unsigned char init_b,
							unsigned char dest_r, unsigned char dest_g, unsigned char dest_b);

    static void DrawPolygonTransparent(HDC hDC, const std::vector<POINT> &poly,
                                       unsigned char alfa, unsigned char r, unsigned char g, unsigned char b);

    static void DrawLineDash(HDC hDC, int32_t x1, int32_t y1, int32_t x2, int32_t y2,
                             unsigned char init_r, unsigned char init_g, unsigned char init_b, double dWidth);
    static void DrawLineDot(HDC hDC, int32_t x1, int32_t y1, int32_t x2, int32_t y2,
                            unsigned char init_r, unsigned char init_g, unsigned char init_b, double dWidth);
    static void DrawLineDashDot(HDC hDC, int32_t x1, int32_t y1, int32_t x2, int32_t y2,
                                unsigned char init_r, unsigned char init_g, unsigned char init_b, double dWidth);
    static void DrawLineDashDotDot(HDC hDC, int32_t x1, int32_t y1, int32_t x2, int32_t y2,
                                   unsigned char init_r, unsigned char init_g, unsigned char init_b, double dWidth);
};

#endif