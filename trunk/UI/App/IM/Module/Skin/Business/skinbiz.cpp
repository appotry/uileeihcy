#include "stdafx.h"
#include "skinbiz.h"
#include "App\IM\include\skin_inc.h"
#include "3rd\markup\markup.h"
#include "App\IM\Module\Skin\ui\skinui.h"

SkinBiz::SkinBiz()
{   
    m_pConfigBiz = NULL;
}

SkinBiz::~SkinBiz()
{
}

long SkinBiz::ExecuteCommand( long nCommand, WPARAM wParam, LPARAM lParam )
{
    if (nCommand == IM::BASE_CMD_ID_TASK)
    {
        IM::ITask* pTask = (IM::ITask*)wParam;
    }
    else if (nCommand == IM::BASE_CMD_ID_INIT)
    {
        OnInitialize();
    }
    return 0;
}

void SkinBiz::OnInitialize()
{
    m_pConfigBiz = (IM::IConfigBiz*)IM::GetFramework()->GetBiz(MODULE_ID_CONFIG);
    if (NULL == m_pConfigBiz)
        return;

    TCHAR  szSkinPath[MAX_PATH] = _T("");
    m_pConfigBiz->GetCurSkinPath(szSkinPath);
    

    SkinData* pData = new SkinData;
    memset(pData, 0, sizeof(SkinData));

    if (!ParseSkinPackageXml(szSkinPath, pData))
    {
        SAFE_DELETE(pData);
    }
    else
    {
        pData->byteAlpha = m_pConfigBiz->GetSkinAlpha();
        pData->byteTextureAlpha = m_pConfigBiz->GetSkinTextureAlpha();
        IM::GetFramework()->PostFunction(IM::Ui, IM::create_mem_fun1_pkg(&SkinUI::InitSkinData, g_pSkinUI, pData), true);
    }
}

//  解析皮肤包
bool  SkinBiz::ParseSkinPackageXml(const TCHAR* szXmlPath, ThemeData* pData)
{
    if (!szXmlPath)
        return false;

    CMarkup  markup;
    if (!markup.Load(szXmlPath))
        return false;

    if (!markup.FindElem() || !markup.IntoElem())
        return false;

    if (markup.FindElem(_T("main")))
    {
        String  strMainFile = markup.GetAttrib(_T("file"));
        String  strDrawType = markup.GetAttrib(_T("drawtype"));
        String  strBackColor = markup.GetAttrib(_T("bkcol"));
        String  strAvgColor = markup.GetAttrib(_T("avgcol"));

        pData->bkcol = UI::Util::TranslateHexColor(strBackColor.c_str());
        pData->avgcol = UI::Util::TranslateHexColor(strAvgColor.c_str());

        pData->nDrawType = UI::DRAW_BITMAP_BITBLT;
        if (0 == _tcscmp(_T("stretch"), strDrawType.c_str()))
        {
            pData->nDrawType = UI::DRAW_BITMAP_STRETCH;
        }
        else if (0 == _tcscmp(_T("topright"), strDrawType.c_str()))
        {
            pData->nDrawType = UI::DRAW_BITMAP_BITBLT_RIGHTTOP;
        }
        else if (0 == _tcscmp(_T("bottomleft"), strDrawType.c_str()))
        {
            pData->nDrawType = UI::DRAW_BITMAP_BITBLT_LEFTBOTTOM;
        }
        else if (0 == _tcscmp(_T("bottomright"), strDrawType.c_str()))
        {
            pData->nDrawType = UI::DRAW_BITMAP_BITBLT_RIGHTBOTTOM;
        }

        if (UI::Util::IsFullPath(strMainFile.c_str()))
        {
            _tcsncpy(pData->szPath, strMainFile.c_str(), MAX_PATH-1);
        }
        else
        {
            UI::Util::CalcFullPathByRelative(szXmlPath, strMainFile.c_str(), pData->szPath);
        }
    }
    
    return true;
}

// UI调用Biz，点击了某个皮肤按钮
long SkinBiz::ChangeSkinByConfig( BSTR bstrXml )
{
    if (!bstrXml)
        return 0;

    ThemeData* pData = new ThemeData;
    memset(pData, 0, sizeof(ThemeData));

    if (!ParseSkinPackageXml(bstrXml, pData))
    {
        delete pData;
        return 0;
    }

    IM::GetFramework()->PostFunction(IM::Ui, IM::create_mem_fun1_pkg(&SkinUI::ChangeSkin, g_pSkinUI, pData), false);

    // Save
    m_pConfigBiz->SetCurSkinPath(bstrXml);
    SysFreeString(bstrXml);
    return 0;
}

// UI通知Biz
long  SkinBiz::OnSkinAlphaChanged(int n)
{
    m_pConfigBiz->SetSkinAlpha((byte)n);
    return 0;
}
// UI通知Biz
long  SkinBiz::OnSkinTextureAlphaChanged(int n)
{
    m_pConfigBiz->SetSkinTextureAlpha((byte)n);
    return 0;
}