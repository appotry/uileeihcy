#include "stdafx.h"
#include "skinui.h"
#include "skindlg.h"
#include "App\IM\include\task.h"
#include "App\IM\Module\Skin\Business\skinbiz.h"
#include "UISDK\Kernel\Inc\Interface\iimagerender.h"
#include "UISDK/Kernel/Inc/Util/iimage.h"
#include "App/IM/include/main_inc.h"

inline bool  IsEqualEvent(const TCHAR* szName, WPARAM wParam)
{
    if (!szName || !wParam)
        return false;

    return 0 == _tcscmp(szName, (TCHAR*)wParam);
}


SkinUI::SkinUI()
{
    m_pShareSkinRender = NULL;
    m_pSkinDlg = NULL;

    m_byteAlpha = 255;
    m_byteTextureAlpha = 255;
    m_pMainUI = NULL;
}

SkinUI::~SkinUI()
{
}


// material.render.type="image"
// material.render.image="skin"
// material.render.region="6"
// material.render.image.drawtype="bitbltrighttop"
void  SkinUI::CreateShareSkinRender()
{
    UI::IUIApplication*  pUIApp = IM::GetUIApplication();
    if (NULL == pUIApp)
        return;

    UI::IRenderBase*  pRenderBase = NULL;
    pUIApp->CreateRenderBase(UI::RENDER_TYPE_IMAGE, NULL, &pRenderBase);
    if (NULL == pRenderBase)
        return;

    m_pShareSkinRender = (UI::IImageRender*)pRenderBase->QueryInterface(UI::uiiidof(IImageRender));
    
    UI::IMapAttribute*  pMapAttrib = NULL;
    UI::UICreateIMapAttribute(&pMapAttrib);
    pMapAttrib->AddAttr(_T("render.image"), _T("skin"));
    pMapAttrib->AddAttr(_T("render.image.drawtype"), _T("stretch"));  // 默认皮肤是拉伸模式 _T("bitbltrighttop")
    pMapAttrib->AddAttr(_T("render.type"), _T("image"));
    m_pShareSkinRender->SetAttribute(pMapAttrib, NULL, true);
    SAFE_RELEASE(pMapAttrib);

    UI::IImageRender* pImageRender = (UI::IImageRender*)pRenderBase->QueryInterface(UI::IID_UI_IImageRender);
    if (pImageRender)
    {
        pImageRender->SetBkColorFillType(UI::BKCOLOR_FILL_EMPTY);
    }
}

UI::IImageRender*  SkinUI::GetShareSkinRender()
{
    if (NULL == m_pShareSkinRender)
    {
        CreateShareSkinRender();
    }

    return m_pShareSkinRender;
}

long SkinUI::ExecuteCommand( long nCommand, WPARAM wParam, LPARAM lParam )
{
    if (IM::BASE_CMD_ID_TASK == nCommand)
    {
        IM::ITask* pTask = (IM::ITask*)wParam;
//         switch (pTask->GetCommandId())
//         {
//         }
    }
    else if (IM::BASE_CMD_ID_EXIT == nCommand)
    {
        if (m_pSkinDlg)
        {
            if (::IsWindow(m_pSkinDlg->GetHWND()))
            {
                ::DestroyWindow(m_pSkinDlg->GetHWND());
            }
            SAFE_DELETE_Ixxx(m_pSkinDlg);
        }

        SAFE_RELEASE(m_pShareSkinRender);
    }
    else if (IM::BASE_CMD_ID_EVENT == nCommand)
    {

    }
    else if (IM::BASE_CMD_ID_INIT == nCommand)
    {
        IM::GetFramework()->AddUIEvent(EVENT_SKIN_TEXTURE_ALPHA_CHANGED);
    }
    return 0;
}

// 直接由CMainUI调用过来的
void  SkinUI::Show()
{
    if (NULL == m_pSkinDlg)
    {
        CSkinDlg::CreateInstance(IM::GetUIApplication(), &m_pSkinDlg);
        m_pSkinDlg->Create(IM::GetUIApplication(), _T("changeskindlg"));
        m_pSkinDlg->SetSkinUI(this);
        m_pSkinDlg->InitData(m_byteAlpha, m_byteTextureAlpha);

        if (NULL == m_pSkinDlg->GetMaterialRender())
        {
            m_pSkinDlg->SetMaterialRender(GetShareSkinRender());
        }

        if (NULL == m_pMainUI)
        {
            m_pMainUI = (IM::IMainUI*)IM::GetFramework()->GetUI(MODULE_ID_MAIN);
        }
        if (m_pMainUI)
        {
            long lCount = m_pMainUI->GetToolbarPluginCount();
            for (long i = 0; i < lCount; i++)
            {
                int nSort = 0;
                IM::IMainDlgToolbarPlugin* pPlugin = m_pMainUI->GetToolbarPluginByIndex(i, &nSort);
                m_pSkinDlg->InsertToolbarPlugin(pPlugin, nSort);
            }
        }
    }
    else
    {

    }
    m_pSkinDlg->ShowWindow();
}


long  SkinUI::ChangeSkin(ThemeData* pThemeData)
{
    ChangeSkin(pThemeData->szPath, pThemeData->nDrawType, pThemeData->bkcol, pThemeData->avgcol);
    delete pThemeData;
    return 0;
}

// 读取保存的皮肤数据
long  SkinUI::InitSkinData(SkinData* pSkinData)
{
    ChangeSkin(pSkinData->szPath, pSkinData->nDrawType, pSkinData->bkcol, pSkinData->avgcol);    
    if (m_pShareSkinRender)
    {
        m_pShareSkinRender->SetAlpha(pSkinData->byteAlpha);
    }
    m_byteAlpha = pSkinData->byteAlpha;
    m_byteTextureAlpha = pSkinData->byteTextureAlpha;
    delete pSkinData;
    return 0;
}

void  SkinUI::ChangeSkin(const String&  strFile, int nImageDrawType, int nColor, int nHLSColor)
{
    if (NULL == m_pShareSkinRender)
        CreateShareSkinRender();
    
    if (NULL == m_pShareSkinRender)
        return;

    UI::IUIApplication*  pUIApplication = IM::GetUIApplication();
    UI::ISkinManager*  pSkinMgr = pUIApplication->GetSkinMgr();
    UI::IImageRes*  pImageRes = pUIApplication->GetActiveSkinImageRes();

    UI::IImageResItem* pImageResItem = pImageRes->GetImageResItem(_T("skin"));
    if (NULL == pImageResItem)
        return;

    pImageResItem->ModifyImage(strFile.c_str());
    m_pShareSkinRender->SetColor(nColor);
    m_pShareSkinRender->SetImageDrawType(nImageDrawType);

    WORD h = 0, l = 0, s = 0;
    ColorRGBToHLS(nHLSColor, &h, &l, &s);
    UI::ISkinRes*  pSkin = pSkinMgr->GetActiveSkin();
    pSkin->ChangeSkinHLS(h, l, s, CHANGE_SKIN_HLS_FLAG_H|CHANGE_SKIN_HLS_FALG_REPLACE_MODE);

    UI::ITopWindowManager*  pTopWindowMgr = pUIApplication->GetTopWindowMgr();
    if (pTopWindowMgr)
    {
        pTopWindowMgr->UpdateAllWindow();

        UI::UIMSG  msg;
        msg.message = UI_WM_SKINMATERIALCHANGED;
        pTopWindowMgr->ForwardMessage2AllObj(&msg);

        pTopWindowMgr->UpdateAllWindow();
    }
}

void  SkinUI::ChangeSkinTransparent(int n)
{
    if (m_pShareSkinRender)
    {
        m_pShareSkinRender->SetAlpha(n);
        IM::GetUIApplication()->RedrawTopWindows();
        IM::GetFramework()->PostFunction(IM::Biz, IM::create_mem_fun1_pkg(&SkinBiz::OnSkinAlphaChanged, g_pSkinBiz, n), false);
        m_byteAlpha = (byte)n;
    }
}

void  SkinUI::ChangeSkinTextureAlpha(int n)
{
    if (m_pShareSkinRender)
    {
//         if (m_pMainDlg)
//         {
//             m_pMainDlg->OnTextureAlphaChanged(nPos);
//         }
//         if (m_pChatDlg)
//         {
//             m_pChatDlg->OnTextureAlphaChanged(nNewAlpha);
//         }

        // 发送事件通知
        IM::GetFramework()->FireUIEvent(EVENT_SKIN_TEXTURE_ALPHA_CHANGED, n);

        IM::GetUIApplication()->RedrawTopWindows();
        IM::GetFramework()->PostFunction(IM::Biz, IM::create_mem_fun1_pkg(&SkinBiz::OnSkinTextureAlphaChanged, g_pSkinBiz, n), false);
        m_byteTextureAlpha = (byte)n;
    }
}

void SkinUI::ChangeCustomSkin( const TCHAR* szImgPath )
{
    TCHAR szSkinDir[MAX_PATH] = _T("");
    UI::ISkinManager*  pSkinMgr = IM::GetUIApplication()->GetSkinMgr();
    pSkinMgr->GetSkinDirection(szSkinDir);

    String  strFile = szSkinDir;
    strFile.append(_T("\\..\\customskin\\skin.png"));

    // 制作背景图
    UI::IImage  image;
    image.Load(szImgPath);

    // 必须先计算平均色，再修改图片渐变边缘
    DWORD dwAverageColor = image.GetAverageColor();
    dwAverageColor |= 0xff000000;

    image.AlphaBottomEdge(2);
    image.AlphaRightEdge(2);
    image.SaveAsPng(strFile.c_str());

    ChangeSkin(strFile.c_str(), UI::DRAW_BITMAP_BITBLT, dwAverageColor, dwAverageColor);
}

void  SkinUI::ChangeDefaultSkin()
{
    this->ChangeSkinByConfig(_T("2012\\Img\\SkinMgr\\DefaultSkins\\themeconfig.xml"));
}

void  SkinUI::ChangeSkinByConfig(const TCHAR*  szXml)
{
    if (NULL == szXml)
        return;

    String  strFile;
    if (UI::Util::IsFullPath(szXml))
    {
        strFile = szXml;
    }
    else
    {
        TCHAR szSkinDir[MAX_PATH] = _T("");
        UI::ISkinManager*  pSkinMgr = IM::GetUIApplication()->GetSkinMgr();
        pSkinMgr->GetSkinDirection(szSkinDir);

        strFile = szSkinDir;
        strFile.append(_T("\\"));
        strFile.append(szXml);
    }

    CComBSTR bstr(strFile.c_str());
    IM::GetFramework()->PostFunction(IM::Biz, IM::create_mem_fun1_pkg(&SkinBiz::ChangeSkinByConfig, g_pSkinBiz, (BSTR)bstr.Detach()), false);
}

void SkinUI::ShowToolbarPluginBtn( IM::IMainDlgToolbarPlugin* p, bool bShow )
{
    if (m_pMainUI)
    {
        m_pMainUI->ShowToolbarPluginBtn(p, bShow);
    }
}