#pragma once
#include "UISDK\Kernel\Inc\Interface\iskindatasource.h"

namespace UI
{

class FileDataSource : public ISkinDataSource
{
public:
    void  Init() {};
    void  Release()
    {
        delete this;
    }
    void  SetDirection(const TCHAR* szDir);
    void  SetName(const TCHAR* szName);

    const TCHAR*  GetDirection();
    const TCHAR*  GetName();
    SKIN_PACKET_TYPE  GetType();

    bool  Load_UIDocument(IUIDocument* pDocument, const TCHAR* szPath);
    bool  Load_RenderBitmap(IRenderBitmap* pBitmap, const TCHAR* szPath, bool bCreateAlphaChannel);
    bool  Load_Image(IImage* pImage, const TCHAR* szPath);
    bool  Load_StreamBuffer(const TCHAR* szPath, IStreamBufferReader** pp);

private:
   // bool  FileDataSource::CalcFilePath(const TCHAR* szData, ISkinRes* pSkinRes, __out String& strLastPath);

private:
    String  m_strDir;
    String  m_strName;
};

}