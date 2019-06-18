#ifndef __GDI_TEXT_DRAWING_UTILS_H__
#define __GDI_TEXT_DRAWING_UTILS_H__
#pragma once

class GDCPaint;

class CGdiPlusTextDrawUtils
{
// Static operations
public:
    static void DrawTextByCircle(HDC hDC, const GDCPaint &paint, double dCenterAngle, int32_t nRadius, int32_t nCX, int32_t nCY,
                                 const wchar_t *sText, bool bAllignBottom, bool bRevertTextDir);
    static void DrawTextByEllipse(HDC hDC, const GDCPaint &paint, double dCenterAngle, int32_t nRadiusX, int32_t nRadiusY,
                                  int32_t nCX, int32_t nCY, const wchar_t *sText, bool bAllignBottom, double dEllipseAngleRad);
};

#endif