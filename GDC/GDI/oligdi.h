/*
 * Module ID: oligdi.h
 * Title    : Header file for GDI objects declaration.
 * Purpose  : Provide a small and efficient OO encapsulation of
 *            GDI API.
 *
 * Author   : Olivier Langlois <olanglois@sympatico.ca>
 * Date     : December, 8 2005
 *
 * Note     : Adapted from WPP by Paul Dilascia
 *            http://www.dilascia.com
 *            OFlickerFreeDC is an OLIGDI adaption of
 *            Keith Rule(keithr@europa.com) CMemDC code
 *            http://www.codeproject.com/gdi/flickerfree.asp
 *            The reason to use OLIGDI over MFC classes is because
 *            MFC provides features that most users do not use.
 *
 *            1- CDC::m_hAttribDC is an example. Most of the time
 *               m_hAttribDC == m_hDC and the presence of m_hAttribDC just
 *               add superfluous overhead all over CDC code. Here is an example:
 *
 *            OPoint CDC::MoveTo(int x, int y)
 *            {
 *              ASSERT(m_hDC != NULL);
 *              OPoint point;
 *
 *              if (m_hDC != m_hAttribDC)
 *                 VERIFY(::MoveToEx(m_hDC, x, y, &point));
 *              if (m_hAttribDC != NULL)
 *                 VERIFY(::MoveToEx(m_hAttribDC, x, y, &point));
 *              return point;
 *            }
 *
 *            2- Temporary objects for handle maps. Extremely useful for
 *               windows but not very useful for GDI handles.
 *
 *            The difference is minime but it is there. For people looking
 *            for the best drawing performance, they should use OLIGDI.
 *            On top of that OLIGDI offers a nice feature that is not present
 *            in MFC. The DC object keeps track of replaced default objects and
 *            automatically restore them prior to delete the DC.
 */

#ifndef _OLIGDI_H_
#define _OLIGDI_H_

#include <math.h>

//#ifndef _WINDOWS_
//	#include <windows.h>
//#endif

#ifndef _WINDEBUG_H_
	//#include "windebug.h"
#endif


// Debug macros ([TR]: simplified version)
#ifdef DEBUG
    #define WINASSERTD(a)        ASSERT(a)
    #define LASTERRORDISPLAYD(a) ASSERT(a)
#else
    #define WINASSERTD(a)        
    #define LASTERRORDISPLAYD(a) (a)
#endif

#ifndef _INC_COMMCTRL
	#include <commctrl.h>
#endif

#ifndef _INC_TCHAR
	#include <tchar.h>
#endif

#ifdef _MSC_VER
    #pragma comment(lib, "gdi32.lib")
    #pragma comment(lib, "comctl32.lib")
    #pragma comment(lib, "User32.lib")
#endif

#ifdef __GNUC__
    #include "string.h"
#endif

#include "string"

// Microsoft does not support inline assembler for the x64 compiler
// https://msdn.microsoft.com/en-us/library/wbk4z78b.aspx
// One of the constraints for the x64 compiler is to have 
// no inline assembler support. 
#ifndef __x86_64__
    #define _USEASM
#endif

/////////////////////////////////////////////////////////////////////////////
// mirroring support

//WINBUG: some mirroring stuff will be in wingdi.h someday
#ifndef LAYOUT_LTR
	#define LAYOUT_LTR                         0x00000000
#endif

#ifndef LAYOUT_RTL
	#define LAYOUT_RTL                         0x00000001
	#define NOMIRRORBITMAP                     0x80000000
#endif

//WINBUG: some mirroring stuff will be in winuser.h someday
#ifndef WS_EX_LAYOUTRTL
#define WS_EX_LAYOUTRTL                    0x00400000L
#endif

/*
 * Definitions
 */

/*
 * Special TRUE value to allow bitwise logic with handles.
 * Bitwise logic operators are more efficient than short-circuit
 * operators.
 */
#define OTRUE -1

/*
 * Forward declaration
 */
class ODC;

/*
 * Base class for all drawing objects (pen, brush, etc.)
 */
class ODrawObj
{
protected:
	HGDIOBJ m_hobj;						// Windows handle
	BOOL    m_del;						// whether to delete

	void set(HGDIOBJ h, BOOL d)	{ m_hobj = h; m_del = d; }
	void DeleteObj(void);

	ODrawObj()					{ m_hobj=NULL; m_del=FALSE; }
	ODrawObj(HGDIOBJ h, BOOL d)	{ set(h, d); }
	~ODrawObj()					{ DeleteObj(); }

public:
	HGDIOBJ GetSafeHandle(void)
	{ return this != NULL ? m_hobj : NULL; }
	operator HGDIOBJ()			{ return m_hobj; }

	int GetObject(int nCount, LPVOID lpObject) const
	{ return ::GetObject(m_hobj, nCount, lpObject); }

	friend class OIC;
};

/*
 * Bitmap object.
 */
class OBitmap : public ODrawObj {
public:
	OBitmap( int nWidth,         // bitmap width, in pixels
             int nHeight,        // bitmap height, in pixels
             UINT cPlanes,       // number of color planes
             UINT cBitsPerPel,   // number of bits to identify color
             CONST VOID *lpvBits // color data array
		   ) : ODrawObj(::CreateBitmap(nWidth,nHeight,cPlanes,
		                               cBitsPerPel,lpvBits), TRUE) {}
	OBitmap() {}

	BOOL CreateBitmap( int nWidth,         // bitmap width, in pixels
                       int nHeight,        // bitmap height, in pixels
                       UINT cPlanes,       // number of color planes
                       UINT cBitsPerPel,   // number of bits to identify color
                       CONST VOID *lpvBits // color data array
					 );
	OBitmap(HBITMAP hBitmap, BOOL d = FALSE)
	{
		set(hBitmap, d);
	}

	void SetBitmap(HBITMAP hBitmap, BOOL d = FALSE)
	{
		DeleteObj();
		set(hBitmap, d);
	}

	int GetBitmap(BITMAP* pBitMap)
	{ 
		ASSERT(m_hobj != NULL);
		return ::GetObject(m_hobj, sizeof(BITMAP), pBitMap);
	}

	BOOL LoadMappedBitmap(HINSTANCE hInst, UINT nIDBitmap, UINT nFlags,
						  LPCOLORMAP lpColorMap, int nMapSize)
	{

		HBITMAP hBmp = ::CreateMappedBitmap(hInst, nIDBitmap, (WORD)nFlags,
							 lpColorMap, nMapSize);
		SetBitmap(hBmp, TRUE);
		return hBmp != NULL;
	}

	BOOL CreateCompatibleBitmap(HDC hDC, int nWidth, int nHeight);
	BOOL CreateCompatibleBitmap(ODC* pDC, int nWidth, int nHeight);

	BOOL LoadBitmap(LPCTSTR lpszResourceName, HINSTANCE hInst);
	BOOL LoadBitmap(UINT nIDResource, HINSTANCE hInst);

	LONG getBits(LPSTR buf,int len);
	LONG setBits(LPSTR buf,int len);

	void GetObject(BITMAP &data)
	{ LASTERRORDISPLAYD(::GetObject(m_hobj, sizeof(BITMAP), (LPSTR)&data)); }
	operator HBITMAP() { return (HBITMAP)m_hobj; }
};

/*
 * Pen object
 */
class OPen : public ODrawObj {
public:
	// Constructors
	OPen(void) {}
	OPen(int id) : ODrawObj(::GetStockObject(id), FALSE) {}
	OPen(int style, int wid=1, COLORREF color = RGB(255,255,255) )
		: ODrawObj(::CreatePen(style, wid, color), TRUE) {}
	OPen(LPLOGPEN data) : ODrawObj(::CreatePenIndirect(data), TRUE) {}

	BOOL CreatePen(int style=PS_SOLID, int wid=1, COLORREF color = RGB(255,255,255) );

	void GetObject(LOGPEN &data)
	{ LASTERRORDISPLAYD(::GetObject(m_hobj, sizeof(LOGPEN), (LPSTR)&data)); }
	operator HPEN()	{ return (HPEN)m_hobj; }
};

class OPalette : public ODrawObj
{
// Construction/Destruction
public:
	OPalette() { }
	BOOL CreatePalette(LPLOGPALETTE lpLogPalette)
	{
		HPALETTE hPal = ::CreatePalette(lpLogPalette);
		DeleteObj();
		set(hPal, TRUE);
		return hPal ? TRUE : FALSE;
	}

	BOOL CreateHalftonePalette(HDC hDC)
	{
		HPALETTE hPal = ::CreateHalftonePalette(hDC);
		DeleteObj();
		set(hPal, TRUE);
		return hPal ? TRUE : FALSE;
	}

	operator HPALETTE() const { return (HPALETTE)m_hobj; }

	UINT GetPaletteEntries(UINT nStartIndex, UINT nNumEntries,
						  LPPALETTEENTRY lpPaletteColors) const
	{
		return ::GetPaletteEntries((HPALETTE)m_hobj, nStartIndex,
									nNumEntries, lpPaletteColors); }
};

// Brush object
class OBrush : public ODrawObj
{
public:
	// Constructors
	OBrush() { }
	OBrush(int id) : ODrawObj(::GetStockObject(id), FALSE) {}
	OBrush(COLORREF color) : ODrawObj(::CreateSolidBrush(color), TRUE) {}
	OBrush(COLORREF c, int h) : ODrawObj(::CreateHatchBrush(h, c), TRUE) {}
	OBrush(LOGBRUSH &lb) : ODrawObj(::CreateBrushIndirect(&lb), TRUE)    {}
	OBrush(OBitmap &bm) : ODrawObj(::CreatePatternBrush(bm), TRUE) {}

	void CreateSolidBrush(COLORREF crColor)
	{
		DeleteObj();
		set(::CreateSolidBrush(crColor), TRUE);
	}

	void CreateSysColorBrush(int nIndex)
	{
		DeleteObj();
		set(::GetSysColorBrush(nIndex), TRUE);
	}

	void GetObject(LOGBRUSH &data)
	{ LASTERRORDISPLAYD(::GetObject(m_hobj, sizeof(LOGBRUSH), (LPSTR)&data)); }
	operator HBRUSH() { return (HBRUSH)m_hobj; }

	static OBrush *GetHalftoneBrush();
};

// Font object
class ODC;

class OFont : public ODrawObj {
public:
	// Constructors
	OFont(CONST LOGFONT &lf) : ODrawObj(::CreateFontIndirect(&lf), TRUE) {}
	OFont(void) {}

	BOOL CreateFontIndirect(CONST LOGFONT *lf);
	BOOL CreateFont(
		int nHeight,               // height of font
		int nWidth,                // average character width
		int nEscapement,           // angle of escapement
		int nOrientation,          // base-line orientation angle
		int fnWeight,              // font weight
		DWORD fdwItalic,           // italic attribute option
		DWORD fdwUnderline,        // underline attribute option
		DWORD fdwStrikeOut,        // strikeout attribute option
		DWORD fdwCharSet,          // character set identifier
		DWORD fdwOutputPrecision,  // output precision
		DWORD fdwClipPrecision,    // clipping precision
		DWORD fdwQuality,          // output quality
		DWORD fdwPitchAndFamily,   // pitch and family
		LPCTSTR lpszFace           // typeface name
		);

	void GetObject(LOGFONT &data)
	{ LASTERRORDISPLAYD(::GetObject(m_hobj, sizeof(LOGFONT), (LPSTR)&data)); }
	operator HFONT() { return (HFONT)m_hobj; }

	BOOL CreatePointFont(int nPointSize, LPCTSTR lpszFaceName, ODC *pDC = NULL);
	BOOL CreatePointFontIndirect(const LOGFONT* lpLogFont, ODC* pDC = NULL);

	BOOL CreateStockObject(int nIndex)
	{
		HGDIOBJ hObj = ::GetStockObject(nIndex);
		DeleteObj();
		set(hObj, FALSE);
		return hObj != NULL;
	}

	int GetLogFont(LOGFONT* pLogFont)
	{
		ASSERT(m_hobj != NULL);
		return ::GetObject(m_hobj, sizeof(LOGFONT), pLogFont);
	}
};

class ORgn : public ODrawObj
{
public:
	ORgn(void) {}

	BOOL CreateRectRgn(int x1, int y1, int x2, int y2);
	BOOL CreateEllipticRgn(int x1, int y1, int x2, int y2);

	BOOL CreatePolygonRgn(LPPOINT lpPoints, int nCount, int nMode)
	{
		DeleteObj();
		HRGN hRgn = ::CreatePolygonRgn(lpPoints, nCount, nMode);
		set(hRgn,OTRUE);
		return hRgn ? TRUE : FALSE;
	}

	BOOL CreateRectRgnIndirect(LPCRECT lpRect)
	{
		DeleteObj();
		HRGN hRgn = ::CreateRectRgnIndirect(lpRect);
		set(hRgn,OTRUE);
		return hRgn ? TRUE : FALSE;
	}

	int CombineRgn(ORgn *pRgn1, ORgn *pRgn2, int nCombineMode);
	int OffsetRgn( POINT &p ) { return OffsetRgn(p.x,p.y); }
	int OffsetRgn( int x, int y );
};

/*
 * Class to represent an information context
 * is also the base class for all kinds of device contexts.
 */
class OIC
{
public:
	 // Each type of drawing object has an ID, used as offset to store handle in a table.
	enum WHICHOBJ { SELPEN=0, SELFONT, SELBRUSH, SELBITMAP, NDRAWOBJ };

protected:
	HDC m_hDC;					// Windows handle to DC
	BOOL m_del;

	HANDLE m_origObj[NDRAWOBJ];	// original drawing objects
	int    m_anySelected;		// whether any new objects are selected

	void init(HDC h, BOOL d=TRUE)	// common initializer for all constructors
	{ ::ZeroMemory(this,sizeof(OIC)); m_hDC = h; m_del = d; }

	HGDIOBJ select(WHICHOBJ which, HGDIOBJ h);
	HGDIOBJ select(WHICHOBJ which, ODrawObj *obj)
	{ return select(which, obj->m_hobj); }

private:
	WHICHOBJ GetObjectType(HGDIOBJ h);

public:
	OIC(HDC h=NULL, BOOL d = FALSE) { init(h,d); }
	OIC(  LPCTSTR lpszDriver,       // driver name
          LPCTSTR lpszDevice,       // device name
          LPCTSTR lpszOutput,       // port or file name
          CONST DEVMODE *lpdvmInit  // optional initialization data
		  ) { init(::CreateIC(lpszDriver,lpszDevice,
		                      lpszOutput,lpdvmInit)); }
	virtual ~OIC();

	HDC GetSafeHdc(void) const
	{ return this != NULL ? m_hDC : NULL; }
	operator HDC()				{ return m_hDC; }
	/*
	 * These functions are called automatically by OID destructor
	 * but the user may have to call them explicitly in the case
	 * where the GDI objects have been created on the stack.
	 * In this case, there is no guarantee that the OID destructor
	 * will be called before ODrawObj objects destructor.
	 */
	void restoreSelection(void);
	void restoreSelection(WHICHOBJ which);

	int GetDeviceCaps(int cap)      { return ::GetDeviceCaps(m_hDC, cap); }
	HPEN   SelectObject( OPen *p )   { return (HPEN)  select(SELPEN,p);   }
	HPEN   SelectObject( HPEN p )    { return (HPEN)  select(SELPEN,p);   }
	HFONT  SelectObject( OFont *f )  { return (HFONT) select(SELFONT,f);  }
	HFONT  SelectObject( HFONT f )   { return (HFONT) select(SELFONT,f);  }
	HBRUSH SelectObject( OBrush *b ) { return (HBRUSH)select(SELBRUSH,b); }
	HBRUSH SelectObject( HBRUSH b )  { return (HBRUSH)select(SELBRUSH,b); }

	// Bitmaps can be selected for memory DCs only, and for only one DC at a time.
	HBITMAP SelectObject(OBitmap *bm) { return (HBITMAP)select(SELBITMAP, bm); }
	HBITMAP SelectObject(HBITMAP bm)  { return (HBITMAP)select(SELBITMAP, bm); }

	ptrdiff_t    SelectObject(ORgn* pRgn)
	{
		ptrdiff_t nRetVal = (ptrdiff_t)::SelectObject(m_hDC, pRgn->GetSafeHandle());
		return nRetVal;
	}

	int    SelectClipRgn( ORgn *r )  { return SelectClipRgn((HRGN)r->GetSafeHandle()); }
	int    SelectClipRgn( HRGN r );
	HGDIOBJ SelectStockObject( int h );

	int OffsetClipRgn( SIZE &s ) { return OffsetClipRgn(s.cx,s.cy); }
	int OffsetClipRgn( int x, int y );

	int SetMapMode(int fnMapMode );
	int GetMapMode(void);
	void LPtoDP(LPRECT lpRect) const;
	void DPtoLP(LPRECT lpRect) const;
	void DPtoLP(LPPOINT lpPoints, int nCount = 1) const
	{
		::DPtoLP(m_hDC, lpPoints, nCount);
	}
	void LPtoDP(LPSIZE lpSize) const
	{
		SIZE sizeWinExt = GetWindowExt();
		SIZE sizeVpExt  = GetViewportExt();
		lpSize->cx = MulDiv(lpSize->cx, abs(sizeVpExt.cx), abs(sizeWinExt.cx));
		lpSize->cy = MulDiv(lpSize->cy, abs(sizeVpExt.cy), abs(sizeWinExt.cy));
	}
	void LPtoDP(LPPOINT lpPoints, int nCount = 1) const
	{ ::LPtoDP(m_hDC, lpPoints, nCount); }

	SIZE GetTextExtent( LPCTSTR lpszString, int nCount ) const;
	SIZE GetOutputTextExtent(LPCTSTR lpszString, int nCount) const
	{
		SIZE size;
		::GetTextExtentPoint32(m_hDC, lpszString, nCount, &size);
		return size;
	}

	SIZE GetWindowExt(void) const;
	SIZE SetWindowExt(SIZE s);
	SIZE GetViewportExt(void) const;
	SIZE SetViewportExt(SIZE s);
	POINT GetViewportOrg(void) const;
	POINT SetViewportOrg(POINT &p) { return SetViewportOrg(p.x,p.y); }
	POINT SetViewportOrg(int x, int y);
	POINT GetWindowOrg(void) const;
	POINT SetWindowOrg(int x, int y);
	COLORREF GetBkColor(void) const;
	COLORREF SetBkColor(COLORREF crColor);
	int SetROP2(int nDrawMode) { return ::SetROP2(m_hDC, nDrawMode); }
	int SaveDC(void);
	BOOL RestoreDC(int nSaveDC = -1);
	int SetBkMode(int nBkMode) { return ::SetBkMode(m_hDC, nBkMode); }
	int GetBkMode() const { return ::GetBkMode(m_hDC); }

	COLORREF SetTextColor(COLORREF crColor) { return ::SetTextColor(m_hDC, crColor); }
	POINT OffsetViewportOrg(int nWidth, int nHeight)
	{
		POINT point;
		::OffsetViewportOrgEx(m_hDC, nWidth, nHeight, &point);
		return point;
	}

	COLORREF GetTextColor() const
	{
		return ::GetTextColor(m_hDC);
	}
};

/*
 * Information about the display
 */
class ODisplayInfo : public OIC
{
public:
	ODisplayInfo() : OIC(__TEXT("DISPLAY"),NULL,NULL,NULL) {}
};

/*
 * Generic device context class
 */
class ODC : public OIC
{
public:
	ODC(HDC h=NULL, BOOL d = FALSE)	: OIC(h,d) {}
	ODC(  LPCTSTR lpszDriver,        // driver name
          LPCTSTR lpszDevice,        // device name
          LPCTSTR lpszOutput,        // not used; should be NULL
          CONST DEVMODE *lpInitData  // optional printer data
       )
	{
		init(::CreateDC(lpszDriver, lpszDevice,
			            lpszOutput, lpInitData));
	}

	int DrawText(LPCTSTR lpszString, int nCount, const RECT *lpRect, UINT nFormat);
	BOOL TextOut(int x, int y, LPCTSTR lpszString);
	BOOL ExtTextOut(int x, int y, UINT nOptions, LPCRECT lpRect,
	                LPCTSTR lpszString, UINT nCount, LPINT lpDxWidths);
	BOOL DrawFocusRect(LPCRECT lpRect);
	BOOL Rectangle(int x1, int y1, int x2, int y2);
	BOOL Rectangle(const RECT* lpRect)
	{
		return ::Rectangle(m_hDC, lpRect->left, lpRect->top,
	                              lpRect->right, lpRect->bottom);
	}
	void FrameRect(LPCRECT lpRect, OBrush* pBrush)
	{
		::FrameRect(m_hDC, lpRect, (HBRUSH)pBrush->GetSafeHandle());
	}
	void FrameRect(LPCRECT lpRect, HBRUSH pBrush)
	{
		::FrameRect(m_hDC, lpRect, pBrush);
	}

	void FillRect(const RECT* lpRect, OBrush* pBrush)
	{ ::FillRect(m_hDC, lpRect, (HBRUSH)pBrush->GetSafeHandle()); }

	BOOL Polygon(LPPOINT lpPoints, int nCount)
	{ return ::Polygon(m_hDC, lpPoints, nCount); }

	BOOL Ellipse(int x1, int y1, int x2, int y2);
	BOOL Ellipse(LPCRECT lpRect)
	{
		return ::Ellipse(m_hDC, lpRect->left, lpRect->top,
						 lpRect->right, lpRect->bottom);
	}

	POINT MoveTo(const POINT &point) { ASSERT(m_hDC); return MoveTo(point.x, point.y); }
    template <class TPoint>
    void MoveTo(const TPoint &point) { ASSERT(m_hDC); MoveTo(point.x, point.y);        }
	BOOL LineTo(const POINT &point)  { ASSERT(m_hDC); return LineTo(point.x, point.y); }
    template <class TPoint>
    BOOL LineTo(const TPoint &point) { ASSERT(m_hDC); return LineTo(point.x, point.y); }

	POINT MoveTo(int x, int y);
	BOOL LineTo(int x, int y);
	int GetClipBox(LPRECT lpRect) const;

	// Misc Helper Functions
	void FillSolidRect(LPCRECT lpRect, COLORREF clr);
	void FillSolidRect(int x, int y, int cx, int cy, COLORREF clr);
	BOOL FillRgn(ORgn* pRgn, OBrush* pBrush)
	{
		return ::FillRgn(m_hDC, (HRGN)pRgn->GetSafeHandle(),
						(HBRUSH)pBrush->GetSafeHandle());
	}

	BOOL FrameRgn(ORgn* pRgn, OBrush* pBrush, int nWidth, int nHeight)
	{
		return ::FrameRgn(m_hDC, (HRGN)pRgn->GetSafeHandle(),
						 (HBRUSH)pBrush->GetSafeHandle(),
						  nWidth, nHeight);
	}

	BOOL BitBlt(int x, int y, int nWidth, int nHeight, OIC *pSrcDC,
				int xSrc, int ySrc, DWORD dwRop);

	BOOL PatBlt(int x, int y, int nWidth, int nHeight, DWORD dwRop)
	{  return ::PatBlt(m_hDC, x, y, nWidth, nHeight, dwRop); }

	void Draw3dRect(int x, int y, int cx, int cy,
					COLORREF clrTopLeft,
					COLORREF clrBottomRight)
	{
		FillSolidRect(x, y, cx - 1, 1, clrTopLeft);
		FillSolidRect(x, y, 1, cy - 1, clrTopLeft);
		FillSolidRect(x + cx, y, -1, cy, clrBottomRight);
		FillSolidRect(x, y + cy, cx, -1, clrBottomRight);
	}

	void Draw3dRect(const RECT* lpRect, COLORREF clrTopLeft, COLORREF clrBottomRight)
	{
		Draw3dRect(lpRect->left, lpRect->top, lpRect->right - lpRect->left,
				   lpRect->bottom - lpRect->top, clrTopLeft, clrBottomRight);
	}

	BOOL DrawEdge(LPRECT lpRect, UINT nEdge, UINT nFlags)
	{
		return ::DrawEdge(m_hDC, lpRect, nEdge, nFlags);
	}

	// Rename... does not export DrawState...
	BOOL ODrawState(POINT pt, SIZE size, HICON hIcon, UINT nFlags, HBRUSH hBrush);

	BOOL ODrawState(POINT pt, SIZE size, LPCTSTR lpszText, UINT nFlags,
				   BOOL bPrefixText, int nTextLen, HBRUSH hBrush);

	BOOL GetTextMetrics(LPTEXTMETRIC lpMetrics) const
	{
		return ::GetTextMetrics(m_hDC, lpMetrics);
	}

	void InvertRect(LPCRECT lpRect)
	{ ASSERT(m_hDC != NULL); ::InvertRect(m_hDC, lpRect); }

	COLORREF GetPixel(int x, int y) const
	{
		return ::GetPixel(m_hDC, x, y);
	}

	COLORREF SetPixel(int x, int y, COLORREF color)
	{
		return ::SetPixel(m_hDC, x, y, color);
	}

	DWORD GetLayout() const;
	DWORD SetLayout(DWORD dwSetLayout);

	int ExcludeClipRect(int x1, int y1, int x2, int y2);
	int ExcludeClipRect(LPCRECT lpRect);

	int IntersectClipRect(int x1, int y1, int x2, int y2);
	int IntersectClipRect(LPCRECT lpRect);

	void DrawDragRect(LPCRECT lpRect, SIZE size,
					  LPCRECT lpRectLast,
					  SIZE sizeLast,
					  OBrush *pBrush,
					  OBrush *pBrushLast);

	HPALETTE SelectPalette(HPALETTE pPalette, BOOL bForceBackground)
	{
		return ::SelectPalette(m_hDC,
			pPalette, bForceBackground);
	}

	UINT RealizePalette()
	{
		return ::RealizePalette(m_hDC);
	}
	BOOL DrawIcon(int x, int y, HICON hIcon)
	{ return ::DrawIcon(m_hDC, x, y, hIcon); }

	BOOL GetCharWidth(UINT nFirstChar, UINT nLastChar, LPINT lpBuffer) const
	{ return ::GetCharWidth(m_hDC, nFirstChar, nLastChar, lpBuffer); }

	BOOL GetCharWidth(UINT nFirstChar, UINT nLastChar, float* lpFloatBuffer) const
	{ return ::GetCharWidthFloat(m_hDC, nFirstChar, nLastChar, lpFloatBuffer); }

	UINT SetTextAlign(UINT nFlags)
	{
		return ::SetTextAlign(m_hDC, nFlags);
	}

	int SetTextJustification(int nBreakExtra, int nBreakCount)
	{
		return ::SetTextJustification(m_hDC, nBreakExtra, nBreakCount);
	}

	BOOL CreateCompatibleDC(ODC* pDC)
	{
		HDC hDC = ::CreateCompatibleDC(pDC->GetSafeHdc());
		init(hDC, TRUE);
		return hDC != NULL;
	}

	POINT SetBrushOrg(int x, int y)
	{
		POINT point;
		::SetBrushOrgEx(m_hDC, x, y, &point);
		return point;
	}

	POINT SetBrushOrg(POINT point)
	{
		::SetBrushOrgEx(m_hDC, point.x, point.y, &point);
		return point;
	}

	BOOL DrawFrameControl(LPRECT lpRect, UINT nType, UINT nState)
	{
		return ::DrawFrameControl(m_hDC, lpRect, nType, nState);
	}
};

/*
 * Subclass for client device context
 */
class OClientDC : public ODC
{
protected:
	HWND m_hWnd;					// the window
	OClientDC() : m_hWnd(NULL) {}	// no-arg constructor is private
public:
	OClientDC(HWND hWnd)
		: m_hWnd(hWnd)
	{
		WINASSERTD(hWnd != NULL);
		LASTERRORDISPLAYD(m_hDC = ::GetDC(hWnd));
	}
	virtual ~OClientDC(void);
};

/*
 * Subclass for window DC, including non-client area
 */
class OWindowDC : public OClientDC
{
public:
	OWindowDC(HWND hWnd)
	{
		ASSERT(hWnd == NULL);
		m_hWnd = hWnd;
		LASTERRORDISPLAYD(m_hDC = ::GetWindowDC(hWnd));
	}
};

class OPaintDC : public ODC
{
public:
	PAINTSTRUCT m_ps;			// Windows structure

	OPaintDC(HWND hWnd) : m_hWnd(hWnd)
	{
		WINASSERTD(hWnd);
		LASTERRORDISPLAYD( m_hDC = ::BeginPaint(hWnd, &m_ps) );
	}
	virtual ~OPaintDC();

private:
	HWND        m_hWnd;
};

/*
 * Memory device context
 */
class OMemDC : public ODC
{
protected:
	OMemDC(void) : ODC(NULL,TRUE) {}
	void CreateMemDC(OBitmap *pbm, ODC *pdc)
	{
		WINASSERTD(pbm != NULL);
		LASTERRORDISPLAYD(m_hDC = ::CreateCompatibleDC(pdc->GetSafeHdc()));
		SelectObject(pbm);
	}
public:
	OMemDC(OBitmap *pbm, ODC *pdc = NULL)
		: ODC(NULL,TRUE)
	{
		CreateMemDC(pbm,pdc);
	}
};

class OFlickerFreeDC : public OMemDC
{
private:
	OBitmap		m_bitmap;		// Offscreen bitmap
	ODC        *m_pDC;			// Saves CDC passed in constructor
	RECT		m_rect;			// Rectangle of drawing area.
public:
	OFlickerFreeDC(ODC *pDC, const RECT *pRect = NULL)
	{
		WINASSERTD(pDC != NULL);

		// Some initialization
		m_pDC = pDC;

		// Get the rectangle to draw
		if (pRect == NULL)
		{
			pDC->GetClipBox(&m_rect);
		}
		else
		{
			m_rect = *pRect;
		}

		// Create a Memory DC
		pDC->LPtoDP(&m_rect);
		m_bitmap.CreateCompatibleBitmap(pDC,
			                            m_rect.right - m_rect.left,
										m_rect.bottom- m_rect.top);
		CreateMemDC(&m_bitmap,pDC);
		SetMapMode(pDC->GetMapMode());
		SetWindowExt(pDC->GetWindowExt());
		SetViewportExt(pDC->GetViewportExt());
		pDC->DPtoLP(&m_rect);
		SetWindowOrg(m_rect.left, m_rect.top);

		// Fill background
		FillSolidRect(&m_rect, pDC->GetBkColor());
	}
	virtual ~OFlickerFreeDC(void);
};

/*
 * Enhanced Metafile device context
 */
class OMetaFileDC : public ODC
{
public:
	OMetaFileDC(void) {}
	OMetaFileDC( OIC *pIC,              // handle to reference DC
                 LPCTSTR lpFilename,    // file name
                 CONST RECT *lpRect,    // bounding rectangle
                 LPCTSTR lpDescription  // description string
			   )
	{
		CreateEnhanced(pIC,lpFilename,lpRect,lpDescription);
	}
	virtual ~OMetaFileDC(void);

	BOOL CreateEnhanced( OIC *pIC,             // handle to reference DC
                         LPCTSTR lpFilename,    // file name
                         CONST RECT *lpRect,    // bounding rectangle
                         LPCTSTR lpDescription  // description string
					   );
	HENHMETAFILE CloseEnhanced(void);
};

//----------------------------------------------------------------------------
// @class         OBitmapDC |
//                memory bitmap device context
// @base          public | ODC
//----------------------------------------------------------------------------
// @prog
// Anneke Sicherer-Roetman
// @revs
// 08-10-1999 - First implementation
//----------------------------------------------------------------------------
// @ex The memory bitmap device context can be used in two ways: |
//  // usage 1 : make complete bitmap and use afterwards.
//  // make memory bitmap DC, draw in it, close DC,
//  // draw resulting bitmap, delete bitmap
//  OBitmapDC bitmapDC_1(50, 50, pDC);
//  CAutoPen pen1(&bitmapDC_1, PS_SOLID, 1, RGB(0,0,255));
//  bitmapDC_1.Rectangle(0, 0, 50, 50);
//  bitmapDC_1.MoveTo(0,50);
//  bitmapDC_1.LineTo(50,0);
//  OBitmap *pbmp = bitmapDC_1.Close();
//  DrawBitmap(pbmp, pDC, OPoint(10, 10));
//  delete pbmp;
//  pDC->TextOut(10, 70, "draw OBitmap");
//
//  // usage 2 : use DC as scratch pad and blit whenever needed.
//  // make memory bitmap DC, draw in it, blit DC to screen,
//  // use automatic cleanup in OBitmapDC's destructor
//  OBitmapDC bitmapDC_2(50, 50, pDC);
//  CAutoPen pen2(&bitmapDC_2, PS_SOLID, 1, RGB(255,0,0));
//  bitmapDC_2.Rectangle(0, 0, 50, 50);
//  bitmapDC_2.MoveTo(0, 0);
//  bitmapDC_2.LineTo(50, 50);
//  pDC->BitBlt(200, 10, 50, 50, &bitmapDC_2, 0, 0, SRCCOPY);
//  pDC->TextOut(200, 70, "blit OBitmapDC");
//----------------------------------------------------------------------------
// @todo
//----------------------------------------------------------------------------

class OBitmapDC : public ODC
{
 // Construction/Destruction
public:
  OBitmapDC(int width, int height,
			ODC *pOrigDC = NULL,
			COLORREF background = RGB(255,255,255));
  // destructor, destroys bitmap if it
  // has not been released with GetFinalBitmap
  virtual ~OBitmapDC();

// Operations
public:
  // returns pointer to memory bitmap and prevents further use
  OBitmap *Close();

public:
  static void DrawBitmap(OBitmap *bitmap, ODC *pDC, const int32_t x, const int32_t y);
  static void DrawBitmap(OBitmap *bitmap, ODC *pDC, const RECT &rect,
						 bool stretch = false);

// Attributes
private:
  OBitmap *m_pBitmap; // cmember pointer to memory bitmap
  HBITMAP  m_pOldBmp; // cmember pointer to old DC bitmap

private:
    OBitmapDC(OBitmapDC &dc);
    OBitmapDC *operator=(OBitmapDC &dc);
};

#ifndef _INC_COMMCTRL
	#include <commctrl.h>
#endif

class OImageList
{
// Construction/Destruction
public:
	OImageList() : m_hImageList(NULL) { }
	virtual ~OImageList() { DeleteImageList(); }

// Operations
public:
	BOOL Create(int cx, int cy, UINT nFlags, int nInitial, int nGrow)
	{
		m_hImageList = ImageList_Create(cx, cy, nFlags, nInitial, nGrow);
		return m_hImageList != NULL;
	}

	BOOL Create(HINSTANCE hInst, UINT nBitmapID, int cx, int nGrow, COLORREF crMask)
	{
		m_hImageList = ImageList_LoadBitmap(hInst,
			(LPCTSTR)(DWORD_PTR)nBitmapID, cx, nGrow, crMask);
		return m_hImageList != NULL;
	}

	BOOL Create(HINSTANCE hInst,
			    LPCTSTR lpszBitmapID,
				int cx, int nGrow,
				COLORREF crMask)
	{
		m_hImageList = ImageList_LoadBitmap(hInst, lpszBitmapID, cx, nGrow, crMask);
		return m_hImageList != NULL;
	}

	BOOL Create(OImageList& imagelist1, int nImage1,
			    OImageList& imagelist2, int nImage2, int dx, int dy)
	{
		m_hImageList = ImageList_Merge(imagelist1.m_hImageList, nImage1,
									  imagelist2.m_hImageList, nImage2, dx, dy);
		return m_hImageList != NULL;
	}
#ifndef __GNUC__
	BOOL Create(OImageList* pImageList)
	{
		m_hImageList = ImageList_Duplicate(pImageList->m_hImageList);
		return m_hImageList != NULL;
	}
#endif
	operator HIMAGELIST() const
	{
		return m_hImageList;
	}

	HIMAGELIST GetSafeHandle() const
	{
		return (this == NULL) ? NULL : m_hImageList;
	}

	int GetImageCount() const
	{
		return ImageList_GetImageCount(m_hImageList);
	}

	int Add(OBitmap *pbmImage, OBitmap* pbmMask)
	{
		return ImageList_Add(m_hImageList, (HBITMAP)pbmImage->GetSafeHandle(), (HBITMAP)pbmMask->GetSafeHandle());
	}

	int Add(OBitmap* pbmImage, COLORREF crMask)
	{
		return ImageList_AddMasked(m_hImageList, (HBITMAP)pbmImage->GetSafeHandle(), crMask);
	}

	BOOL Remove(int nImage)
	{
		return ImageList_Remove(m_hImageList, nImage);
	}

	BOOL Replace(int nImage, OBitmap* pbmImage, OBitmap* pbmMask)
	{
		return ImageList_Replace(m_hImageList,
			nImage,
			(HBITMAP)pbmImage->GetSafeHandle(),
			(HBITMAP)pbmMask->GetSafeHandle());
	}

	int Add(HICON hIcon)
	{
		return ImageList_AddIcon(m_hImageList, hIcon);
	}

	int Replace(int nImage, HICON hIcon)
	{
		return ImageList_ReplaceIcon(m_hImageList, nImage, hIcon);
	}

	HICON ExtractIcon(int nImage)
	{
		return ImageList_ExtractIcon(NULL, m_hImageList, nImage);
	}

	COLORREF SetBkColor(COLORREF cr)
	{
		return ImageList_SetBkColor(m_hImageList, cr);
	}

	COLORREF GetBkColor() const
	{
		return ImageList_GetBkColor(m_hImageList);
	}

	BOOL SetOverlayImage(int nImage, int nOverlay)
	{
		return ImageList_SetOverlayImage(m_hImageList, nImage, nOverlay);
	}

	BOOL GetImageInfo(int nImage, IMAGEINFO* pImageInfo) const
	{
		return ImageList_GetImageInfo(m_hImageList, nImage, pImageInfo);
	}

	BOOL BeginDrag(int nImage, POINT ptHotSpot)
	{
		return ImageList_BeginDrag(m_hImageList, nImage, ptHotSpot.x, ptHotSpot.y);
	}

	static void EndDrag()
	{
		ImageList_EndDrag();
	}

	static BOOL DragMove(POINT pt)
	{
		return ImageList_DragMove(pt.x, pt.y);
	}

	BOOL SetDragCursorImage(int nDrag, POINT ptHotSpot)
	{
		return ImageList_SetDragCursorImage(m_hImageList, nDrag, ptHotSpot.x, ptHotSpot.y);
	}

	BOOL DragShowNolock(BOOL bShow)
	{
		return ImageList_DragShowNolock(bShow);
	}

	static HIMAGELIST GetDragImage(LPPOINT lpPoint, LPPOINT lpPointHotSpot)
	{
		return ImageList_GetDragImage(lpPoint, lpPointHotSpot);
	}

	static BOOL DragEnter(HWND hWnd, POINT point)
	{
		return ImageList_DragEnter(hWnd, point.x, point.y);
	}

	static BOOL DragLeave(HWND hWnd)
	{
		return ImageList_DragLeave(hWnd);
	}

	BOOL DeleteImageList()
	{
		if (m_hImageList == NULL)
			return FALSE;

		return ImageList_Destroy(m_hImageList);
	}

	BOOL Draw(HDC hDC, int nImage, POINT pt, UINT nStyle)
	{
		ASSERT(m_hImageList != NULL); 
		return ImageList_Draw(m_hImageList, nImage, hDC, pt.x, pt.y, nStyle);
	}

	HIMAGELIST m_hImageList;            // must be first data member
};

class OBitmapUtil
{
// Static operations
public:
	static PBITMAPINFO CreateBitmapInfoStruct(HBITMAP hBmp);

	// size - size of binary
	static unsigned char *CreateBitmapFileBinary(OBitmap *pBmp, long &size);

	// CopyBitmapToClipboard	- Copies a device-dependent bitmap to clipboard
	// pWnd				- Pointer to window that opens the clipboard
	// bitmap			- The device-dependent bitmap
	// pPal				- Pointer to logical palette - Can be NULL
	// NOTE				- GDI objects are detached from bitmap & pPal
	//				  as the clipboard owns them after the copy
	static void CopyBitmapToClipboard(HWND hWnd, OBitmap& bitmap, OPalette* pPal = NULL);

	static bool CopyBitmap(OBitmap &bmpSrc, OBitmap &bmpDst);

	static HBITMAP LoadSysColorBitmap(HINSTANCE hInst, HRSRC hRsrc, BOOL bMono = FALSE);

	//----------------------------------------------------------------------------
	// Function DrawBitmap
	// draws bitmap at specified point in specified device context
	// rdesc   nothing
	// parm    const OBitmap | *bitmap | bitmap to draw
	// parm    const ODC     | *pDC    | device context to draw in
	// parm    const OPoint  | &point  | top left point of bitmap
	//----------------------------------------------------------------------------
	// Anneke Sicherer-Roetman
	// 06-10-1999 - First implementation
	static void DrawBitmap(OBitmap *bitmap, ODC *pDC, const int32_t x, const int32_t y)
	{
	  BITMAP bm;
	  bitmap->GetObject(bm);
	  int w = bm.bmWidth;
	  int h = bm.bmHeight;
	  OMemDC memDC(bitmap, (ODC *)pDC);
	  ((ODC *)pDC)->BitBlt(x, y, w, h, &memDC, 0, 0, SRCCOPY);
	}

	//----------------------------------------------------------------------------
	// Function DrawBitmap
	// draws bitmap centered in specified rectangle in specified device context
	// rdesc   nothing
	// parm    const OBitmap | *bitmap | bitmap to draw
	// parm    const ODC     | *pDC    | device context to draw in
	// parm    const ORect   | &rect   | rectangle to center in
	//----------------------------------------------------------------------------
	// Anneke Sicherer-Roetman
	// 06-10-1999 - First implementation
	static void DrawBitmap(OBitmap *bitmap, ODC *pDC, const RECT &rect)
	{
	  BITMAP bm;
	  bitmap->GetObject(bm);
	  int w = bm.bmWidth;
	  int h = bm.bmHeight;
	  POINT point;
	  point.x = rect.left + ((rect.right - rect.left) / 2) - (w / 2);
	  point.y = rect.top + ((rect.bottom - rect.top) / 2) - (h / 2);
	  DrawBitmap(bitmap, pDC, point.x, point.y);
	}

	static void DrawTransparentBitmap(ODC *pDC, OBitmap *pBmp, int x, int y, COLORREF transparentColor)
	{
		ODC dcTemp, dcBack, dcObject, dcMem, dcSave;
		OBitmap bmAndBack, bmAndObject, bmAndMem, bmSave;
		BITMAP bmp;
		POINT ptSize;
		COLORREF color;

		dcTemp.CreateCompatibleDC(pDC);
		dcTemp.SelectObject(pBmp);

		pBmp->GetObject(bmp);
		ptSize.x = bmp.bmWidth;
		ptSize.y = bmp.bmHeight;
		dcTemp.DPtoLP(&ptSize);

		dcBack.CreateCompatibleDC(pDC);
		dcObject.CreateCompatibleDC(pDC);
		dcMem.CreateCompatibleDC(pDC);
		dcSave.CreateCompatibleDC(pDC);

		bmAndBack.CreateBitmap(ptSize.x, ptSize.y, 1, 1, NULL);
		bmAndObject.CreateBitmap(ptSize.x, ptSize.y, 1, 1, NULL);

		bmAndMem.CreateCompatibleBitmap(pDC, ptSize.x, ptSize.y);
		bmSave.CreateCompatibleBitmap(pDC, ptSize.x, ptSize.y);

		HBITMAP bmAndBackOld   = dcBack.SelectObject(&bmAndBack);
		HBITMAP bmAndObjectOld = dcObject.SelectObject(&bmAndObject);
		HBITMAP bmAndMemOld    = dcMem.SelectObject(&bmAndMem);
		HBITMAP bmAndSaveOld   = dcSave.SelectObject(&bmSave);

		dcTemp.SetMapMode(pDC->GetMapMode());

		dcSave.BitBlt(0, 0, ptSize.x, ptSize.y, &dcTemp, 0, 0, SRCCOPY);

		color = dcTemp.SetBkColor(transparentColor);

		dcObject.BitBlt(0, 0, ptSize.x, ptSize.y, &dcTemp, 0, 0, SRCCOPY);

		dcTemp.SetBkColor(color);

		dcBack.BitBlt(0, 0, ptSize.x, ptSize.y, &dcObject, 0, 0, NOTSRCCOPY);
		dcMem.BitBlt(0, 0, ptSize.x, ptSize.y, pDC, x, y, SRCCOPY);
		dcMem.BitBlt(0, 0, ptSize.x, ptSize.y, &dcObject, 0, 0, SRCAND);
		dcTemp.BitBlt(0, 0, ptSize.x, ptSize.y, &dcBack, 0, 0, SRCAND);
		dcMem.BitBlt(0, 0, ptSize.x, ptSize.y, &dcTemp, 0, 0, SRCPAINT);
		pDC->BitBlt(x, y, ptSize.x, ptSize.y, &dcMem, 0, 0, SRCCOPY);
		dcTemp.BitBlt(0, 0, ptSize.x, ptSize.y, &dcSave, 0, 0, SRCCOPY);

		dcBack.SelectObject(bmAndBackOld);
		dcObject.SelectObject(bmAndObjectOld);
		dcMem.SelectObject(bmAndMemOld);
		dcSave.SelectObject(bmAndSaveOld);
	}
};

class CFontUtil
{
	static int CALLBACK FontEnumProc(const LOGFONT*, const TEXTMETRIC*, DWORD, LPARAM lParam)
	{
		if ( lParam != (LPARAM)NULL )
		{
			*(BOOL*)lParam = TRUE;
		}
		return 0;
	}

// Static operations
public:
	static BOOL IsFontInstalled(LPCTSTR pszFace)
	{
		BOOL bInstalled;
		HDC hDC;
		LOGFONT lf;

		memset(&lf, 0, sizeof(lf));

		lstrcpy(lf.lfFaceName, pszFace);
		lf.lfCharSet = DEFAULT_CHARSET;

		bInstalled = FALSE;
		hDC = ::GetDC(NULL);
		if (hDC != NULL)
		{
			::EnumFontFamiliesEx(hDC, &lf, FontEnumProc, (LPARAM)&bInstalled, 0);
			::ReleaseDC(NULL, hDC);
		}

		return bInstalled;
	}

	typedef struct _tagFONT_PROPERTIES
	{
		TCHAR csName[1024];
		TCHAR csCopyright[1024];
		TCHAR csTrademark[1024];
		TCHAR csFamily[1024];
	} FONT_PROPERTIES, *LPFONT_PROPERTIES;

	typedef struct _tagFONT_PROPERTIES_ANSI
	{
		char csName[1024];
		char csCopyright[1024];
		char csTrademark[1024];
		char csFamily[1024];
	} FONT_PROPERTIES_ANSI;

	typedef struct _tagTT_OFFSET_TABLE
	{
		USHORT	uMajorVersion;
		USHORT	uMinorVersion;
		USHORT	uNumOfTables;
		USHORT	uSearchRange;
		USHORT	uEntrySelector;
		USHORT	uRangeShift;
	} TT_OFFSET_TABLE;

	typedef struct _tagTT_TABLE_DIRECTORY
	{
		char	szTag[4];			//table name
		ULONG	uCheckSum;			//Check sum
		ULONG	uOffset;			//Offset from beginning of file
		ULONG	uLength;			//length of the table in bytes
	} TT_TABLE_DIRECTORY;

	typedef struct _tagTT_NAME_TABLE_HEADER
	{
		USHORT	uFSelector;			//format selector. Always 0
		USHORT	uNRCount;			//Name Records count
		USHORT	uStorageOffset;		//Offset for strings storage, from start of the table
	} TT_NAME_TABLE_HEADER;

	typedef struct _tagTT_NAME_RECORD
	{
		USHORT	uPlatformID;
		USHORT	uEncodingID;
		USHORT	uLanguageID;
		USHORT	uNameID;
		USHORT	uStringLength;
		USHORT	uStringOffset;	//from start of storage area
	} TT_NAME_RECORD;

	#define SWAPWORD(x)		MAKEWORD(HIBYTE(x), LOBYTE(x))
	#define SWAPLONG(x)		MAKELONG(SWAPWORD(HIWORD(x)), SWAPWORD(LOWORD(x)))

	static BOOL GetFontProperties(LPCTSTR lpszFilePath,
								  FONT_PROPERTIES * lpFontPropsX)
	{
		FONT_PROPERTIES_ANSI fp;
		FONT_PROPERTIES_ANSI * lpFontProps = &fp;

		memset(lpFontProps, 0, sizeof(FONT_PROPERTIES_ANSI));

		HANDLE hFile = INVALID_HANDLE_VALUE;
		hFile = ::CreateFile(lpszFilePath,
							 GENERIC_READ,// | GENERIC_WRITE,
							 FILE_SHARE_READ,
							 NULL,
							 OPEN_EXISTING,
							 FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
							 NULL);

		if (hFile == INVALID_HANDLE_VALUE)
		{
			WINASSERTD(FALSE);
			return FALSE;
		}

		// get the file size
		DWORD dwFileSize = ::GetFileSize(hFile, NULL);

		if (dwFileSize == INVALID_FILE_SIZE)
		{
			WINASSERTD(FALSE);
			::CloseHandle(hFile);
			return FALSE;
		}

		// Create a file mapping object that is the current size of the file
		HANDLE hMappedFile = NULL;
		hMappedFile = ::CreateFileMapping(hFile,
										  NULL,
										  PAGE_READONLY, //PAGE_READWRITE,
										  0,
										  dwFileSize,
										  NULL);

		if (hMappedFile == NULL)
		{
			WINASSERTD(FALSE);
			::CloseHandle(hFile);
			return FALSE;
		}

		LPBYTE lpMapAddress = (LPBYTE) ::MapViewOfFile(hMappedFile,		// handle to file-mapping object
												FILE_MAP_READ,//FILE_MAP_WRITE,			// access mode
												0,						// high-order DWORD of offset
												0,						// low-order DWORD of offset
												0);						// number of bytes to map

		if (lpMapAddress == NULL)
		{
			WINASSERTD(FALSE);
			//(_T("ERROR: %s failed\n"), _T("MapViewOfFile"));
			::CloseHandle(hMappedFile);
			::CloseHandle(hFile);
			return FALSE;
		}

		BOOL bRetVal = FALSE;
		int index = 0;

		TT_OFFSET_TABLE ttOffsetTable;
		memcpy(&ttOffsetTable, &lpMapAddress[index], sizeof(TT_OFFSET_TABLE));
		index += sizeof(TT_OFFSET_TABLE);

		ttOffsetTable.uNumOfTables = SWAPWORD(ttOffsetTable.uNumOfTables);
		ttOffsetTable.uMajorVersion = SWAPWORD(ttOffsetTable.uMajorVersion);
		ttOffsetTable.uMinorVersion = SWAPWORD(ttOffsetTable.uMinorVersion);

		//check is this is a true type font and the version is 1.0
		if (ttOffsetTable.uMajorVersion != 1 || ttOffsetTable.uMinorVersion != 0)
			return bRetVal;

		TT_TABLE_DIRECTORY tblDir;
		memset(&tblDir, 0, sizeof(TT_TABLE_DIRECTORY));
		BOOL bFound = FALSE;
		char szTemp[4096];
		memset(szTemp, 0, sizeof(szTemp));

		for (int i = 0; i< ttOffsetTable.uNumOfTables; i++)
		{
			//f.Read(&tblDir, sizeof(TT_TABLE_DIRECTORY));
			memcpy(&tblDir, &lpMapAddress[index], sizeof(TT_TABLE_DIRECTORY));
			index += sizeof(TT_TABLE_DIRECTORY);

			strncpy(szTemp, tblDir.szTag, 4);
			#if defined(__GNUC__)
			if (strcasecmp(szTemp, "name") == 0)
            #else
            if (stricmp(szTemp, "name") == 0)
            #endif
			{
				bFound = TRUE;
				tblDir.uLength = SWAPLONG(tblDir.uLength);
				tblDir.uOffset = SWAPLONG(tblDir.uOffset);
				break;
			}
			else if (szTemp[0] == 0)
			{
				break;
			}
		}

		if (bFound)
		{
			index = tblDir.uOffset;

			TT_NAME_TABLE_HEADER ttNTHeader;
			memcpy(&ttNTHeader, &lpMapAddress[index], sizeof(TT_NAME_TABLE_HEADER));
			index += sizeof(TT_NAME_TABLE_HEADER);

			ttNTHeader.uNRCount = SWAPWORD(ttNTHeader.uNRCount);
			ttNTHeader.uStorageOffset = SWAPWORD(ttNTHeader.uStorageOffset);
			TT_NAME_RECORD ttRecord;
			bFound = FALSE;

			for (int i = 0;
				 i < ttNTHeader.uNRCount &&
				 (lpFontProps->csCopyright[0] == 0 ||
				  lpFontProps->csName[0] == 0      ||
				  lpFontProps->csTrademark[0] == 0 ||
				  lpFontProps->csFamily[0] == 0);
				 i++)
			{
				memcpy(&ttRecord, &lpMapAddress[index], sizeof(TT_NAME_RECORD));
				index += sizeof(TT_NAME_RECORD);

				ttRecord.uNameID = SWAPWORD(ttRecord.uNameID);
				ttRecord.uStringLength = SWAPWORD(ttRecord.uStringLength);
				ttRecord.uStringOffset = SWAPWORD(ttRecord.uStringOffset);

				if (ttRecord.uNameID == 1 || ttRecord.uNameID == 0 || ttRecord.uNameID == 7)
				{
					int nPos = index; //f.GetPosition();

					index = tblDir.uOffset + ttRecord.uStringOffset + ttNTHeader.uStorageOffset;

					memset(szTemp, 0, sizeof(szTemp));

					memcpy(szTemp, &lpMapAddress[index], ttRecord.uStringLength);
					index += ttRecord.uStringLength;

					if (szTemp[0] != 0)
					{
						WINASSERTD(strlen(szTemp) < sizeof(lpFontProps->csName));

						switch (ttRecord.uNameID)
						{
							case 0:
								if (lpFontProps->csCopyright[0] == 0)
									strncpy(lpFontProps->csCopyright, szTemp,
										sizeof(lpFontProps->csCopyright)-1);
								break;

							case 1:
								if (lpFontProps->csFamily[0] == 0)
									strncpy(lpFontProps->csFamily, szTemp,
										sizeof(lpFontProps->csFamily)-1);
								bRetVal = TRUE;
								break;

							case 4:
								if (lpFontProps->csName[0] == 0)
									strncpy(lpFontProps->csName, szTemp,
										sizeof(lpFontProps->csName)-1);
								break;

							case 7:
								if (lpFontProps->csTrademark[0] == 0)
									strncpy(lpFontProps->csTrademark, szTemp,
										sizeof(lpFontProps->csTrademark)-1);
								break;

							default:
								break;
						}
					}
					index = nPos;
				}
			}
		}

		::UnmapViewOfFile(lpMapAddress);
		::CloseHandle(hMappedFile);
		::CloseHandle(hFile);

		if (lpFontProps->csName[0] == 0)
			strcpy(lpFontProps->csName, lpFontProps->csFamily);

		memset(lpFontPropsX, 0, sizeof(FONT_PROPERTIES));

	#ifdef _UNICODE
		::MultiByteToWideChar(CP_ACP, 0, lpFontProps->csName, -1, lpFontPropsX->csName,
			sizeof(lpFontPropsX->csName)/sizeof(TCHAR)-1);
		::MultiByteToWideChar(CP_ACP, 0, lpFontProps->csCopyright, -1, lpFontPropsX->csCopyright,
			sizeof(lpFontPropsX->csCopyright)/sizeof(TCHAR)-1);
		::MultiByteToWideChar(CP_ACP, 0, lpFontProps->csTrademark, -1, lpFontPropsX->csTrademark,
			sizeof(lpFontPropsX->csTrademark)/sizeof(TCHAR)-1);
		::MultiByteToWideChar(CP_ACP, 0, lpFontProps->csFamily, -1, lpFontPropsX->csFamily,
			sizeof(lpFontPropsX->csFamily)/sizeof(TCHAR)-1);
	#else
		strcpy(lpFontPropsX->csName, lpFontProps->csName);
		strcpy(lpFontPropsX->csCopyright, lpFontProps->csCopyright);
		strcpy(lpFontPropsX->csTrademark, lpFontProps->csTrademark);
		strcpy(lpFontPropsX->csFamily, lpFontProps->csFamily);
	#endif

		return bRetVal;
	}

	static BOOL IsOpenType(LPCTSTR lpszFilePath)
	{
		WINASSERTD(lpszFilePath && lpszFilePath[0] != _T('\0'));
		if (!lpszFilePath || lpszFilePath[0] == _T('\0'))
		{
			return FALSE;
		}

		HANDLE hFile = INVALID_HANDLE_VALUE;
		hFile = ::CreateFile(lpszFilePath,
							 GENERIC_READ,
							 FILE_SHARE_READ,
							 NULL,
							 OPEN_EXISTING,
							 FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
							 NULL);

		if (hFile == INVALID_HANDLE_VALUE)
		{
            //str.Format(_T("ERROR:  failed to open '%s'\n"), lpszFilePath);
			return FALSE;
		}

		// get the file size
		DWORD dwFileSize = ::GetFileSize(hFile, NULL);
        
		if (dwFileSize == INVALID_FILE_SIZE)
		{
            //("ERROR: GetFileSize failed\n");
			::CloseHandle(hFile);
			return FALSE;
		}

		// Create a file mapping object that is the current size of the file
		HANDLE hMappedFile = NULL;
		hMappedFile = ::CreateFileMapping(hFile, NULL,
										  PAGE_READONLY, //PAGE_READWRITE,
										  0,
										  dwFileSize,
										  NULL);

		if (hMappedFile == NULL)
		{
			//ERROR: CreateFileMapping failed\n");
			::CloseHandle(hFile);
			return FALSE;
		}

		LPBYTE lpMapAddress = (LPBYTE) ::MapViewOfFile(hMappedFile,		// handle to file-mapping object
												FILE_MAP_READ,//FILE_MAP_WRITE,			// access mode
												0,						// high-order DWORD of offset
												0,						// low-order DWORD of offset
												0);						// number of bytes to map

		if (lpMapAddress == NULL)
		{
			//"ERROR: MapViewOfFile failed\n");
			::CloseHandle(hMappedFile);
			::CloseHandle(hFile);
			return FALSE;
		}

		int index = 0;

		BOOL bFound = FALSE;
        #ifndef __GNUC__
		__try
		#endif
		{
			TT_OFFSET_TABLE ttOffsetTable;
			memcpy(&ttOffsetTable, &lpMapAddress[index], sizeof(TT_OFFSET_TABLE));
			index += sizeof(TT_OFFSET_TABLE);

			ttOffsetTable.uNumOfTables  = SWAPWORD(ttOffsetTable.uNumOfTables);
			ttOffsetTable.uMajorVersion = SWAPWORD(ttOffsetTable.uMajorVersion);
			ttOffsetTable.uMinorVersion = SWAPWORD(ttOffsetTable.uMinorVersion);

			//check if the version is 1.0
			if (ttOffsetTable.uMajorVersion == 1 && ttOffsetTable.uMinorVersion == 0)
			{
				TT_TABLE_DIRECTORY tblDir;
				memset(&tblDir, 0, sizeof(TT_TABLE_DIRECTORY));

				char szTemp[5];

				USHORT n = ttOffsetTable.uNumOfTables;

				for (USHORT i = 0; i < n; i++)
				{
					memcpy(&tblDir, &lpMapAddress[index], sizeof(TT_TABLE_DIRECTORY));
					index += sizeof(TT_TABLE_DIRECTORY);

					memset(szTemp, 0, sizeof(szTemp));
					strncpy(szTemp, tblDir.szTag, 4);
					#ifdef __GNUC__
					if (strcasecmp(szTemp, "dsig") == 0)
                    #else
					if (stricmp(szTemp, "dsig") == 0)
					#endif
					{
						bFound = TRUE;
						tblDir.uLength = SWAPLONG(tblDir.uLength);
						tblDir.uOffset = SWAPLONG(tblDir.uOffset);
						break;
					}
					else if (szTemp[0] == 0)
					{
						// no more tags
						break;
					}
					else
					{
						// some other tag
					}
				}
			}
		}
		#ifndef __GNUC__
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			//"ERROR: exception in IsOpenType()\n");
		}
        #endif
		::UnmapViewOfFile(lpMapAddress);
		::CloseHandle(hMappedFile);
		::CloseHandle(hFile);

		return bFound;
	}

	#undef SWAPWORD
	#undef SWAPLONG

	static void GetSystemLogFont(LOGFONT &logfont)
	{
		OFont tempFont;

		tempFont.CreateStockObject(SYSTEM_FONT);
		tempFont.GetObject(logfont);

		logfont.lfWidth = 0;
	}
};

// Inline functions
#include "oligdi.inl"

#endif
