#pragma once
#include "UISDK\Kernel\Inc\Interface\ilistitembase.h"
#include "App\im\IM_Ctrls\inc\ipluginbtnlistboxitem.h"
#include "UISDK\Kernel\Inc\Util\iimage.h"
#include "UISDK\Kernel\Inc\Interface\ilistctrlbase.h"

namespace UI
{
    interface ICheckButton;
    interface IListCtrlBase;
}

// 由于这里直接采用ListBox类，而不去新增加一个列表控件，因此一些额外的数据
// 用PluginBtnListBoxData保存在ListBox的UserData中进行共享。
class PluginBtnListBoxShareData : public UI::MessageProxy
{
public:
    PluginBtnListBoxShareData();
    ~PluginBtnListBoxShareData();

    UI_BEGIN_MSG_MAP
        
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(PluginBtnListBoxShareData, UI::IListItemTypeShareData)

    void  SetIPluginBtnListBoxShareData(IPluginBtnListBoxShareData* p)
    { m_pIPluginBtnListBoxShareData = p; SetIMessageProxy(static_cast<UI::IMessage*>(p)); }

public:
    void  Init(UI::IListCtrlBase*);
    UI::IRenderBase*  m_pOddForegndRender;

private:
    UI::IListCtrlBase*  m_pIListCtrl;
    IPluginBtnListBoxShareData*  m_pIPluginBtnListBoxShareData;
};

class PluginBtnListBoxItem : public UI::MessageProxy
{
public:
    PluginBtnListBoxItem();
    ~PluginBtnListBoxItem();

    UI_BEGIN_MSG_MAP
        UIMSG_WM_PAINT(OnPaint)
        UIMSG_WM_INITIALIZE(OnInitialize)
    UIALT_MSG_MAP(1)
        UIMSG_BN_CLICKED3(OnBtnClicked)
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(PluginBtnListBoxItem, UI::IListItemBase)

public:
    void  SetIPluginBtnListBoxItem(IPluginBtnListBoxItem* p) {m_pIPluginBtnListBoxItem = p;  SetIMessageProxy(static_cast<UI::IMessage*>(p));};

    void  SetIconPath(const TCHAR*  szPath);
    void  SetIcon(HBITMAP hBitmap);
    void  SetSortValue(int n) { m_nSortValue = n; }
    int   GetSortValue() { return m_nSortValue; }
    void  SetCheck(bool b);

protected:
    void  OnPaint(UI::IRenderTarget* p);
    void  OnInitialize();
    void  OnBtnClicked(UI::IMessage*  pMsgFrom);
private:
    IPluginBtnListBoxItem*  m_pIPluginBtnListBoxItem;
    UI::ICheckButton*  m_pCheckBtn;
    
    UI::IImage m_icon;
    PluginBtnListBoxShareData*  m_pShareData;
    int  m_nSortValue;
};