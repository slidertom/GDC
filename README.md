# GDC - Graphical Draw Context

Supported backends: 

  SVG file
  
  HBITMAP (MSW) 
  
  HDC     (MSW) 
  
  
 Compatibility: C++17 standard

Stateless drawings. GDCPaint must be provided for the any drawing function.
Similar approach as SKIA does

 Dependencies:
 SVG - <fstream>
 HBITMAP, HDC - GDI (oligdi.h wrapper by Olivier Langlois), gdi plus

Expected usage area: same drawing code for the svg output and on screen drawings.

It's a good replacement for the [MFC CDC](https://msdn.microsoft.com/en-us/library/fxhhde73.aspx) class. GDC contains similar interface as CDC,
but no dependencies on the [MFC](https://en.wikipedia.org/wiki/Microsoft_Foundation_Class_Library) toolkit.

