#ifndef GDIBITMAP_H_5C5778BD_74E9_4ff3_9A84_E41BCD454DF3
#define GDIBITMAP_H_5C5778BD_74E9_4ff3_9A84_E41BCD454DF3
#include "UISDK\Kernel\Src\Renderlibrary\renderresourceimpl.h"
#include "UISDK\Kernel\Src\atl\image.h"

namespace UI
{

template<class T>
class GDIRenderBitmapImpl : public T
{
public:
    enum 
    {
        GDI_RENDERBITMAP_FLAG_ATTACH = 0x01,
        GDI_RENDERBITMAP_FLAG_ATTACH_DELETE = 0x02,
    };
    GDIRenderBitmapImpl()
    {
        m_nFlag = 0;
    }
	virtual ~GDIRenderBitmapImpl()
	{
        if ((m_nFlag & GDI_RENDERBITMAP_FLAG_ATTACH) &&
            (m_nFlag & GDI_RENDERBITMAP_FLAG_ATTACH_DELETE))
        {
            m_image.Detach();
        }
//		UI_LOG_DEBUG(_T("GDIRenderBitmap Delete. ptr=0x%08X"), this);
	}

	virtual void SetAttribute(IMapAttribute* pMapAttrib){};
	virtual GRAPHICS_RENDER_LIBRARY_TYPE GetGraphicsRenderLibraryType() { return GRAPHICS_RENDER_LIBRARY_TYPE_GDI; }

	Image*  GetBitmap() { return &m_image; }

public:
	virtual bool  LoadFromFile(const TCHAR* szPath, bool bCreateAlphaChannel)
	{
		if (!m_image.IsNull())
		{
			m_image.Destroy();
		}

		if (bCreateAlphaChannel)
		{
			m_image.ForceUseAlpha();
		}

        const TCHAR* szExt = szPath + (_tcslen(szPath)-4);
		if (0 == _tcsicmp(szExt, _T(".ico")))
		{
			const int ICON_SIZE = 16;
			HICON hIcon = (HICON)::LoadImage ( NULL, szPath, IMAGE_ICON,ICON_SIZE,ICON_SIZE, LR_LOADFROMFILE );
			HDC hMemDC = Image::GetCacheDC();

			m_image.Create( ICON_SIZE, ICON_SIZE, 32, Image::createAlphaChannel );
			HBITMAP hOldBmp = (HBITMAP)::SelectObject( hMemDC, (HBITMAP)m_image );

			::DrawIconEx( hMemDC, 0,0, hIcon, ICON_SIZE, ICON_SIZE, 0, NULL, DI_NORMAL );
			::SelectObject(hMemDC, hOldBmp);
			Image::ReleaseCacheDC(hMemDC);
			::DestroyIcon(hIcon);
		}
		else
		{
			m_image.Load(szPath);
		}

		return m_image.IsNull() ? false: true;
	}
    virtual bool  LoadFromData(byte* pData, int nSize, bool bCreateAlphaChannel)
    {
        if (!m_image.IsNull())
        {
            m_image.Destroy();
        }

        if (bCreateAlphaChannel)
        {
            m_image.ForceUseAlpha();
        }

        m_image.LoadFromData(pData, nSize);
        return m_image.IsNull() ? false: true;
    }

	virtual bool  Create(int nWidth, int nHeight)
	{
		UIASSERT(nHeight > 0);   // 使用反向的(bottom-up DIB)
		if (!m_image.IsNull())
		{
			m_image.Destroy();
		}
		m_image.Create(nWidth, nHeight, 32, Image::createAlphaChannel );

		if (m_image.IsNull())
			return false;
		else
			return true;
	}

	virtual int   GetWidth()
	{
		return m_image.GetWidth();
	}
	virtual int   GetHeight()
	{
		return m_image.GetHeight();
	}

	virtual int   GetBPP()
	{
		return m_image.GetBPP();
	}

	virtual COLORREF GetAverageColor() 
	{
		return m_image.GetAverageColor();
	}

	virtual BYTE* LockBits()
	{
		return (BYTE*)m_image.GetBits();
	}
	virtual void  UnlockBits()
	{
		// Nothing.
	}

	virtual bool  SaveBits(ImageData* pImageData)
	{
		if (NULL == pImageData)
			return false;

		int nSize = 0;
		SAFE_ARRAY_DELETE(pImageData->m_ptr);
		m_image.SaveBits(pImageData, &nSize);
		pImageData->m_ptr = new BYTE[nSize];

		return m_image.SaveBits(pImageData, &nSize);
	}
	virtual bool  ChangeHLS( const ImageData* pOriginImageData, short h, short l , short s, int nFlag )
	{
		bool bRet = m_image.ChangeHLS(pOriginImageData, h, l, s, nFlag );
		return bRet;
	}

	virtual HBITMAP CopyRect(RECT *prc)
	{
		return m_image.CopyImageByRect(prc);
	}


    void  Attach(HBITMAP hBitmap, bool bDelete)
    {
        m_image.Attach(hBitmap);
        m_nFlag |= GDI_RENDERBITMAP_FLAG_ATTACH;
        if (bDelete)
        {
            m_nFlag |= GDI_RENDERBITMAP_FLAG_ATTACH_DELETE;
        }
    }
    virtual HBITMAP  Detach() 
    {
        m_nFlag &= ~(GDI_RENDERBITMAP_FLAG_ATTACH_DELETE|GDI_RENDERBITMAP_FLAG_ATTACH);
        return m_image.Detach(); 
    }
protected:
	Image   m_image;
    int  m_nFlag;
};

class GDIRenderBitmap : public GDIRenderBitmapImpl<IRenderResourceImpl<IRenderBitmap> >
{
public:
	static  void CreateInstance( IRenderBitmap** ppOutRef );
    virtual IMAGE_ITEM_TYPE  GetImageType() { return IMAGE_ITEM_TYPE_IMAGE; }
};
class GDIIconRenderBitmap : public GDIRenderBitmap
{
public:
	GDIIconRenderBitmap();

	static  void  CreateInstance(IRenderBitmap** pOutRef);
	virtual void  SetAttribute(IMapAttribute* pMapAttrib);
	virtual bool  LoadFromFile(const TCHAR* szPath, bool bCreateAlphaChannel);
    virtual bool  LoadFromData(byte* pData, int nSize, bool bCreateAlphaChannel);
    
    virtual IMAGE_ITEM_TYPE  GetImageType() { return IMAGE_ITEM_TYPE_ICON; }
private:
	int    m_nIconWidth;
	int    m_nIconHeight;
};

class GDIImageListRenderBitmap : public GDIRenderBitmapImpl<IRenderResourceImpl<IImageListRenderBitmap> >
{
protected:
	GDIImageListRenderBitmap();

public:
	static  void CreateInstance(IRenderBitmap** pOutRef );
	virtual void SetAttribute(IMapAttribute* pMapAttrib);

	virtual int  GetItemWidth();
	virtual int  GetItemHeight();
	virtual IMAGELIST_LAYOUT_TYPE GetLayoutType();
	virtual bool GetIndexPos(int nIndex, POINT* pPoint);
	virtual int  GetItemCount() { return m_nCount; }

    virtual IMAGE_ITEM_TYPE  GetImageType() { return IMAGE_ITEM_TYPE_IMAGE_LIST; }
private:
	IMAGELIST_LAYOUT_TYPE   m_eLayout;
	int     m_nCount;
};
}

#endif // GDIBITMAP_H_5C5778BD_74E9_4ff3_9A84_E41BCD454DF3