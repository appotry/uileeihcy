#include "stdafx.h"
#include "filedatasource.h"
#include "UISDK\Kernel\Src\SkinParse\msxmlwrap\msxmlwrap.h"
#include "UISDK\Kernel\Inc\Util\iimage.h"
#include "UISDK\Kernel\Src\Atl\image.h"
#include "UISDK\Kernel\Src\SkinParse\datasource\File\filebufferreader.h"

namespace UI
{

void  FileDataSource::SetDirection(const TCHAR* szDir)
{
    if (szDir)
        m_strDir = szDir;
    else
        m_strDir.clear();

    int nLength = m_strDir.length();
    if (nLength > 1)
    {
        if (m_strDir[nLength-1] != _T('\\'))
        {
            m_strDir.append(_T("\\"));
        }
    }
}
void  FileDataSource::SetName(const TCHAR* szName)
{
    if (szName)
        m_strName = szName;
    else
        m_strName.clear();
}
const TCHAR*  FileDataSource::GetDirection()
{
    return m_strDir.c_str();
}
const TCHAR*  FileDataSource::GetName()
{
    return m_strName.c_str();
}
SKIN_PACKET_TYPE  FileDataSource::GetType()
{
    return SKIN_PACKET_TYPE_DIR;
}
bool FileDataSource::Load_UIDocument(IUIDocument* pDocument, const TCHAR* szPath)
{
    if (NULL == pDocument || NULL == szPath)
        return false;

    String strTemp = m_strDir;
    strTemp.append(szPath);

    return pDocument->LoadFile(strTemp.c_str());
}

bool  FileDataSource::Load_RenderBitmap(IRenderBitmap* pBitmap, const TCHAR* szPath, bool bCreateAlphaChannel)
{
    if (NULL == pBitmap || NULL == szPath)
        return false;

    String strTemp = m_strDir;
    strTemp.append(szPath);

    return pBitmap->LoadFromFile(strTemp.c_str(), bCreateAlphaChannel);
}

bool  FileDataSource::Load_Image(IImage* pImage, const TCHAR* szPath)
{
    if (NULL == pImage || NULL == szPath)
        return false;

    String strTemp = m_strDir;
    strTemp.append(szPath);

    pImage->GetImpl()->Load(strTemp.c_str());
    return pImage->IsNull()? false:true;
}

bool  FileDataSource::Load_StreamBuffer(const TCHAR* szPath, IStreamBufferReader** pp)
{
    if (NULL == pp || NULL == szPath)
        return false;

    String strTemp = m_strDir;
    strTemp.append(szPath);

    FileBufferReader*  pBuffer = new FileBufferReader;
    if (!pBuffer->load(strTemp.c_str()))
    {
        delete pBuffer;
        return false;
    }

    *pp = pBuffer;
    return true;
}

// 
// bool  FileDataSource::CalcFilePath(const TCHAR* szPath, ISkinRes* pSkinRes, __out String& strLastPath)
// {
//     if (NULL == szPath || NULL == pSkinRes)
//         return false;
// 
//     if (_tcslen(szPath) >= MAX_PATH)
//         return false;
// 
//     // 1. 直接就是绝对路径
//     if (Util::IsFullPath(szPath))
//     {
//         if (!::PathFileExists(szPath))
//             return false;
// 
//         strLastPath = szPath;
//         return true;
//     }
// 
//     TCHAR  szFullPath[MAX_PATH] = _T("");
// 
//     // 相对于皮肤包路径
//     const TCHAR* szSkinDir = pSkinRes->GetPath();
//     if (Util::CalcFullPathByRelative(szSkinDir, szPath, szFullPath))
//     {
//         if (PathFileExists(szFullPath))
//         {
//             strLastPath = szFullPath;
//             return true;
//         }
//     }
// 
//     // 相对于配置文件路径
//     IUIDocument*  pDoc = NULL;
//     pElement->GetDocument(&pDoc);
// 
//     if (NULL == pDoc)
//         return false;
// 
//     TCHAR  szFileDir[MAX_PATH] = _T("");
//     if (FALSE == Util::GetPathDir(pDoc->GetPath(), szFileDir))
//     {
//         SAFE_RELEASE(pDoc);
//         return false;
//     }
//     SAFE_RELEASE(pDoc);
// 
//     if (Util::CalcFullPathByRelative(szFileDir, szPath, szFullPath))
//     {
//         if (PathFileExists(szFullPath))
//         {
//             strLastPath = szFullPath;
//             return true;
//         }
//     }
// 
//     return false;
// }

}