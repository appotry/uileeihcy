#include "stdafx.h"
#include "SkinDlg.h"
#include "UISDK\Control\Inc\Interface\ibutton.h"
#include "UISDK\Control\Inc\Interface\isliderctrl.h"
#include "UISDK\Kernel\Inc\Util\iimage.h"
#include "App\IM\IM_Ctrls\inc\ipluginbtnlistboxitem.h"
#include "skinui.h"
#include "App/IM/include/main_inc.h"

bool  MainDlgToolbarListItemCompareProc(UI::IListItemBase* p1, UI::IListItemBase* p2)
{
    IPluginBtnListBoxItem* pItem1 = static_cast<IPluginBtnListBoxItem*>(p1);
    IPluginBtnListBoxItem* pItem2 = static_cast<IPluginBtnListBoxItem*>(p2);

    return pItem1->GetSortValue() < pItem2->GetSortValue();
}

CSkinDlg::CSkinDlg()
{
 //   m_pMainDlg = NULL;

    m_pBtnPreview18394 = NULL;
    m_pBtnPreview19667 = NULL;
    m_pBtnPreview18409 = NULL;
    m_pBtnPreview19700 = NULL;
    m_pBtnPreview19508 = NULL;
    m_pBtnPreview19301 = NULL;
    m_pBtnPreview18400 = NULL;
    m_pBtnPreview19247 = NULL;
    m_pBtnPreviewDefault = NULL;
    m_pBtnPreview17 = NULL;
    m_pBtnPreview18 = NULL;
    m_pBtnPreview19 = NULL;
    m_pBtnPreview20 = NULL;
    m_pBtnPreview21 = NULL;
    m_pBtnPreview22 = NULL;
    m_pBtnPreview23 = NULL;
    m_pBtnPreview24 = NULL;
    m_pBtnPreview25 = NULL;
    m_pBtnPreview26 = NULL;
    m_pBtnPreview27 = NULL;
    m_pBtnPreview28 = NULL;
    m_pBtnPreview29 = NULL;

    m_pBtnCustom = NULL;
    m_pBtnUp = NULL;
    m_pBtnDown = NULL;
    m_pPluginBtnListBox = NULL;

    m_pTransparentSlider = NULL;
    m_pTextureSlider = NULL;
}
CSkinDlg::~CSkinDlg()
{

}

void  CSkinDlg::OnInitialize()
{
    __super::nvProcessMessage(GetCurMsg(), 0, 0);

    UI::IPanel*  pPanelTabSkin = (UI::IPanel*)this->FindChildObject(_T("tabpanel_skin"));
    UI::IPanel*  pPanelTabPlugin = (UI::IPanel*)this->FindChildObject(_T("tabpanel_uimgr"));

    m_pBtnPreview18394 = (UI::IButton*)pPanelTabSkin->FindChildObject(_T("btn_skinpreview_18394"));
    m_pBtnPreview19667 = (UI::IButton*)pPanelTabSkin->FindChildObject(_T("btn_skinpreview_19667"));
    m_pBtnPreview18409 = (UI::IButton*)pPanelTabSkin->FindChildObject(_T("btn_skinpreview_18409"));
    m_pBtnPreview19700 = (UI::IButton*)pPanelTabSkin->FindChildObject(_T("btn_skinpreview_19700"));
    m_pBtnPreview19508 = (UI::IButton*)pPanelTabSkin->FindChildObject(_T("btn_skinpreview_19508"));
    m_pBtnPreview19301 = (UI::IButton*)pPanelTabSkin->FindChildObject(_T("btn_skinpreview_19301"));
    m_pBtnPreview18400 = (UI::IButton*)pPanelTabSkin->FindChildObject(_T("btn_skinpreview_18400"));
    m_pBtnPreview19247 = (UI::IButton*)pPanelTabSkin->FindChildObject(_T("btn_skinpreview_19247"));
    m_pBtnPreviewDefault = (UI::IButton*)pPanelTabSkin->FindChildObject(_T("btn_skinpreivew_default"));
    m_pBtnPreview17 = (UI::IButton*)pPanelTabSkin->FindChildObject(_T("btn_skinpreivew_17"));
    m_pBtnPreview18 = (UI::IButton*)pPanelTabSkin->FindChildObject(_T("btn_skinpreivew_18"));
    m_pBtnPreview19 = (UI::IButton*)pPanelTabSkin->FindChildObject(_T("btn_skinpreivew_19"));
    m_pBtnPreview20 = (UI::IButton*)pPanelTabSkin->FindChildObject(_T("btn_skinpreivew_20"));
    m_pBtnPreview21 = (UI::IButton*)pPanelTabSkin->FindChildObject(_T("btn_skinpreivew_21"));
    m_pBtnPreview22 = (UI::IButton*)pPanelTabSkin->FindChildObject(_T("btn_skinpreivew_22"));
    m_pBtnPreview23 = (UI::IButton*)pPanelTabSkin->FindChildObject(_T("btn_skinpreivew_23"));
    m_pBtnPreview24 = (UI::IButton*)pPanelTabSkin->FindChildObject(_T("btn_skinpreivew_24"));
    m_pBtnPreview25 = (UI::IButton*)pPanelTabSkin->FindChildObject(_T("btn_skinpreivew_25"));
    m_pBtnPreview26 = (UI::IButton*)pPanelTabSkin->FindChildObject(_T("btn_skinpreivew_26"));
    m_pBtnPreview27 = (UI::IButton*)pPanelTabSkin->FindChildObject(_T("btn_skinpreivew_27"));
    m_pBtnPreview28 = (UI::IButton*)pPanelTabSkin->FindChildObject(_T("btn_skinpreivew_28"));
    m_pBtnPreview29 = (UI::IButton*)pPanelTabSkin->FindChildObject(_T("btn_skinpreivew_29"));

    m_pBtnCustom = (UI::IButton*)pPanelTabSkin->FindChildObject(_T("btn_customskin"));

    m_pTransparentSlider = (UI::ISliderCtrl*)pPanelTabSkin->FindChildObject(_T("slider_changetransparent_alpha"));;
    m_pTextureSlider = (UI::ISliderCtrl*)pPanelTabSkin->FindChildObject(_T("slider_changetexture_alpha"));;

    if (m_pTransparentSlider)
    {
        m_pTransparentSlider->SetRange(255, 32, false);
        m_pTransparentSlider->SetLine(-10);
    }
    if (m_pTextureSlider)
    {
        m_pTextureSlider->SetRange(255, 0, false);
        m_pTextureSlider->SetLine(-10);
    }

    m_pBtnUp = (UI::IButton*)pPanelTabPlugin->FindChildObject(_T("pluginbtn.up"));
    m_pBtnDown = (UI::IButton*)pPanelTabPlugin->FindChildObject(_T("pluginbtn.down"));
    m_pPluginBtnListBox = (UI::IListBox*)pPanelTabPlugin->FindChildObject(_T("pluginbtnlistbox"));
    if (m_pPluginBtnListBox)
    {
        m_pPluginBtnListBox->SetSortCompareProc(MainDlgToolbarListItemCompareProc);
        m_pPluginBtnListBox->ModifyStyleEx(LISTCTRLBASE_STYLE_SORT_ASCEND, 0, 0);
    }
    if (m_pBtnUp)
        m_pBtnUp->SetEnable(false, false);
    if (m_pBtnDown)
        m_pBtnDown->SetEnable(false, false);
}

void  CSkinDlg::SetSkinUI(SkinUI*  p)
{
    m_pSkinUI = p;
}
void  CSkinDlg::InitData(byte byteAlpha, byte byteTextureAlpha)
{
    if (m_pTextureSlider)
    {
        m_pTextureSlider->SetPos(byteTextureAlpha, false);
    }
    if (m_pTransparentSlider)
    {
        m_pTransparentSlider->SetPos(byteAlpha, false);
    }
}

void  CSkinDlg::OnSysCommand(UINT nID, CPoint point)
{
    SetMsgHandled(FALSE);

    if (nID == SC_CLOSE)
    {
        SetMsgHandled(TRUE);
        HideWindow();
        return;
    }
}   


void  CSkinDlg::OnBtnClick(UI::IMessage*  pBtn)
{
    UI::IUIApplication*  pUIApplication = GetUIApplication();
    UI::IImageRes*  pImageRes = pUIApplication->GetActiveSkinImageRes();

    if (pBtn == m_pBtnPreviewDefault)
    {
        this->m_pSkinUI->ChangeDefaultSkin();
    }
    else if (pBtn == m_pBtnPreview17)
    {
        this->m_pSkinUI->ChangeSkinByConfig(_T("2012\\Img\\SkinMgr\\1.45_17\\themeconfig.xml"));
    }
    else if (pBtn == m_pBtnPreview18)
    {
        this->m_pSkinUI->ChangeSkinByConfig(_T("2012\\Img\\SkinMgr\\1.45_18\\themeconfig.xml"));
    }
    else if (pBtn == m_pBtnPreview19)
    {
        this->m_pSkinUI->ChangeSkinByConfig(_T("2012\\Img\\SkinMgr\\1.45_19\\themeconfig.xml"));
    }
    else if (pBtn == m_pBtnPreview20)
    {
        this->m_pSkinUI->ChangeSkinByConfig(_T("2012\\Img\\SkinMgr\\1.45_20\\themeconfig.xml"));
    }
    else if (pBtn == m_pBtnPreview21)
    {
        this->m_pSkinUI->ChangeSkinByConfig(_T("2012\\Img\\SkinMgr\\1.45_21\\themeconfig.xml"));
    }
    else if (pBtn == m_pBtnPreview22)
    {
        this->m_pSkinUI->ChangeSkinByConfig(_T("2012\\Img\\SkinMgr\\1.45_22\\themeconfig.xml"));
    }
    else if (pBtn == m_pBtnPreview23)
    {
        this->m_pSkinUI->ChangeSkinByConfig(_T("2012\\Img\\SkinMgr\\1.45_23\\themeconfig.xml"));
    }
    else if (pBtn == m_pBtnPreview24)
    {
        this->m_pSkinUI->ChangeSkinByConfig(_T("2012\\Img\\SkinMgr\\1.45_24\\themeconfig.xml"));
    }
    else if (pBtn == m_pBtnPreview25)
    {
        this->m_pSkinUI->ChangeSkinByConfig(_T("2012\\Img\\SkinMgr\\1.45_25\\themeconfig.xml"));
    }
    else if (pBtn == m_pBtnPreview26)
    {
        this->m_pSkinUI->ChangeSkinByConfig(_T("2012\\Img\\SkinMgr\\1.45_26\\themeconfig.xml"));
    }
    else if (pBtn == m_pBtnPreview27)
    {
        this->m_pSkinUI->ChangeSkinByConfig(_T("2012\\Img\\SkinMgr\\1.45_27\\themeconfig.xml"));
    }
    else if (pBtn == m_pBtnPreview28)
    {
        this->m_pSkinUI->ChangeSkinByConfig(_T("2012\\Img\\SkinMgr\\1.45_28\\themeconfig.xml"));
    }
    else if (pBtn == m_pBtnPreview29)
    {
        this->m_pSkinUI->ChangeSkinByConfig(_T("2012\\Img\\SkinMgr\\1.45_29\\themeconfig.xml"));
    }
    else if (pBtn == m_pBtnPreview18394)
    {
        this->m_pSkinUI->ChangeSkinByConfig(_T("2012\\Img\\SkinMgr\\18394\\themeconfig.xml"));
    }
    else if (pBtn == m_pBtnPreview19667)
    {
        this->m_pSkinUI->ChangeSkinByConfig(_T("2012\\Img\\SkinMgr\\19667\\themeconfig.xml"));
    }
    else if (pBtn == m_pBtnPreview18409)
    {
        this->m_pSkinUI->ChangeSkinByConfig(_T("2012\\Img\\SkinMgr\\18409\\themeconfig.xml"));
    }
    else if (pBtn == m_pBtnPreview19700)
    {
        this->m_pSkinUI->ChangeSkinByConfig(_T("2012\\Img\\SkinMgr\\19700\\themeconfig.xml"));
    }
    else if (pBtn == m_pBtnPreview19508)
    {
        this->m_pSkinUI->ChangeSkinByConfig(_T("2012\\Img\\SkinMgr\\19508\\themeconfig.xml"));
    }
    else if (pBtn == m_pBtnPreview19301)
    {
       this->m_pSkinUI->ChangeSkinByConfig(_T("2012\\Img\\SkinMgr\\19301\\themeconfig.xml"));
    }
    else if (pBtn == m_pBtnPreview18400)
    {
        this->m_pSkinUI->ChangeSkinByConfig(_T("2012\\Img\\SkinMgr\\18400\\themeconfig.xml"));
    }
    else if (pBtn == m_pBtnPreview19247)
    {
        this->m_pSkinUI->ChangeSkinByConfig(_T("2012\\Img\\SkinMgr\\19247\\themeconfig.xml"));
    }
    else
    {
        SetMsgHandled(FALSE);
    }
}

void  CSkinDlg::OnChangeTransparent(int nPos, int nScrollType)
{
    m_pSkinUI->ChangeSkinTransparent(nPos);
}
void  CSkinDlg::OnChangeTextureAlpha(int nPos, int nScrollType)
{   
    m_pSkinUI->ChangeSkinTextureAlpha(nPos);
}
// void  CSkinDlg::SetMainDlg(CMainDlg* pDlg)
// {
//     m_pMainDlg = pDlg;
// }

void  CSkinDlg::OnBtnCustomSkin()
{
    CFileDialog dlg(TRUE, NULL, 0, 4|2, _T("*.bmp;*.jpg;*.png\0*.bmp;*.jpg;*.png\0\0"));
    if (IDCANCEL == dlg.DoModal())
        return;

    m_pSkinUI->ChangeCustomSkin(dlg.m_szFileName);
//    ChangeCustomSkin(dlg.m_szFileName);
}


#if 0
void  CSkinDlg::ChangeOwnSkin_Small(const String&  strImageFile)
{
    String  strFile;
    if (UI::Util::IsFullPath(strImageFile.c_str()))
    {
        strFile = strImageFile;
    }
    else
    {
        TCHAR szSkinDir[MAX_PATH] = _T("");
        UI::ISkinManager*  pSkinMgr = GetUIApplication()->GetSkinMgr();
        pSkinMgr->GetSkinDirection(szSkinDir);

        strFile = szSkinDir;
        strFile.append(_T("\\"));
        strFile.append(strImageFile);
    }

    // 获取图片（0，0）处的像素。（QQ默认皮肤都是这样取色值的。因为这些图片都是右上角对齐）
    UI::IImage  image;
    image.Load(strFile.c_str());

    DWORD dwEdgeColor = image.GetPixel(0, 0);
    dwEdgeColor |= 0xff000000;

    // 有些图片的边缘颜色与平均颜色不一致，HLS的颜色应该取平均色
    DWORD dwAverageColor = image.GetAverageColor();

    m_pSkinUI->ChangeSkin(strFile, UI::DRAW_BITMAP_BITBLT_RIGHTTOP, dwEdgeColor, dwAverageColor);
}
void  CSkinDlg::ChangeDefaultSkin()
{
    TCHAR szSkinDir[MAX_PATH] = _T("");
    UI::ISkinManager*  pSkinMgr = GetUIApplication()->GetSkinMgr();
    pSkinMgr->GetSkinDirection(szSkinDir);

    String strFile = szSkinDir;
    strFile.append(_T("\\2012\\Img\\SkinMgr\\DefaultSkins\\main_bkg.jpg"));

    // 获取图片（0，0）处的像素。
    UI::IImage  image;
    image.Load(strFile.c_str());

    DWORD dwEdgeColor = image.GetPixel(0, 0);
    dwEdgeColor |= 0xff000000;

    m_pSkinUI->ChangeSkin(strFile, UI::DRAW_BITMAP_STRETCH, dwEdgeColor, dwEdgeColor);
}

// 大预览图片换肤
void  CSkinDlg::ChangeOwnSkin_Big(const String&  strImageFile)
{
    TCHAR szSkinDir[MAX_PATH] = _T("");
    UI::ISkinManager*  pSkinMgr = GetUIApplication()->GetSkinMgr();
    pSkinMgr->GetSkinDirection(szSkinDir);

    String  strFile = szSkinDir;
    strFile.append(_T("\\"));
    strFile.append(strImageFile);

    // 制作背景图
    UI::IImage  image;
    image.Load(strFile.c_str());

    DWORD dwBackColor = image.GetPixel(image.GetWidth()-1, 0);
    DWORD dwAverageColor = image.GetAverageColor();
    dwBackColor |= 0xff000000;

    m_pSkinUI->ChangeSkin(strFile.c_str(), UI::DRAW_BITMAP_BITBLT, dwBackColor, dwAverageColor);
}

void  CSkinDlg::ChangeCustomSkin(const String&  strImageFile)
{
    TCHAR szSkinDir[MAX_PATH] = _T("");
    UI::ISkinManager*  pSkinMgr = GetUIApplication()->GetSkinMgr();
    pSkinMgr->GetSkinDirection(szSkinDir);

    String  strFile = szSkinDir;
    strFile.append(_T("\\..\\customskin\\skin.png"));

    // 制作背景图
    UI::IImage  image;
    image.Load(strImageFile.c_str());

    // 必须先计算平均色，再修改图片渐变边缘
    DWORD dwAverageColor = image.GetAverageColor();
    dwAverageColor |= 0xff000000;

    image.AlphaBottomEdge(2);
    image.AlphaRightEdge(2);
    image.SaveAsPng(strFile.c_str());

    m_pSkinUI->ChangeSkin(strFile.c_str(), UI::DRAW_BITMAP_BITBLT, dwAverageColor, dwAverageColor);
}
#endif


#if 0
void  CSkinDlg::InitPluginBtnListBox()
{
//     if (NULL == m_pPluginBtnListBox)
//         return;

    // 默认添加按钮
//     UI::IUIApplication*  pUIApp = GetUIApplication();
//     UI::ISkinManager*  pSkinMgr = pUIApp->GetSkinMgr();
// 
//     IPluginBtnListBoxItem*  pItem = NULL;
//     IPluginBtnListBoxItem::CreateInstance(pUIApp, &pItem);
//     m_pPluginBtnListBox->AddItem(pItem, LISTITEM_OPFLAG_NOALL);
// 
//     pItem->SetText(_T("界面管理器"));
//     pItem->SetSortValue(99999999);
// 
//     UI::ISkinRes* pSkinRes = pSkinMgr->GetActiveSkin();
//     UI::IImageRes* pImageRes = pSkinRes->GetImageRes();
//     UI::IImageResItem* pResItem = pImageRes->GetImageResItem(_T("changeskin_pluginadd"));
//     if (pResItem)
//     {
//         pItem->SetIconPath(pResItem->GetPath());
//     }
//     
//     m_pPluginBtnListBox->UpdateItemRect();
}
#endif

void  CSkinDlg::InsertToolbarPlugin(IM::IMainDlgToolbarPlugin* pPlugin, int nSort)
{
    // 加载插件数据
    
    IPluginBtnListBoxItem*  pItem = NULL;
    IPluginBtnListBoxItem::CreateInstance(GetUIApplication(), &pItem);
    m_pPluginBtnListBox->AddItem(pItem, LISTITEM_OPFLAG_NOALL);

    pItem->SetText(pPlugin->GetDesc());
    pItem->SetIcon(pPlugin->GetIcon2());
    pItem->SetSortValue(nSort);

    pItem->SetData((LPARAM)pPlugin);
    pItem->SetCheck(true);

    m_pPluginBtnListBox->Sort();
    m_pPluginBtnListBox->UpdateItemRect();
}

void  CSkinDlg::OnLcnSelChanged(UI::IMessage* pMsgFrom, UI::IListItemBase* pOldSelItem, UI::IListItemBase* pSelItem)
{
    if (m_pPluginBtnListBox == pMsgFrom)
    {
        UI::IListItemBase* pLastItem = m_pPluginBtnListBox->GetLastItem();
        if (NULL == pSelItem || pSelItem == pLastItem)
        {
            m_pBtnUp->SetEnable(false, true);
            m_pBtnDown->SetEnable(false, true);
        }
        else
        {
            m_pBtnUp->SetEnable(pSelItem->GetPrevItem() ? true:false, true);
            m_pBtnDown->SetEnable(pSelItem->GetNextItem()==pLastItem ? false:true, true);
        }
    }
}
void  CSkinDlg::OnBtnUp()
{
    IPluginBtnListBoxItem*  p = (IPluginBtnListBoxItem*)m_pPluginBtnListBox->GetFirstSelectItem();
    if (NULL == p)
        return;

    IPluginBtnListBoxItem*  pPrev = (IPluginBtnListBoxItem*)p->GetPrevItem();
    m_pPluginBtnListBox->SwapItemPos(p, pPrev);

    // 更新上下按钮状态
    OnLcnSelChanged(m_pPluginBtnListBox, NULL, p);
}
void  CSkinDlg::OnBtnDown()
{
    IPluginBtnListBoxItem*  p = static_cast<IPluginBtnListBoxItem*>(m_pPluginBtnListBox->GetFirstSelectItem());
    if (!p)
        return;

    IPluginBtnListBoxItem*  pNext = (IPluginBtnListBoxItem*)p->GetNextItem();
    m_pPluginBtnListBox->SwapItemPos(p, pNext);

    // 更新上下按钮状态
    OnLcnSelChanged(m_pPluginBtnListBox, NULL, p);
}

LRESULT  CSkinDlg::OnPluginListCheckbtnClicked(WPARAM w, LPARAM l)
{
    PluginBtnListBoxItemCheckBtnClick*  pData = (PluginBtnListBoxItemCheckBtnClick*)l;
    m_pSkinUI->ShowToolbarPluginBtn((IM::IMainDlgToolbarPlugin*)pData->pItem->GetData(), pData->bChecked?true:false);

    return 0;
}