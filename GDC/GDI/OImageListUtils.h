#ifndef __O_IMAGE_LIST_UTILS_H__
#define __O_IMAGE_LIST_UTILS_H__
#pragma once

#ifndef _OLIGDI_H_
	#include "Ctrl/GDC/GDI/oligdi.h"
#endif

namespace o_image_list_utils
{
	inline void SetTreeCtrlImageList(HWND hWnd, OImageList* pImageList, int nImageListType)
	{
		ASSERT(::IsWindow(hWnd));
        #pragma warning(suppress : 6011) // GetSafeHandle does the "right" thing even when called on a null pointer, so the warning is spurious.
		::SendMessage(hWnd, TVM_SETIMAGELIST, (WPARAM)nImageListType, (LPARAM)pImageList->GetSafeHandle());
	}

	inline void SetComboBoxExImageList(HWND hWnd, OImageList* pImageList)
	{
		ASSERT(::IsWindow(hWnd));
		::SendMessage(hWnd, CBEM_SETIMAGELIST, 0, (LPARAM)pImageList->GetSafeHandle());
	}

	inline void SetListCtrlImageList(HWND hWnd, OImageList* pImageList, int nImageList)
	{
		ASSERT(::IsWindow(hWnd));
		::SendMessage(hWnd, LVM_SETIMAGELIST, nImageList, (LPARAM)pImageList->GetSafeHandle());
	}

	inline void SetTabCtrlImageList(HWND hWnd, OImageList* pImageList)
	{
		ASSERT(::IsWindow(hWnd));
		::SendMessage(hWnd, TCM_SETIMAGELIST, 0, (LPARAM)pImageList->GetSafeHandle());
	}
};

#endif