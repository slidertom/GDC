/*
 * Module ID: oligdi.inl
 * Title    : Inlines for OLIGDI.
 *
 * Author   : Olivier Langlois <olanglois@sympatico.ca>
 * Date     : December, 14 2005
 */

/*
 * Module ID: oligdi.inl
 * Title    : Implementation file for GDI objects definition.
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

#ifndef _countof
	#define _countof(array) (sizeof(array)/sizeof(array[0]))
#endif

/******************************************************************************
 * ODrawObj functions definitions
 */

/*-----------------------------------------------------------------------------
 * Public functions
 */

/*=============================================================================
 * Protected functions
 */

/*
 * ODrawObj::DeleteObj function
 *
 * Destroy drawing object, but only if delete flag says so.
 * Internal protected function.
 *
 * An attempt was made to optimize this function in assembly but it has
 * not been concluding.
 */
#ifdef __GNUC__
    #ifdef _USEASM
        #undef _USEASM
    #endif
#endif
#ifdef _USEASM
__declspec(naked)
#endif
inline void ODrawObj::DeleteObj(void)
{
#ifdef _USEASM
	__asm
	{
	mov	eax, DWORD PTR [ecx]
	mov	edx, DWORD PTR [ecx+4]
	mov	DWORD PTR [ecx], 0
	test eax,edx
	jz SHORT DOEnd

	push	eax
	call	DWORD PTR DeleteObject
DOEnd:
	ret 0
	}
#else
	/*
	 * I modified the condition to improve DeleteObj()
	 * performance since it is a function that is called very often
	 */
	if ( (ptrdiff_t)m_hobj & m_del )
	{
		LASTERRORDISPLAYD(::DeleteObject(m_hobj));
	}
	m_hobj = NULL;
#endif
}



/*-----------------------------------------------------------------------------
 * Public functions
 */

/*
 * OIC::restoreSelection function
 *
 * Restore selected display objects (pens, brushes, etc.).
 */
inline void OIC::restoreSelection(void)
{
	for (int i = 0; m_anySelected && i < NDRAWOBJ; i++)
	{
		restoreSelection((WHICHOBJ)i);
	}
}

/******************************************************************************
 * OBitmap functions definitions
 */

inline BOOL OBitmap::CreateBitmap( int nWidth,         // bitmap width, in pixels
                                   int nHeight,        // bitmap height, in pixels
                                   UINT cPlanes,       // number of color planes
                                   UINT cBitsPerPel,   // number of bits to identify color
                                   CONST VOID *lpvBits // color data array
				                 )
{
	HBITMAP hRes;

	LASTERRORDISPLAYD( hRes = ::CreateBitmap(nWidth,nHeight,cPlanes,cBitsPerPel,lpvBits) );
	DeleteObj();
	set(hRes,OTRUE);
	return hRes ? TRUE : FALSE;
}

inline 	BOOL OBitmap::CreateCompatibleBitmap(ODC* pDC, int nWidth, int nHeight)
{
	HBITMAP hRes;

	LASTERRORDISPLAYD( hRes = ::CreateCompatibleBitmap(pDC->GetSafeHdc(),nWidth,nHeight) );
	DeleteObj();
	set(hRes,OTRUE);
	return hRes ? TRUE : FALSE;
}

inline 	BOOL OBitmap::CreateCompatibleBitmap(HDC hDC, int nWidth, int nHeight)
{
	HBITMAP hRes;

	LASTERRORDISPLAYD( hRes = ::CreateCompatibleBitmap(hDC,nWidth,nHeight) );
	DeleteObj();
	set(hRes,OTRUE);
	return hRes ? TRUE : FALSE;
}

inline BOOL OBitmap::LoadBitmap(LPCTSTR lpszResourceName, HINSTANCE hInst)
{
	HBITMAP hRes = ::LoadBitmap(hInst, lpszResourceName);
	DeleteObj();
	set(hRes,OTRUE);
	return hRes ? TRUE : FALSE;
}

inline BOOL OBitmap::LoadBitmap(UINT nIDResource, HINSTANCE hInst)
{
	HBITMAP hRes = ::LoadBitmap(hInst, MAKEINTRESOURCE(nIDResource));
	DeleteObj();
	set(hRes,OTRUE);
	return hRes ? TRUE : FALSE;
}

inline LONG OBitmap::getBits(LPSTR buf,int len)
{
	LONG lRes;
	LASTERRORDISPLAYD(lRes = ::GetBitmapBits((HBITMAP)m_hobj, len, buf));
	return lRes;
}

inline LONG OBitmap::setBits(LPSTR buf,int len)
{
	LONG lRes;
	LASTERRORDISPLAYD(lRes = ::SetBitmapBits((HBITMAP)m_hobj,len,buf));
	return lRes;
}

// OPen

inline BOOL OPen::CreatePen(int style, int wid, COLORREF color )
{
	HPEN hRes;

	LASTERRORDISPLAYD(hRes = ::CreatePen(style,wid,color));
	DeleteObj();
	set(hRes,OTRUE);
	return hRes ? TRUE : FALSE;
}

// OFont

inline BOOL OFont::CreatePointFont(int nPointSize, LPCTSTR lpszFaceName, ODC* pDC)
{
	LOGFONT logFont;
	memset(&logFont, 0, sizeof(LOGFONT));
	logFont.lfCharSet = DEFAULT_CHARSET;
	logFont.lfHeight = nPointSize;
	lstrcpyn(logFont.lfFaceName, lpszFaceName, _countof(logFont.lfFaceName));

	return CreatePointFontIndirect(&logFont, pDC);
}

inline BOOL OFont::CreatePointFontIndirect(const LOGFONT* lpLogFont, ODC* pDC)
{
	HDC hDC;
	if (pDC != NULL)
	{
		hDC = pDC->GetSafeHdc();
	}
	else
	{
		hDC = ::GetDC(NULL);
	}

	// convert nPointSize to logical units based on pDC
	LOGFONT logFont = *lpLogFont;
	POINT pt;
	pt.y = ::GetDeviceCaps(hDC, LOGPIXELSY) * logFont.lfHeight;
	pt.y /= 720;    // 72 points/inch, 10 decipoints/point
	::DPtoLP(hDC, &pt, 1);
	POINT ptOrg = { 0, 0 };
	::DPtoLP(hDC, &ptOrg, 1);
	logFont.lfHeight = -abs(pt.y - ptOrg.y);

	if (pDC == NULL)
		ReleaseDC(NULL, hDC);

	return CreateFontIndirect(&logFont);
}

inline BOOL OFont::CreateFontIndirect(CONST LOGFONT *lf)
{
	HFONT hRes;

	LASTERRORDISPLAYD( hRes = ::CreateFontIndirect(lf) );
	DeleteObj();
	set(hRes,OTRUE);
	return hRes ? TRUE : FALSE;
}

inline BOOL OFont::CreateFont(
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
		)
{
	HFONT hRes;

	LASTERRORDISPLAYD( hRes = ::CreateFont(
		nHeight,nWidth,nEscapement,nOrientation,fnWeight,fdwItalic,
		fdwUnderline,fdwStrikeOut,fdwCharSet,fdwOutputPrecision,
		fdwClipPrecision,fdwQuality,fdwPitchAndFamily,lpszFace) );
	DeleteObj();
	set(hRes,OTRUE);
	return hRes ? TRUE : FALSE;
}

// ORgn

inline BOOL ORgn::CreateRectRgn(int x1, int y1, int x2, int y2)
{
	HRGN hRes;

	LASTERRORDISPLAYD( hRes = ::CreateRectRgn(x1,y1,x2,y2));
	DeleteObj();
	set(hRes,OTRUE);
	return hRes ? TRUE : FALSE;
}

inline BOOL ORgn::CreateEllipticRgn(int x1, int y1, int x2, int y2)
{
	HRGN hRes;

	LASTERRORDISPLAYD( hRes = ::CreateEllipticRgn(x1,y1,x2,y2));
	DeleteObj();
	set(hRes,OTRUE);
	return hRes ? TRUE : FALSE;
}

inline int ORgn::CombineRgn(ORgn *pRgn1, ORgn *pRgn2, int nCombineMode)
{
	int iRes;
	WINASSERTD(m_hobj != NULL);

	LASTERRORDISPLAYD( (iRes = ::CombineRgn((HRGN)m_hobj,
		               (HRGN)pRgn1->GetSafeHandle(),
	                   (HRGN)pRgn2->GetSafeHandle(),
					   nCombineMode)) != ERROR);
	return iRes;
}

inline int ORgn::OffsetRgn( int x, int y )
{
	int iRes;
	WINASSERTD(m_hobj != NULL);

	LASTERRORDISPLAYD( (iRes = ::OffsetRgn((HRGN)m_hobj,x,y)) != ERROR);
	return iRes;
}

/******************************************************************************
 * OIC functions definitions
 */

/*
 * Constructor/Destructor
 */
inline OIC::~OIC()
{
	if (m_hDC)
	{
		restoreSelection();
		if( m_del )
		{
			LASTERRORDISPLAYD(::DeleteDC(m_hDC));
		}
	}
}

/*
 * OIC::SelectStockObject function
 */
inline HGDIOBJ OIC::SelectStockObject(int h)
{
    ASSERT(::GetStockObject(h));
	HGDIOBJ hObj = ::GetStockObject(h);
	WHICHOBJ objType = GetObjectType(hObj);

	if( objType < NDRAWOBJ ) {
		hObj = select(objType,hObj);
	}
	else {
		hObj = NULL;
	}

	return hObj;
}

inline void OIC::restoreSelection(WHICHOBJ which)
{
	if( m_origObj[which] )
	{
		WINASSERTD(m_hDC != NULL);
		::SelectObject(m_hDC, m_origObj[which]);
		m_origObj[which] = NULL;		// don't restore twice!
		m_anySelected--;
		WINASSERTD( m_anySelected >= 0 );
	}
}

inline int OIC::SelectClipRgn( HRGN r )
{
	WINASSERTD(m_hDC != NULL);
	int nRetVal;
	LASTERRORDISPLAYD((nRetVal = ::SelectClipRgn(m_hDC, r)) != ERROR);
	return nRetVal;
}

inline int OIC::OffsetClipRgn( int x, int y )
{
	WINASSERTD(m_hDC != NULL);
	int nRetVal;
	LASTERRORDISPLAYD((nRetVal = ::OffsetClipRgn(m_hDC, x, y)) != ERROR);
	return nRetVal;
}

inline int OIC::SetMapMode(int fnMapMode )
{
	WINASSERTD(m_hDC != NULL);
	int nRetVal;
	LASTERRORDISPLAYD(nRetVal = ::SetMapMode(m_hDC, fnMapMode));
	return nRetVal;
}

inline int OIC::GetMapMode(void)
{
	WINASSERTD(m_hDC != NULL);
	int nRetVal;
	LASTERRORDISPLAYD(nRetVal = ::GetMapMode(m_hDC));
	return nRetVal;
}

inline void OIC::LPtoDP(LPRECT lpRect) const
{
	WINASSERTD(m_hDC != NULL);
	LASTERRORDISPLAYD(::LPtoDP(m_hDC, (LPPOINT)lpRect, 2));
}

inline void OIC::DPtoLP(LPRECT lpRect) const
{
	WINASSERTD(m_hDC != NULL);
	LASTERRORDISPLAYD(::DPtoLP(m_hDC, (LPPOINT)lpRect, 2));
}

inline SIZE OIC::GetTextExtent( LPCTSTR lpszString, int nCount ) const
{
	WINASSERTD(m_hDC != NULL);
	SIZE size;
	LASTERRORDISPLAYD(::GetTextExtentPoint32(m_hDC, lpszString, nCount, &size));
	return size;
}

inline SIZE OIC::GetWindowExt(void) const
{
	WINASSERTD(m_hDC != NULL);
	SIZE size;
	LASTERRORDISPLAYD(::GetWindowExtEx(m_hDC, &size));
	return size;
}

inline SIZE OIC::SetWindowExt(SIZE s)
{
	WINASSERTD(m_hDC != NULL);
	SIZE size;
	LASTERRORDISPLAYD(::SetWindowExtEx(m_hDC, s.cx, s.cy, &size));
	return size;
}

inline SIZE OIC::GetViewportExt(void) const
{
	WINASSERTD(m_hDC != NULL);
	SIZE size;
	LASTERRORDISPLAYD(::GetViewportExtEx(m_hDC, &size));
	return size;
}

inline SIZE OIC::SetViewportExt(SIZE s)
{
	WINASSERTD(m_hDC != NULL);
	SIZE size;
	LASTERRORDISPLAYD(::SetViewportExtEx(m_hDC, s.cx, s.cy, &size));
	return size;
}

inline POINT OIC::GetViewportOrg(void) const
{
	WINASSERTD(m_hDC != NULL);
	POINT point;
	LASTERRORDISPLAYD(::GetViewportOrgEx(m_hDC, &point));
	return point;
}

inline POINT OIC::SetViewportOrg(int x, int y)
{
	WINASSERTD(m_hDC != NULL);
	POINT point;
	LASTERRORDISPLAYD(::SetViewportOrgEx(m_hDC, x, y, &point));
	return point;
}

inline POINT OIC::GetWindowOrg(void) const
{
	WINASSERTD(m_hDC != NULL);
	POINT point;
	LASTERRORDISPLAYD(::GetWindowOrgEx(m_hDC, &point));
	return point;
}

inline POINT OIC::SetWindowOrg(int x, int y)
{
	WINASSERTD(m_hDC != NULL);
	POINT point;
	LASTERRORDISPLAYD(::SetWindowOrgEx(m_hDC, x, y, &point));
	return point;
}

inline COLORREF OIC::GetBkColor(void) const
{
	WINASSERTD(m_hDC != NULL);
	COLORREF crRetVal;
	LASTERRORDISPLAYD( (crRetVal = ::GetBkColor(m_hDC)) != CLR_INVALID);
	return crRetVal;
}

inline COLORREF OIC::SetBkColor(COLORREF crColor)
{
	WINASSERTD(m_hDC != NULL);
	COLORREF crRetVal;
	LASTERRORDISPLAYD( (crRetVal = ::SetBkColor(m_hDC, crColor)) !=
	                   CLR_INVALID);
	return crRetVal;
}

inline int OIC::SaveDC(void)
{
	WINASSERTD(m_hDC != NULL);
	int iRes;
	LASTERRORDISPLAYD( iRes = ::SaveDC(m_hDC) );
	return iRes;
}

inline BOOL OIC::RestoreDC(int nSaveDC)
{
	WINASSERTD(m_hDC != NULL);
	BOOL bRes;
	LASTERRORDISPLAYD( bRes = ::RestoreDC(m_hDC,nSaveDC) );
	return bRes;
}

/*=============================================================================
 * Protected functions
 */

/*
 * OIC::select function
 *
 * Protected method to select a display object
 * Destroys old selected object if required.
 * "which" specifies whether object is a pen, brush, etc.
 * "del" specifies whether to delete this object.
 */
inline HGDIOBJ OIC::select(WHICHOBJ which, HGDIOBJ h)
{
	HGDIOBJ old;

	WINASSERTD(h);
	old = ::SelectObject(m_hDC, h);
	WINASSERTD(old && old != HGDI_ERROR);

	if( m_origObj[which] == NULL )
	{
		m_origObj[which] = old;
		m_anySelected++;
		WINASSERTD( m_anySelected <= NDRAWOBJ );
	}
	else if( m_origObj[which] == h )
	{
		m_origObj[which] = NULL;
		m_anySelected--;
		WINASSERTD( m_anySelected >= 0 );
	}

	return old;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Private functions
 */

/*
 * OIC::GetObjectType function
 */
inline OIC::WHICHOBJ OIC::GetObjectType(HGDIOBJ h)
{
	DWORD    dwObjType;
	WHICHOBJ objType;

	LASTERRORDISPLAYD(dwObjType = ::GetObjectType(h));
	switch(dwObjType)
	{
	case OBJ_PEN:
	case OBJ_EXTPEN:
		objType = SELPEN;
		break;
	case OBJ_FONT:
		objType = SELFONT;
		break;
	case OBJ_BRUSH:
		objType = SELBRUSH;
		break;
	case OBJ_BITMAP:
		objType = SELBITMAP;
		break;
	default:
		objType = NDRAWOBJ;
		//D( TCHAR tmpBuf[1024]; )
		//D( wsprintf(tmpBuf,"Unsupported object type: %d", dwObjType); )
		break;
	}
	return objType;
}

// ODC
inline int ODC::DrawText(LPCTSTR lpszString, int nCount, const RECT *lpRect, UINT nFormat)
{
	int iRes;
	WINASSERTD(m_hDC);
	LASTERRORDISPLAYD(iRes = ::DrawText(m_hDC, lpszString, nCount, (LPRECT)lpRect, nFormat));
	return iRes;
}

inline BOOL ODC::TextOut(int x, int y, LPCTSTR lpszString)
{
	BOOL bRes;
	WINASSERTD(m_hDC);
	LASTERRORDISPLAYD(bRes = ::TextOut(m_hDC, x, y, lpszString, (int32_t)::wcslen(lpszString)));
	return bRes;
}

inline BOOL ODC::ExtTextOut(int x, int y, UINT nOptions, LPCRECT lpRect,
							LPCTSTR lpszString, UINT nCount, LPINT lpDxWidths)
{
	BOOL bRes;
	WINASSERTD(m_hDC != NULL);
	LASTERRORDISPLAYD(bRes = ::ExtTextOut(m_hDC, x, y, nOptions, lpRect,
	                                      lpszString, nCount, lpDxWidths));
	return bRes;
}

inline BOOL ODC::DrawFocusRect(LPCRECT lpRect)
{
	BOOL bRes;
	WINASSERTD(m_hDC != NULL);
	LASTERRORDISPLAYD(bRes = ::DrawFocusRect(m_hDC,lpRect));
	return bRes;
}

inline BOOL ODC::Rectangle(int x1, int y1, int x2, int y2)
{
	BOOL bRes;
	WINASSERTD(m_hDC != NULL);
	LASTERRORDISPLAYD(bRes = ::Rectangle(m_hDC, x1, y1, x2, y2));
	return bRes;
}

inline BOOL ODC::Ellipse(int x1, int y1, int x2, int y2)
{
	BOOL bRes;
	WINASSERTD(m_hDC != NULL);
	LASTERRORDISPLAYD(bRes = ::Ellipse(m_hDC, x1, y1, x2, y2));
	return bRes;
}

inline POINT ODC::MoveTo(int x, int y)
{
	POINT point;
	WINASSERTD(m_hDC != NULL);
	LASTERRORDISPLAYD(::MoveToEx(m_hDC,x,y,&point));
	return point;
}

inline BOOL ODC::LineTo(int x, int y)
{
	BOOL bRes;
	WINASSERTD(m_hDC != NULL);
	LASTERRORDISPLAYD(bRes = ::LineTo(m_hDC, x, y));
	return bRes;
}

inline int ODC::GetClipBox(LPRECT lpRect) const
{
	WINASSERTD(m_hDC);
	int iRes;
	LASTERRORDISPLAYD((iRes =::GetClipBox(m_hDC, lpRect)) != ERROR);
	return iRes;
}

inline void ODC::FillSolidRect(LPCRECT lpRect, COLORREF clr)
{
	SetBkColor(clr);
	ExtTextOut(0, 0, ETO_OPAQUE, lpRect, NULL, 0, NULL);
}

inline void ODC::FillSolidRect(int x, int y, int cx, int cy, COLORREF clr)
{
	SetBkColor(clr);
	RECT rect;
	rect.left   = x;
	rect.top    = y;
	rect.right  = x + cx;
	rect.bottom = y + cy;
	ExtTextOut(0, 0, ETO_OPAQUE, &rect, NULL, 0, NULL);
}

inline BOOL ODC::BitBlt(int x, int y, int nWidth, int nHeight, OIC *pSrcDC,
						int xSrc, int ySrc, DWORD dwRop)
{
	BOOL bRes;
	ASSERT(m_hDC != NULL);
	LASTERRORDISPLAYD(bRes = ::BitBlt(m_hDC, x, y, nWidth, nHeight,
	                                  pSrcDC->GetSafeHdc(), xSrc,
									  ySrc, dwRop));
	return bRes;
}

inline int ODC::ExcludeClipRect(int x1, int y1, int x2, int y2)
{
	ASSERT(m_hDC != NULL);
	int nRetVal = ::ExcludeClipRect(m_hDC, x1, y1, x2, y2);
	return nRetVal;
}

inline int ODC::ExcludeClipRect(LPCRECT lpRect)
{
	ASSERT(m_hDC != NULL);
	int nRetVal = ::ExcludeClipRect(m_hDC, lpRect->left, lpRect->top,
			lpRect->right, lpRect->bottom);
	return nRetVal;
}

inline int ODC::IntersectClipRect(int x1, int y1, int x2, int y2)
{
	ASSERT(m_hDC != NULL);
	int nRetVal = ::IntersectClipRect(m_hDC, x1, y1, x2, y2);
	return nRetVal;
}

inline int ODC::IntersectClipRect(LPCRECT lpRect)
{
	ASSERT(m_hDC != NULL);
	int nRetVal = ::IntersectClipRect(m_hDC,
		                              lpRect->left, lpRect->top,
									  lpRect->right, lpRect->bottom);
	return nRetVal;
}

typedef DWORD (CALLBACK* _GDIGETLAYOUTPROC)(HDC);
typedef DWORD (CALLBACK* _GDISETLAYOUTPROC)(HDC, DWORD);

inline DWORD ODC::GetLayout() const
{
	ASSERT(m_hDC != NULL);

	HINSTANCE hInst = ::GetModuleHandleA("GDI32.DLL");
	ASSERT(hInst != NULL);

	DWORD dwGetLayout = LAYOUT_LTR;

	_GDIGETLAYOUTPROC pfn;
	pfn = (_GDIGETLAYOUTPROC)GetProcAddress(hInst, "GetLayout");

	// if they API is available, just call it. If it is not
	// available, indicate an error.

	if (pfn != NULL)
		dwGetLayout = (*pfn)(m_hDC);
	else
	{
		dwGetLayout = GDI_ERROR;
		SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
	}

	return dwGetLayout;
}

inline DWORD ODC::SetLayout(DWORD dwSetLayout)
{
	ASSERT(m_hDC != NULL);

	HINSTANCE hInst = ::GetModuleHandleA("GDI32.DLL");
	ASSERT(hInst != NULL);

	DWORD dwGetLayout = LAYOUT_LTR;

	_GDISETLAYOUTPROC pfn;
	pfn = (_GDISETLAYOUTPROC) GetProcAddress(hInst, "SetLayout");

	// If the API is availalbe, just call it. If it's not available,
	// setting anything other than LAYOUT_LTR is an error.

	if (pfn != NULL)
		dwGetLayout = (*pfn)(m_hDC, dwSetLayout);
	else if (dwSetLayout != LAYOUT_LTR)
	{
		dwGetLayout = GDI_ERROR;
		SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
	}

	return dwGetLayout;
}


// OMetaFileDC

inline BOOL OMetaFileDC::CreateEnhanced( OIC *pIC, // handle to reference DC
                            LPCTSTR lpFilename,    // file name
                            CONST RECT *lpRect,    // bounding rectangle
                            LPCTSTR lpDescription  // description string
					       )
{
	LASTERRORDISPLAYD(m_hDC = ::CreateEnhMetaFile(
			pIC->GetSafeHdc(),lpFilename,lpRect,lpDescription));
	return m_hDC!=NULL;
}

inline HENHMETAFILE OMetaFileDC::CloseEnhanced(void)
{
	HENHMETAFILE h;
	ASSERT(m_hDC != NULL);
	restoreSelection();
    LASTERRORDISPLAYD(h = ::CloseEnhMetaFile(m_hDC));
	m_hDC = NULL;
	return h;
}

// int    width       width of memory bitmap
// int    height      height of memory bitmap
// ODC    *pOrigDC    DC to make bitmap compatible with (default NULL = screen)
// COLORREF background  background color (default white)

inline OBitmapDC::OBitmapDC(int width,
							int height,
							ODC *pOrigDC /* = NULL */,
                            COLORREF background /* = RGB(255,255,255) */)
:	ODC(),
    m_pBitmap(NULL),
    m_pOldBmp(NULL)
{
  // create new bitmap
  OWindowDC dc(NULL); // screen

  if (!pOrigDC) {
	 pOrigDC = &dc;
  }

  m_pBitmap = new OBitmap();
  m_pBitmap->CreateCompatibleBitmap(pOrigDC, width, height);

  // make compatible device context and select bitmap into it
  CreateCompatibleDC(pOrigDC);
  m_pOldBmp = (HBITMAP)select(SELBITMAP, m_pBitmap);
  ODC::SetMapMode(pOrigDC->GetMapMode());

  // paint background in bitmap
  FillSolidRect(0, 0, width, height, background);
}



// draws bitmap at specified point in specified device context
// const OBitmap *bitmap  bitmap to draw
// const ODC     *pDC     device context to draw in
// const POINT   &point   top left point of bitmap

inline void OBitmapDC::DrawBitmap(OBitmap *bitmap,
								  ODC *pDC,
								  const int32_t x, const int32_t y)
{
  // determine bitmap size
  BITMAP bm;
  bitmap->GetObject(bm);
  int w = bm.bmWidth;
  int h = bm.bmHeight;

  // create memory device context
  OMemDC memDC(bitmap, pDC);
  memDC.SetMapMode(pDC->GetMapMode());

  // blit bitmap to specified device context
  pDC->BitBlt(x, y, w, h, &memDC, 0, 0, SRCCOPY);
}

// draws bitmap centered in specified rectangle in specified device context
// const OBitmap *bitmap  bitmap to draw
// const ODC     *pDC     device context to draw in
// const CRect   &rect    rectangle to center in
// bool          stretch  stretch? (default false)
//----------------------------------------------------------------------------

inline void OBitmapDC::DrawBitmap(OBitmap *bitmap,
								  ODC *pDC,
								  const RECT &rect,
								  bool stretch /* = false */)
{
  // determine bitmap size
  BITMAP bm;
  bitmap->GetObject(bm);
  int w = bm.bmWidth;
  int h = bm.bmHeight;

  if (stretch)
  {
    // create memory device context
    OMemDC memDC(bitmap, pDC);
	memDC.SetMapMode(pDC->GetMapMode());

    memDC.SetMapMode(pDC->GetMapMode());
    // blit bitmap to specified device context with stretching
	::SetStretchBltMode(pDC->GetSafeHdc(), HALFTONE);
	int nWidth  = rect.right  - rect.left;
	int nHeight = rect.bottom - rect.top;
    ::StretchBlt(pDC->GetSafeHdc(), rect.left, rect.top, nWidth, nHeight,
				memDC.GetSafeHdc(),
                            0, 0, w, h, SRCCOPY);
  }
  else
  { // !stretch
    POINT point;
    point.x = rect.left + ((rect.right - rect.left) / 2) - (w / 2);
    point.y = rect.top + ((rect.bottom - rect.top) / 2) - (h / 2);
    DrawBitmap(bitmap, pDC, point.x, point.y);
  }
}

// destructor, destroys bitmap
// if it has not been released with OBitmapDC::GetFinalBitmap

inline OBitmapDC::~OBitmapDC()
{
  if (m_pBitmap)
  {
    OBitmap *pBitmap;
    pBitmap = Close();
    delete pBitmap;
    pBitmap = NULL;
  }
}

// returns pointer to memory bitmap and prevents further use
// NOTE: The programmer is now responsible for deleting this bitmap
// pointer to memory bitmap (OBitmap*) (to be deleted by caller)

inline OBitmap *OBitmapDC::Close()
{
  WINASSERTD(m_pBitmap != NULL);
  OBitmap *pBitmap = m_pBitmap;
  select(SELBITMAP, m_pOldBmp);
  m_pBitmap = nullptr;
  return pBitmap;
}

namespace BitmapUtilSpace
{
	static unsigned int LINES_PER_BAND = 100;

	enum
	{
		blUnsupportedCompression = -1,
		blColorOverflow = -2,
		blNotEnoughMemory = -3,
	};

	static RGBQUAD GetRGB( BITMAPINFO const *pBmi,
                           UINT const      uLine,
                           UINT const      uCol,
                           unsigned char const     *pBits)
	{
	  BITMAPINFOHEADER const * pBmih = &pBmi->bmiHeader;
	  RGBQUAD const * pRgb = pBmi->bmiColors;
	  RGBQUAD rgb = { 0, 0, 0, 0 };

	  DWORD dwBytesPerLine = pBmih->biSizeImage / pBmih->biHeight;

	  switch ( pBmih->biBitCount )
	  {
		case 1:
		  {
			DWORD dwBytePos = uLine * dwBytesPerLine + ( uCol >> 3 );

			WORD wIndex = pBits[ dwBytePos ];
			wIndex = ( WORD ) ( ( wIndex >> ~( ( BYTE ) uCol & 0x7 ) ) & 0x1 );

			rgb = pRgb[wIndex];
		  }
		break;

		case 4:
		  {
			DWORD dwBytePos = uLine * dwBytesPerLine + ( uCol >> 1 );

			WORD wIndex = pBits[ dwBytePos ];
			wIndex = ( WORD ) ( ( wIndex >> ( ( ( ( BYTE ) uCol & 0x1 ) ^ 0x1 ) << 2 ) ) & 0xf );

			rgb = pRgb[wIndex];
		  }
		break;

		case 8:
		  rgb = pRgb[ pBits[ uLine * dwBytesPerLine + uCol ] ];
		break;

		case 16:
		  {
			WORD wCol = ( ( WORD * ) pBits )[ uLine * dwBytesPerLine / 2 + uCol ];

			rgb.rgbRed   = ( BYTE ) ( ( wCol >> 10 ) & 0x1f );
			rgb.rgbGreen = ( BYTE ) ( ( wCol >> 5 ) & 0x1f );
			rgb.rgbBlue  = ( BYTE ) ( wCol & 0x1f );
			rgb.rgbReserved = 0;
		  }
		break;

		case 24:
		  rgb.rgbRed   = pBits[ uLine * dwBytesPerLine + uCol * 3 + 2 ];
		  rgb.rgbGreen = pBits[ uLine * dwBytesPerLine + uCol * 3 + 1 ];
		  rgb.rgbBlue  = pBits[ uLine * dwBytesPerLine + uCol * 3 ];
		  rgb.rgbReserved = 0;
		break;

		case 32:
		  {
			DWORD dwCol = ( ( DWORD * ) pBits )[ uLine * dwBytesPerLine / 4 + uCol ];

			rgb.rgbRed   = ( BYTE ) ( ( dwCol >> 16 ) & 0xff );
			rgb.rgbGreen = ( BYTE ) ( ( dwCol >> 8 ) & 0xff );
			rgb.rgbBlue  = ( BYTE ) ( dwCol & 0xff );
			rgb.rgbReserved = 0;
		  }
		break;

		default:
		  WINASSERTD(FALSE);
		  // error: unsupported bitmap format, bitcount
		break;
	  }

	  return rgb;
	}

	static int GetUsedColors( BITMAPINFO const * pBmi,
                          BYTE const *       pBits,
                          UINT const         nStartScanLine,
                          UINT const         nScanLines,
                          RGBQUAD *          pColorTable,
                          WORD const         wMaxColorTableSize,
                          WORD &             wUsedColors )
	{
	  ASSERT( pBmi );
	  ASSERT( pBits );
	  ASSERT( pColorTable );

	  BITMAPINFOHEADER const * pBmih = &pBmi->bmiHeader;
	  RGBQUAD rgb;

	  if ( pBmih->biCompression != BI_RGB )
		return blUnsupportedCompression;  // unsupported bitmap format, no compression

	  for ( UINT i = nStartScanLine; i < nStartScanLine + nScanLines; i++ )
	  {
		for ( UINT j = 0; j < ( UINT ) pBmih->biWidth; j++ )
		{
		  rgb = GetRGB(pBmi, i, j, pBits );

		  // search color table for current color
		  WORD k = 0;
		  for ( ; k < wUsedColors; k++ )
			if ( pColorTable[k].rgbBlue == rgb.rgbBlue
				 && pColorTable[k].rgbGreen == rgb.rgbGreen
				 && pColorTable[k].rgbRed == rgb.rgbRed )
			  break;

		  // if color could not be found
		  if ( k == wUsedColors )
		  {
			// table full, no more entries and thus color count possible
			if ( wUsedColors == wMaxColorTableSize )
			  return blColorOverflow;            // error: color table overflow
			else
			{
			  // append new color at color table
			  pColorTable[wUsedColors] = rgb;
			  wUsedColors++;
			}
		  }
		}
	  }

	  return wUsedColors;
	}

	static void SetDIBPixel(BITMAPINFO const * pBmi,
                            UINT const         uLine,
                            UINT const         uCol,
                            BYTE *             pBits,
                            BYTE const         sPixelVal )
	{
	  BITMAPINFOHEADER const * pBmih = &pBmi->bmiHeader;

	  DWORD dwBytesPerLine = pBmih->biSizeImage / pBmih->biHeight;

	  switch ( pBmih->biBitCount )
	  {
		// device independent bitmaps
		case 1:
		  {
			DWORD dwBytePos = uLine * dwBytesPerLine + ( uCol >> 3 );

			pBits[ dwBytePos ] |= ( sPixelVal & 0x1 ) << ~( ( BYTE ) uCol & 0x7 );
		  }
		break;

		case 4:
		  {
			DWORD dwBytePos = uLine * dwBytesPerLine + ( uCol >> 1 );

			if ( uCol % 2 == 1 )
			  pBits[ dwBytePos ] |= ( sPixelVal & 0xf );
			else
			  pBits[ dwBytePos ] |= ( sPixelVal & 0xf ) << 4;
		  }
		break;

		case 8:
		  pBits[ uLine * dwBytesPerLine + uCol ] = ( BYTE ) ( sPixelVal & 0xff );
		break;

		// device dependent bitmaps
		case 16:
		case 24:
		case 32:
		  WINASSERTD( FALSE );
		break;

		default:
		  WINASSERTD( FALSE );
		  // error: unsupported bitmap format, bitcount
		break;
	  }
	}

	static void SetRGBPixel( BITMAPINFO const * pBmi,
                         UINT const         uLine,
                         UINT const         uCol,
                         BYTE *             pBits,
                         RGBQUAD            rgbPixel )
	{
	  BITMAPINFOHEADER const * pBmih = &pBmi->bmiHeader;

	  DWORD dwBytesPerLine = pBmih->biSizeImage / pBmih->biHeight;

	  switch ( pBmih->biBitCount )
	  {
		// device independent bitmaps
		case 1:
		case 4:
		case 8:
		  WINASSERTD( FALSE );
		break;

		// device dependent bitmaps
		case 16:
		  {
			// set rgb ranges to 5 bit
			rgbPixel.rgbBlue  >>= 3;//= min( rgbPixel.rgbBlue , 0x1f );
			rgbPixel.rgbGreen >>= 3;//= min( rgbPixel.rgbGreen, 0x1f );
			rgbPixel.rgbRed   >>= 3;//= min( rgbPixel.rgbRed  , 0x1f );

			WORD wColVal = ( WORD ) (  ( WORD ) rgbPixel.rgbBlue << 10
									 | ( WORD ) rgbPixel.rgbGreen << 5
									 | ( WORD ) rgbPixel.rgbRed );
			( ( WORD * ) pBits )[ uLine * dwBytesPerLine / 2 + uCol ] = wColVal;
		  }
		break;

		case 24:
		  pBits[ uLine * dwBytesPerLine + uCol * 3 + 2 ] = rgbPixel.rgbRed;
		  pBits[ uLine * dwBytesPerLine + uCol * 3 + 1 ] = rgbPixel.rgbGreen;
		  pBits[ uLine * dwBytesPerLine + uCol * 3 ]     = rgbPixel.rgbBlue;
		break;

		case 32:
		  {
			DWORD dwColVal =   ( DWORD ) rgbPixel.rgbRed << 16
							 | ( DWORD ) rgbPixel.rgbGreen << 8
							 | ( DWORD ) rgbPixel.rgbBlue;
			( ( DWORD * ) pBits )[ uLine * dwBytesPerLine / 4 + uCol ] = dwColVal;
		  }
		break;

		default:
		  WINASSERTD( FALSE );
		  // error: unsupported bitmap format, bitcount
		break;
	  }
	}

	static BOOL ChangeColorDepth(BITMAPINFO * pBmiSrc,
			  				 BYTE * pBitsSrc,
							 BITMAPINFO *& pBmiDst,
							 BYTE *& pBitsDst,
                             WORD const  wDstBitCount)
	{
	  // if bitcount is greater than 8 bits, no device independent bitmap is requested
	  BOOL bDstIsDIB = TRUE;
	  if ( wDstBitCount > 8 )
		bDstIsDIB = FALSE;

	  // create destination bitmapinfo header, due to wether it's a DIB or not (with or without color palette)
	  if ( bDstIsDIB )
	  {
		pBmiDst = ( BITMAPINFO * ) LocalAlloc(LPTR, sizeof( BITMAPINFOHEADER )
											 + ( 1ULL << wDstBitCount ) * sizeof( RGBQUAD ) );
		pBmiDst->bmiHeader = pBmiSrc->bmiHeader;
		pBmiDst->bmiHeader.biClrUsed = ( 1 << wDstBitCount );
		pBmiDst->bmiHeader.biClrImportant = ( 1 << wDstBitCount );
	  }
	  else
	  {
		pBmiDst = ( BITMAPINFO * ) LocalAlloc(LPTR,  sizeof( BITMAPINFOHEADER ) );
		pBmiDst->bmiHeader = pBmiSrc->bmiHeader;
		pBmiDst->bmiHeader.biClrUsed = 0;
		pBmiDst->bmiHeader.biClrImportant = 0;
	  }

	  pBmiDst->bmiHeader.biBitCount = wDstBitCount;
	  pBmiDst->bmiHeader.biSizeImage = ( ( ( ( pBmiDst->bmiHeader.biWidth
											   * pBmiDst->bmiHeader.biBitCount ) + 31 ) & ~31 ) >> 3 )
									   * pBmiDst->bmiHeader.biHeight;

	  // init color table in case of a DIB
	  if ( bDstIsDIB )
		for ( WORD j = 0; j < pBmiDst->bmiHeader.biClrUsed; j++ )
		{
		  pBmiDst->bmiColors[j].rgbBlue     = 255;
		  pBmiDst->bmiColors[j].rgbGreen    = 255;
		  pBmiDst->bmiColors[j].rgbRed      = 255;
		  pBmiDst->bmiColors[j].rgbReserved = 0;
		}

	  WORD wColorTableSize = 0;     // only used for bDstIsDIB = TRUE

	  UINT  uSrcBytesPerLine = pBmiSrc->bmiHeader.biSizeImage / pBmiSrc->bmiHeader.biHeight;
	  UINT  uDstBytesPerLine = pBmiDst->bmiHeader.biSizeImage / pBmiDst->bmiHeader.biHeight;

	  // only reserve memory for small bitmap bands, thus working also on fat sized bitmaps

	  pBitsDst = new BYTE[ uDstBytesPerLine * BitmapUtilSpace::LINES_PER_BAND ];

	  // perform each band in a loop
	  unsigned char *pBits = (unsigned char *)GlobalAlloc(GMEM_FIXED, pBmiDst->bmiHeader.biSizeImage);

	  int nTime = 0;

	  // perform each band in a loop
	  for ( UINT i = 0; i < ( UINT ) pBmiSrc->bmiHeader.biHeight; i += BitmapUtilSpace::LINES_PER_BAND )
	  {
		  // uCurLines may be smaller at the last loop
          UINT uCurLines = BitmapUtilSpace::LINES_PER_BAND < pBmiSrc->bmiHeader.biHeight - i ? 
              BitmapUtilSpace::LINES_PER_BAND : pBmiSrc->bmiHeader.biHeight - i;

		// build up destination color palette at a DIB
		if ( bDstIsDIB )
		  if ( GetUsedColors( pBmiSrc, pBitsSrc, 0, uCurLines,
							  pBmiDst->bmiColors, ( WORD ) pBmiDst->bmiHeader.biClrUsed, wColorTableSize ) < 0 )
		  {
			GlobalFree(pBits);
			delete [] pBitsDst;
			LocalFree(pBmiDst);
			return FALSE;
		  }

		// init destination buffer (needed for bitmaps smaller 8bit bitcount)
		memset( pBitsDst, 0, uCurLines * uDstBytesPerLine );

		// loops over each pixel
		for (UINT m = 0; m < uCurLines; m++ )
		{
		  for (UINT n = 0; n < ( UINT ) pBmiSrc->bmiHeader.biWidth; n++ )
		  {
			RGBQUAD rgb = GetRGB( pBmiSrc, m, n, pBitsSrc );

			if ( bDstIsDIB )
			{
              WORD k = 0;
			  for ( ; k < wColorTableSize; k++ )
			  {
				if ( pBmiDst->bmiColors[k].rgbBlue == rgb.rgbBlue
					 && pBmiDst->bmiColors[k].rgbGreen == rgb.rgbGreen
					 && pBmiDst->bmiColors[k].rgbRed == rgb.rgbRed )
				{
				  // sets a DIB pixel
				  SetDIBPixel( pBmiDst, m, n, pBitsDst, ( BYTE ) k );
				  break;
				}
			  }
			  WINASSERTD(k < wColorTableSize);
			}
			else
			  // sets a high or true color pixel
			  SetRGBPixel(pBmiDst, m, n, pBitsDst, rgb);
		  }
		}
		int nSize = (uCurLines * uDstBytesPerLine);
		memcpy(pBits + nTime, pBitsDst, nSize);
		pBitsSrc = pBitsSrc + uCurLines * uSrcBytesPerLine;
		nTime = nTime + nSize;
	  }

	  delete [] pBitsDst;
	  pBitsDst = pBits;
	  return TRUE;
	}

	struct WINDOW_COLORMAP
	{
		// use DWORD instead of RGBQUAD so we can compare two RGBQUADs easily
		DWORD rgbqFrom;
		int iSysColorTo;
	};
};


inline void OBitmapUtil::CopyBitmapToClipboard(const HWND hWnd,
										OBitmap& bitmap,
										OPalette* pPal )
{
	::OpenClipboard(hWnd);
	::EmptyClipboard() ;

	if( pPal )
	{
		::SetClipboardData (CF_PALETTE, pPal->GetSafeHandle() ) ;
	}

	::SetClipboardData (CF_BITMAP, bitmap.GetSafeHandle() ) ;
	::CloseClipboard () ;
}

inline PBITMAPINFO OBitmapUtil::CreateBitmapInfoStruct(HBITMAP hBmp)
{
    BITMAP bmp;
    PBITMAPINFO pbmi;
    WORD    cClrBits;

    // Retrieve the bitmap's color format, width, and height.
    if (!GetObject(hBmp, sizeof(BITMAP), (char *)&bmp))
        return NULL;

    // Convert the color format to a count of bits.
    cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel);
    if (cClrBits == 1)
        cClrBits = 1;
    else if (cClrBits <= 4)
        cClrBits = 4;
    else if (cClrBits <= 8)
        cClrBits = 8;
    else if (cClrBits <= 16)
        cClrBits = 16;
    else if (cClrBits <= 24)
        cClrBits = 24;
    else cClrBits = 32;

    // Allocate memory for the BITMAPINFO structure. (This structure
    // contains a BITMAPINFOHEADER structure and an array of RGBQUAD
    // data structures.)

	if (cClrBits != 24) {
		 int size = sizeof(BITMAPINFOHEADER) +
                       sizeof(RGBQUAD) * (1ULL << cClrBits);
         pbmi = (PBITMAPINFO)LocalAlloc(LPTR, size);

     // There is no RGBQUAD array for the 24-bit-per-pixel format.
	}
	else {
         pbmi = (PBITMAPINFO)LocalAlloc(LPTR, sizeof(BITMAPINFOHEADER));
	}
    // Initialize the fields in the BITMAPINFO structure.

    pbmi->bmiHeader.biSize     = sizeof(BITMAPINFOHEADER);
    pbmi->bmiHeader.biWidth    = bmp.bmWidth;
    pbmi->bmiHeader.biHeight   = bmp.bmHeight;
    pbmi->bmiHeader.biPlanes   = bmp.bmPlanes;
    pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel;

    if (cClrBits < 24) {
        pbmi->bmiHeader.biClrUsed = (1<<cClrBits);
	}
    // If the bitmap is not compressed, set the BI_RGB flag.
    pbmi->bmiHeader.biCompression = BI_RGB;
	/**
	 * Compute the number of bytes in the array of color
     * indices and store the result in biSizeImage.
     * For Windows NT/2000, the width must be DWORD aligned unless
     * the bitmap is RLE compressed. This example shows this.
     * For Windows 95/98, the width must be WORD aligned unless the
     * bitmap is RLE compressed.
	 */
    pbmi->bmiHeader.biSizeImage = ((pbmi->bmiHeader.biWidth * cClrBits +31) & ~31) /8
                                  * pbmi->bmiHeader.biHeight;
    // Set biClrImportant to 0, indicating that all of the
    // device colors are important.
     pbmi->bmiHeader.biClrImportant = 0;

     return pbmi;
}

#include <stdlib.h> // malloc

inline unsigned char *OBitmapUtil::CreateBitmapFileBinary(OBitmap *pBmp, long &size)
{
	/**
	 * Bitmap file structure:
	 * 1. BITMAPFILEHEADER    - the type, size, and layout of a file
	 * 2. BITMAPINFO
	 *   2.1 BITMAPINFOHEADER - information about the dimensions and color format
	 *   2.2 RGBQUAD          - defines the colors in the bitmap
	 * 3. Data (lpBits)       - array for bitmap bits
	 */

	WINASSERTD(pBmp != NULL);

	HBITMAP hBitmap = (HBITMAP)pBmp->GetSafeHandle();

	OWindowDC dc(NULL); // Fake DC

	BITMAPINFO    *info   = CreateBitmapInfoStruct(hBitmap);
	unsigned char *lpBits = (unsigned char *)GlobalAlloc(GMEM_FIXED, info->bmiHeader.biSizeImage);
	BITMAP    bitmap;
	pBmp->GetObject(bitmap);

	GetDIBits(dc.GetSafeHdc(), hBitmap, 0, bitmap.bmHeight, lpBits, info, DIB_RGB_COLORS);

	BITMAPINFO    *pBmiDst  = NULL;
	unsigned char *pBitsDst = NULL;

	if ( info->bmiHeader.biBitCount != 16 )
	{   // doesn't work with 16bps bitmaps
		if ( BitmapUtilSpace::ChangeColorDepth(info, lpBits, pBmiDst, pBitsDst, 4) )
		{
			LocalFree(info);
			GlobalFree(lpBits);

			info    = pBmiDst;
			lpBits  = pBitsDst;
		}
	}

	long color_size = info->bmiHeader.biClrUsed*sizeof(RGBQUAD);

	// All bitmap size
	size = sizeof(BITMAPFILEHEADER) + info->bmiHeader.biSize +
		   color_size + info->bmiHeader.biSizeImage;

	BITMAPFILEHEADER	header;		/* File header */
	header.bfType      = 0x4d42;   // 'BM' WINDOWS_BITMAP_SIGNATURE - 0x42 = "B" 0x4d = "M"
	header.bfSize      = size;
	header.bfReserved1 = 0;
	header.bfReserved2 = 0;
	header.bfOffBits   = size - info->bmiHeader.biSizeImage;

	// Compute the offset to the array of color indices.
	header.bfOffBits   = sizeof(BITMAPFILEHEADER) + info->bmiHeader.biSize + color_size;

	unsigned char *pData = (unsigned char *)malloc(size);

	memcpy(pData, &header, sizeof(BITMAPFILEHEADER) );
	memcpy(pData + sizeof(BITMAPFILEHEADER), info, color_size + sizeof(BITMAPINFOHEADER));
	memcpy(pData + sizeof(BITMAPFILEHEADER) + color_size + sizeof(BITMAPINFOHEADER), lpBits, info->bmiHeader.biSizeImage);

	GlobalFree(lpBits);
	LocalFree(info);

	return pData;
}

inline bool OBitmapUtil::CopyBitmap(OBitmap &bmpSrc, OBitmap &bmpDst)
{
	if ((HBITMAP)bmpDst != NULL)

	if (bmpSrc.GetSafeHandle() == NULL)
	{
		return false;
	}

	HBITMAP hDst = (HBITMAP)::CopyImage((HBITMAP)bmpSrc, IMAGE_BITMAP, 0, 0, 0);

	if (hDst == NULL)
	{
		return false;
	}

	bmpDst.SetBitmap(hDst, TRUE);

	return true;
}

/*  Mircrosoft:
	DIBs use RGBQUAD format:
		0xbb 0xgg 0xrr 0x00

	Reasonably efficient code to convert a COLORREF into an
	RGBQUAD is byte-order-dependent, so we need different
	code depending on the byte order we're targeting.
*/
#ifndef RGB_TO_RGBQUAD
	#define RGB_TO_RGBQUAD(r,g,b)   (RGB(b,g,r))
#endif

#ifndef CLR_TO_RGBQUAD
	#define CLR_TO_RGBQUAD(clr)     (RGB(GetBValue(clr), GetGValue(clr), GetRValue(clr)))
#endif

inline HBITMAP OBitmapUtil::LoadSysColorBitmap(HINSTANCE hInst, HRSRC hRsrc, BOOL bMono)
{
	HGLOBAL hglb;

	if ((hglb = LoadResource(hInst, hRsrc)) == NULL)
	{
		return NULL;
	}

	LPBITMAPINFOHEADER lpBitmap = (LPBITMAPINFOHEADER)LockResource(hglb);

	if (lpBitmap == NULL)
	{
		return NULL;
	}

	// make copy of BITMAPINFOHEADER so we can modify the color table
	const int nColorTableSize = 16;
	UINT nSize = lpBitmap->biSize + nColorTableSize * sizeof(RGBQUAD);
	LPBITMAPINFOHEADER lpBitmapInfo = (LPBITMAPINFOHEADER)::malloc(nSize);
	if (lpBitmapInfo == NULL)
		return NULL;
	memcpy(lpBitmapInfo, lpBitmap, nSize);

	static const BitmapUtilSpace::WINDOW_COLORMAP _sysColorMap[] =
	{
		// mapping from color in DIB to system color
		{ RGB_TO_RGBQUAD(0x00, 0x00, 0x00),  COLOR_BTNTEXT },       // black
		{ RGB_TO_RGBQUAD(0x80, 0x80, 0x80),  COLOR_BTNSHADOW },     // dark gray
		{ RGB_TO_RGBQUAD(0xC0, 0xC0, 0xC0),  COLOR_BTNFACE },       // bright gray
		{ RGB_TO_RGBQUAD(0xFF, 0xFF, 0xFF),  COLOR_BTNHIGHLIGHT }   // white
	};

	// color table is in RGBQUAD DIB format
	DWORD* pColorTable =
		(DWORD*)(((LPBYTE)lpBitmapInfo) + (UINT)lpBitmapInfo->biSize);

	for (int iColor = 0; iColor < nColorTableSize; iColor++)
	{
		// look for matching RGBQUAD color in original
		for (int i = 0; i < _countof(_sysColorMap); i++)
		{
			if (pColorTable[iColor] == _sysColorMap[i].rgbqFrom)
			{
				if (bMono)
				{
					// all colors except text become white
					if (_sysColorMap[i].iSysColorTo != COLOR_BTNTEXT)
						pColorTable[iColor] = RGB_TO_RGBQUAD(255, 255, 255);
				}
				else
					pColorTable[iColor] =
						CLR_TO_RGBQUAD(::GetSysColor(_sysColorMap[i].iSysColorTo));
				break;
			}
		}
	}

	int nWidth = (int)lpBitmapInfo->biWidth;
	int nHeight = (int)lpBitmapInfo->biHeight;
	HDC hDCScreen = ::GetDC(NULL);
	HBITMAP hbm = ::CreateCompatibleBitmap(hDCScreen, nWidth, nHeight);

	if (hbm != NULL)
	{
		HDC hDCGlyphs = ::CreateCompatibleDC(hDCScreen);
		HBITMAP hbmOld = (HBITMAP)::SelectObject(hDCGlyphs, hbm);

		LPBYTE lpBits;
		lpBits = (LPBYTE)(lpBitmap + 1);
		lpBits += (1ULL << (lpBitmapInfo->biBitCount)) * sizeof(RGBQUAD);

		StretchDIBits(hDCGlyphs, 0, 0, nWidth, nHeight, 0, 0, nWidth, nHeight,
			lpBits, (LPBITMAPINFO)lpBitmapInfo, DIB_RGB_COLORS, SRCCOPY);
		SelectObject(hDCGlyphs, hbmOld);
		::DeleteDC(hDCGlyphs);
	}
	::ReleaseDC(NULL, hDCScreen);

	// free copy of bitmap info struct and resource itself
	::free(lpBitmapInfo);
	::FreeResource(hglb);

	return hbm;
}

inline BOOL ODC::ODrawState(POINT pt, SIZE size, HICON hIcon, UINT nFlags,
							HBRUSH hBrush)
{
	return ::DrawState(m_hDC, hBrush, NULL,
	(LPARAM)hIcon, 0, pt.x, pt.y, size.cx, size.cy, nFlags|DST_ICON);
}

inline  BOOL ODC::ODrawState(POINT pt, SIZE size, LPCTSTR lpszText, UINT nFlags,
						     BOOL bPrefixText, int nTextLen, HBRUSH hBrush)
{
	return ::DrawState(m_hDC, hBrush,
	NULL, (LPARAM)lpszText, (WPARAM)nTextLen, pt.x, pt.y, size.cx, size.cy, nFlags|(bPrefixText ? DST_PREFIXTEXT : DST_TEXT));
}

inline void ODC::DrawDragRect(LPCRECT lpRect, SIZE size,
							  LPCRECT lpRectLast,
							  SIZE sizeLast,
							  OBrush *pBrush,
						      OBrush *pBrushLast)
{
	// first, determine the update region and select it
	HRGN rgnOutside = ::CreateRectRgnIndirect(lpRect);

	RECT rect = *lpRect;
	::InflateRect(&rect, -size.cx, -size.cy);
	::IntersectRect(&rect, &rect, lpRect);

	HRGN rgnInside = ::CreateRectRgnIndirect(&rect);
	HRGN rgnNew    = ::CreateRectRgn(0, 0, 0, 0);


	::CombineRgn(rgnNew, rgnOutside,
		rgnInside, RGN_XOR);

	HBRUSH pBrushOld = NULL;
	if (pBrush == NULL)
		pBrush = OBrush::GetHalftoneBrush();
	if (pBrushLast == NULL)
		pBrushLast = pBrush;

	//, rgnUpdate;
	// find difference between new region and old region
	HRGN rgnLast = ::CreateRectRgn(0, 0, 0, 0);
	::SetRectRgn(rgnOutside, lpRectLast->left, lpRectLast->top,
				 lpRectLast->right, lpRectLast->bottom);

	rect = *lpRectLast;
	::InflateRect(&rect, -sizeLast.cx, -sizeLast.cy);
	::IntersectRect(&rect, &rect, lpRectLast);

	::SetRectRgn(rgnInside, rect.left, rect.top,
	rect.right, rect.bottom);

	::CombineRgn(rgnLast, rgnOutside, rgnInside, RGN_XOR);

	// only diff them if brushes are the same
	HRGN rgnUpdate = NULL;
	if (pBrush->GetSafeHandle() == pBrushLast->GetSafeHandle())
	{
		rgnUpdate = ::CreateRectRgn(0, 0, 0, 0);
		::CombineRgn(rgnUpdate, rgnLast, rgnNew, RGN_XOR);
	}

	if (pBrush->GetSafeHandle() != pBrushLast->GetSafeHandle() && lpRectLast != NULL)
	{
		// brushes are different -- erase old region first
		SelectClipRgn(rgnLast);
		GetClipBox(&rect);
		pBrushOld = SelectObject(pBrushLast);
		int wt = rect.right  - rect.left;
		int ht = rect.bottom - rect.top;
		PatBlt(rect.left, rect.top, wt, ht, PATINVERT);
		SelectObject(pBrushOld);
		pBrushOld = NULL;
	}

	// draw into the update/new region
	SelectClipRgn(rgnUpdate != NULL ? rgnUpdate : rgnNew);
	GetClipBox(&rect);
	pBrushOld = SelectObject(pBrush);
	int wt1 = rect.right  - rect.left;
	int ht1 = rect.bottom - rect.top;
	PatBlt(rect.left, rect.top, wt1, ht1, PATINVERT);

	// cleanup DC
	if (pBrushOld != NULL)
	{
		SelectObject(pBrushOld);
	}

	::SelectClipRgn(m_hDC, NULL);

	::DeleteObject(rgnLast);
	::DeleteObject(rgnNew);
	::DeleteObject(rgnUpdate);
	::DeleteObject(rgnOutside);
	::DeleteObject(rgnInside);
}

inline OBrush *OBrush::GetHalftoneBrush()
{
	static OBrush _HalftoneBrush;

	if (_HalftoneBrush.GetSafeHandle() == NULL)
	{
		WORD grayPattern[8];
		for (int i = 0; i < 8; i++)
			grayPattern[i] = (WORD)(0x5555 << (i & 1));
		HBITMAP grayBitmap = CreateBitmap(8, 8, 1, 1, &grayPattern);

		if (grayBitmap != NULL)
		{
			_HalftoneBrush.set(::CreatePatternBrush(grayBitmap), TRUE);
			DeleteObject(grayBitmap);
		}
	}

	return &_HalftoneBrush;
}

/******************************************************************************
 * OPaintDC functions definitions
 */

/*
 * Constructor/Destructor
 */
inline OPaintDC::~OPaintDC(void)
{
	restoreSelection();
	::EndPaint(m_hWnd, &m_ps);
	m_hDC = NULL;
}

/******************************************************************************
 * OFlickerFreeDC functions definitions
 */

/*
 * Constructor/Destructor
 */
inline OFlickerFreeDC::~OFlickerFreeDC(void)
{
	// Copy the offscreen bitmap onto the screen.
	m_pDC->BitBlt(m_rect.left, m_rect.top, m_rect.right-m_rect.left,
		          m_rect.bottom-m_rect.top,
				  this, m_rect.left, m_rect.top, SRCCOPY);

	/*
	 * Swap back the original bitmap.
	 * It must be done explicitly because when restoreSelection()
	 * will be called in the base class destructor, m_bitmap will
	 * not exist anymore.
	 */
	restoreSelection(SELBITMAP);
}

/******************************************************************************
 * OClientDC functions definitions
 */

/*
 * Constructor/Destructor
 */
inline OClientDC::~OClientDC(void)
{
	restoreSelection();
	LASTERRORDISPLAYD(::ReleaseDC(m_hWnd, m_hDC));
	m_hDC = NULL;
}



/******************************************************************************
 * OMetaFileDC functions definitions
 */

/*
 * Constructor/Destructor
 */
inline OMetaFileDC::~OMetaFileDC(void)
{
	if( m_hDC )
	{
		//"Warning! Destroying CMetaFileDC without closing.\n";
		HENHMETAFILE hmfTemp = CloseEnhanced();
		LASTERRORDISPLAYD(::DeleteEnhMetaFile(hmfTemp));
	}
}

