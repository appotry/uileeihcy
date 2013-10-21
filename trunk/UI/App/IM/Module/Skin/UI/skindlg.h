#pragma once
#include "UISDK\Kernel\Inc\Interface\icustomwindow.h"
#include "App\IM\IM_Ctrls\inc\ipluginbtnlistboxitem.h"

class CMainDlg;
namespace UI
{
    interface IButton;
    interface ISliderCtrl;
}
namespace IM
{
    struct IMainDlgToolbarPlugin;
}

class CSkinDlg : public UI::ICustomWindow
{
public:
    CSkinDlg();
    ~CSkinDlg();

    UI_BEGIN_MSG_MAP_Ixxx(CSkinDlg)
        UIMSG_WM_INITIALIZE(OnInitialize)
        UIMSG_BN_CLICKED2(m_pBtnCustom, OnBtnCustomSkin)
        UIMSG_BN_CLICKED2(m_pBtnUp, OnBtnUp)
        UIMSG_BN_CLICKED2(m_pBtnDown, OnBtnDown)
        UIMSG_LCN_SELCHANGED(OnLcnSelChanged)
        UIMSG_WM_NOTIFY(LBN_PLUGIN_CHECKBTN_CLICKED, OnPluginListCheckbtnClicked)
        UIMSG_BN_CLICKED3(OnBtnClick)
        UIMSG_WM_SYSCOMMAND(OnSysCommand)
        UIMSG_TRBN_POSCHANGED(m_pTransparentSlider, OnChangeTransparent)
        UIMSG_TRBN_POSCHANGED(m_pTextureSlider,     OnChangeTextureAlpha)
    UI_END_MSG_MAP_CHAIN_PARENT(ICustomWindow)

public:
  //  void  SetMainDlg(CMainDlg* pDlg);
    void  SetSkinUI(SkinUI*  p);
    void  InitData(byte byteAlpha, byte byteTextureAlpha);
    void  InsertToolbarPlugin(IM::IMainDlgToolbarPlugin* pPlugin, int nSort);

protected:
    void  OnInitialize();
    void  OnSysCommand(UINT nID, CPoint point);
    void  OnBtnClick(UI::IMessage*  pBtn);
    void  OnBtnCustomSkin();
    void  OnBtnUp();
    void  OnBtnDown();

    LRESULT  OnPluginListCheckbtnClicked(WPARAM w, LPARAM l);
    void  OnLcnSelChanged(UI::IMessage* pMsgFrom, UI::IListItemBase* pOldSelItem, UI::IListItemBase* pSelItem);
    void  OnChangeTransparent(int nPos, int nScrollType);
    void  OnChangeTextureAlpha(int nPos, int nScrollType);

//     void  ChangeOwnSkin_Big(const String&  strImageFile);
//     void  ChangeOwnSkin_Small(const String&  strImageFile);
//     void  ChangeDefaultSkin();
//     void  ChangeCustomSkin(const String&  strImageFile);

//     void  InitPluginBtnListBox();

private:
    //////////////////////////////////////////////////////////////////////////
    // 换肤
    UI::IButton*  m_pBtnPreview18394;
    UI::IButton*  m_pBtnPreview19667;
    UI::IButton*  m_pBtnPreview18409;
    UI::IButton*  m_pBtnPreview19700;
    UI::IButton*  m_pBtnPreview19508;
    UI::IButton*  m_pBtnPreview19301;
    UI::IButton*  m_pBtnPreview18400;
    UI::IButton*  m_pBtnPreview19247;

    UI::IButton*  m_pBtnPreviewDefault;
    UI::IButton*  m_pBtnPreview17;
    UI::IButton*  m_pBtnPreview18;
    UI::IButton*  m_pBtnPreview19;
    UI::IButton*  m_pBtnPreview20;
    UI::IButton*  m_pBtnPreview21;
    UI::IButton*  m_pBtnPreview22;
    UI::IButton*  m_pBtnPreview23;
    UI::IButton*  m_pBtnPreview24;
    UI::IButton*  m_pBtnPreview25;
    UI::IButton*  m_pBtnPreview26;
    UI::IButton*  m_pBtnPreview27;
    UI::IButton*  m_pBtnPreview28;
    UI::IButton*  m_pBtnPreview29;

    UI::IButton*  m_pBtnCustom;

    UI::ISliderCtrl*  m_pTransparentSlider;
    UI::ISliderCtrl*  m_pTextureSlider;

    //////////////////////////////////////////////////////////////////////////
    // 界面管理
    UI::IButton*  m_pBtnUp;
    UI::IButton*  m_pBtnDown;
    UI::IListBox*  m_pPluginBtnListBox;

    //CMainDlg*  m_pMainDlg;
    SkinUI*  m_pSkinUI;
};                            