#include "stdafx.h"

#include "UISDK\Kernel\Inc\Interface\iwindow.h"
#include "UISDK\Kernel\Src\Renderlibrary\gdi\gdibitmap.h"
#include "UISDK\Kernel\Src\Renderlibrary\gdi\gdirender.h"
#include "UISDK\Kernel\Src\Renderlibrary\gdiplus\gdiplusbitmap.h"
#include "UISDK\Kernel\Src\Renderlibrary\gdiplus\gdiplusrender.h"
#include "UISDK\Kernel\Src\Base\Object\object.h"

//////////////////////////////////////////////////////////////////////////

// 
// IRenderFontListenerImpl::~IRenderFontListenerImpl()
// {
// 	m_listFontModifyListener.clear();
// }
// 
// void IRenderFontListenerImpl::AddModifyListener(Object* p) 
// {
// 	if (NULL == p)
// 		return;
// 
// 	list<Object*>::iterator iter = std::find(m_listFontModifyListener.begin(), m_listFontModifyListener.end(), p);
// 	if (iter == m_listFontModifyListener.end())
// 	{
// 		m_listFontModifyListener.push_back(p);
// 	}
// }
// void IRenderFontListenerImpl::RemoveModifyListener(Object* p) 
// {
// 	list<Object*>::iterator iter = std::find(m_listFontModifyListener.begin(), m_listFontModifyListener.end(), p);
// 	if (iter != m_listFontModifyListener.end())
// 	{
// 		m_listFontModifyListener.erase(iter);
// 	}
// }
// void IRenderFontListenerImpl::NotifyListener()
// {
// 	list<Object*>::iterator iter = m_listFontModifyListener.begin();
// 	list<Object*>::iterator iterEnd = m_listFontModifyListener.end();
// 	for (; iter != iterEnd; iter++)
// 	{
// 		UISendMessage(*iter, UI_WM_FONTMODIFIED, (WPARAM)static_cast<IRenderFont*>(this), 0);
// 	}
// }

void RenderBitmapFactory::CreateInstance(GRAPHICS_RENDER_LIBRARY_TYPE eGraphicsRenderType, IMAGE_ITEM_TYPE eType, IRenderBitmap** ppOut)
{
	if (NULL == ppOut)
		return;

	switch (eGraphicsRenderType)
	{	
	case GRAPHICS_RENDER_LIBRARY_TYPE_GDI:
		{
			if (eType == IMAGE_ITEM_TYPE_ICON)
			{
				GDIIconRenderBitmap::CreateInstance(ppOut);
			}
			else if (eType == IMAGE_ITEM_TYPE_IMAGE_LIST)
			{
				GDIImageListRenderBitmap::CreateInstance(ppOut);
			}
// 			else if (eType == IMAGE_ITEM_TYPE_GIF)
// 			{
// 				GDIGifRenderBitmap::CreateInstance(ppOut);
// 			}
// 			else if (eType == IMAGE_ITEM_TYPE_PNGLISTGIF)
// 			{
// 				GDIGifRenderBitmap::CreateInstance(ppOut);
// 			}
			else 
			{
				GDIRenderBitmap::CreateInstance(ppOut);
			}
		}
		break;

	case GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS:
		{
			if (eType == IMAGE_ITEM_TYPE_ICON)
			{
				GdiplusIconRenderBitmap::CreateInstance(ppOut);
			}
			else if (eType == IMAGE_ITEM_TYPE_IMAGE_LIST)
			{
				GdiplusImageListRenderBitmap::CreateInstance(ppOut);
			}
			else 
			{
				GdiplusRenderBitmap::CreateInstance(ppOut);
			}
		}
		break;

#ifdef UI_D2D_RENDER
	case GRAPHICS_RENDER_LIBRARY_TYPE_DIRECT2D:
		{
			Direct2DRenderBitmap::CreateInstance(ppOut);
		}
		break;
#endif
	}

// 	if (*ppOut)
// 	{
// 		(*ppOut)->SetAttribute(mapAttrib);
// 	} 
}
 
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////

// IRenderTarget::IRenderTarget()
// {
// 	m_hWnd = NULL;
// }
// IRenderTarget::IRenderTarget(HDC hDC)
// {
// 	m_hWnd = NULL;
// }	
IRenderTarget::IRenderTarget(HWND hWnd)
{
	m_hWnd = hWnd;
}

void IRenderTarget::Release()
{
	delete this;
}

//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
namespace UI
{
GRAPHICS_RENDER_LIBRARY_TYPE GetRenderLibraryType(HWND hWnd)
{
	if (NULL == hWnd)
		return GRAPHICS_RENDER_LIBRARY_TYPE_GDI;

	GRAPHICS_RENDER_LIBRARY_TYPE e = (GRAPHICS_RENDER_LIBRARY_TYPE)::SendMessage(hWnd, UI_WM_GET_GRAPHICS_RENDER_LIBRARY_TYPE, 0, 0);
	if (GRAPHICS_RENDER_LIBRARY_TYPE_AUTO == e)
	{
		if (WS_EX_LAYERED & ::GetWindowLong(hWnd, GWL_EXSTYLE))
		{
		//	if (UI_IsUnderXpOS())
				e = GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS;
// 			else
// 				e = GRAPHICS_RENDER_LIBRARY_TYPE_DIRECT2D;
		}
		else
		{
			e = GRAPHICS_RENDER_LIBRARY_TYPE_GDI;
		}
	}

	return e;
}

GRAPHICS_RENDER_LIBRARY_TYPE GetRenderLibraryType(IObject* pObj)
{
	if (NULL == pObj) 
		return GRAPHICS_RENDER_LIBRARY_TYPE_GDI;

    // 针对menu,listbox popup类型一开始没有窗口的控件，向控件本身发消息进行获取
    GRAPHICS_RENDER_LIBRARY_TYPE e = (GRAPHICS_RENDER_LIBRARY_TYPE)UISendMessage(pObj, UI_WM_GET_GRAPHICS_RENDER_LIBRARY_TYPE);
    if (GRAPHICS_RENDER_LIBRARY_TYPE_AUTO != e)
    {
        return e;
    }

    IWindowBase* pWindow = pObj->GetWindowObject();
    if (pWindow)
    {
	    e = (GRAPHICS_RENDER_LIBRARY_TYPE)UISendMessage(pWindow, UI_WM_GET_GRAPHICS_RENDER_LIBRARY_TYPE);
	    if (GRAPHICS_RENDER_LIBRARY_TYPE_AUTO == e)
	    {
		    if (WS_EX_LAYERED & ::GetWindowLong(pWindow->GetHWND(), GWL_EXSTYLE))
		    {
		    //	if (UI_IsUnderXpOS())
				    e = GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS;
    // 			else
    // 				e = GRAPHICS_RENDER_LIBRARY_TYPE_DIRECT2D;
		    }
		    else
		    {
			    e = GRAPHICS_RENDER_LIBRARY_TYPE_GDI;
		    }
	    }
    }
    else
    {
        e = GRAPHICS_RENDER_LIBRARY_TYPE_GDI;
    }

	return e;
}

IRenderTarget*  UICreateRenderTarget(GRAPHICS_RENDER_LIBRARY_TYPE eType, HWND hWnd)
{
    IRenderTarget*  pRenderTarget = NULL;

    switch (eType)
    {
    case GRAPHICS_RENDER_LIBRARY_TYPE_GDI:
        {
            pRenderTarget = new GdiRenderTarget(hWnd);
        }
        break;
    case GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS:
        {
            pRenderTarget = new GdiplusRenderTarget(hWnd);
        }
        break;
    default:
        UIASSERT(0);
        break;
    }
    return pRenderTarget;
}
IRenderTarget* UICreateRenderTarget(HWND hWnd)
{
    return UICreateRenderTarget(GetRenderLibraryType(hWnd), hWnd);
}

void  UICreateRenderBitmap(GRAPHICS_RENDER_LIBRARY_TYPE eGraphicsRenderType, IMAGE_ITEM_TYPE eType, IRenderBitmap** ppOut)
{
    RenderBitmapFactory::CreateInstance(eGraphicsRenderType, eType, ppOut);
}
}