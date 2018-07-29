## GDC - Graphical Draw Context

Supported backends: 
  * [SVG](https://en.wikipedia.org/wiki/Scalable_Vector_Graphics) file
  * [HBITMAP](https://docs.microsoft.com/en-us/windows/desktop/api/windef/index) (MSW) 
  * [HDC](https://docs.microsoft.com/en-us/windows/desktop/api/windef/index)     (MSW) 
  
  
 Compatibility: C++17 standard

Stateless drawings. GDCPaint must be provided for the any drawing function.
Similar approach as [Skia](https://skia.org/) does. Skia was too heavy and a bit limited due the missed direct [svg groups](https://developer.mozilla.org/en-US/docs/Web/SVG/Element/g) support.

 Dependencies:
 * SVG backend contains dependency on the ["fstream"](http://www.cplusplus.com/reference/fstream/fstream/).
 * HBITMAP, HDC backend contains dependencies on the GDI ([oligdi.h](https://www.codeproject.com/Articles/12689/Alternative-to-MFC-for-GDI-programming) wrapper by Olivier Langlois) and [GDI+](https://docs.microsoft.com/en-us/windows/desktop/gdiplus/-gdiplus-gdi-start).

Expected usage area: reuse same drawing code for the svg output and on screen drawings.

It's a good replacement for the [MFC CDC](https://msdn.microsoft.com/en-us/library/fxhhde73.aspx) class. GDC contains similar interface as CDC,
but no dependencies on the [MFC](https://en.wikipedia.org/wiki/Microsoft_Foundation_Class_Library) toolkit. Due the oligdi.h
usage GDC performs better than CDC.

Samples: 
```C++
int32_t nHorzSize = 500;
int32_t nVertSize = 500;
GDCSvg svg(L"test.html", nHorzSize, nVertSize);
GDC gdc(svg);

GDCPaint stroke;
stroke.SetColor(RGB(255, 0, 0));
stroke.SetStrokeType(GDC_PS_DASH);

GDCPaint brush;
brush.SetPaintType(GDC_FILL);
brush.SetColor(RGB(255, 255, 255));

std::vector<GDCPoint> points;
points.push_back(GDCPoint(0,    0));
points.push_back(GDCPoint(100,  0));
points.push_back(GDCPoint(100, 100));
points.push_back(GDCPoint(0, 100));

gdc.DrawPolygon(points, brush, stroke);
