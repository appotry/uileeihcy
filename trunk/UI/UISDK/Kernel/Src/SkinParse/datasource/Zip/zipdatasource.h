#pragma once
#include "UISDK\Kernel\Inc\Interface\iskindatasource.h"
#include "3rd\zip\unzip.h"

namespace UI
{

class ZipDataSource : public ISkinDataSource
{
public:
    ZipDataSource();
    ~ZipDataSource();

    void  Init();
    void  Release()
    {
        delete this;
    }

    void  SetDirection(const TCHAR* szDir);
    void  SetName(const TCHAR* szName);
    SKIN_PACKET_TYPE  GetType();

    const TCHAR*  GetDirection();
    const TCHAR*  GetName();

    bool  Load_UIDocument(IUIDocument* pDocument, const TCHAR* szPath);
    bool  Load_RenderBitmap(IRenderBitmap* pBitmap, const TCHAR* szPath, bool bCreateAlphaChannel);
    bool  Load_Image(IImage* pImage, const TCHAR* szPath);
    bool  Load_StreamBuffer(const TCHAR* szPath, IStreamBufferReader** pp);

private:
    void  TranslatePath(const TCHAR* szOrignPath, TCHAR* szLastPath);

private:
    String  m_strDir;
    String  m_strName;

    HZIP  m_hZip;
};

}