#pragma once
#include "UISDK\Control\Inc\Interface\ilistview.h"
#include "UISDK\Control\Inc\Interface\iheaderctrl.h"

// TODO: �Ȳ�����֧��listview column��ק��Ŀǰû���뵽�������������ݽṹ
namespace UI
{

struct DrawSubItemData
{
    int               nColumn;
    IRenderTarget*    pRenderTarget;
    ITextRenderBase*  pTextRender;
    ColumnInfo*       pInfo;
    const CRect*      prc;
    int               nRenderState;
};

struct SubItemInfo
{
    String  m_strText;
};

class ListViewItemBaseShareData : public MessageProxy
{
public:
    ListViewItemBaseShareData();
    ~ListViewItemBaseShareData();

    UI_BEGIN_MSG_MAP
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(ListViewItemBaseShareData, IListItemTypeShareData)

    void  SetIListViewItemBaseShareData(IListViewItemBaseShareData* p)
    { m_pIListViewItemBaseShareData = p; SetIMessageProxy(static_cast<IMessage*>(p)); }

public:
    IListViewItemBaseShareData*  m_pIListViewItemBaseShareData;
    IColumnsInfo*  pColumnInfo;
};

class ListViewItemBase : public MessageProxy
{
public:
    ListViewItemBase();
    ~ListViewItemBase();	

    UI_BEGIN_MSG_MAP
        UIMSG_WM_PAINT(OnPaint)
        UIMESSAGE_HANDLER_EX(UI_LVIM_SETSUBITEMTEXT, OnSetSubItemText)
        UIMESSAGE_HANDLER_EX(UI_LVIM_GETSUBITEMTEXT, OnGetSubItemText)
        UIMESSAGE_HANDLER_EX(UI_LVIM_ADDCOLUMN,      OnAddColumn)
        UIMESSAGE_HANDLER_EX(UI_LVIM_REMOVECOLUMN,   OnRemoveColumn)
        UIMESSAGE_HANDLER_EX(UI_LVIM_CLEARCOLUMN,    OnClearColumn)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_INITIALIZE(OnInitialize)
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(ListViewItemBase, IListItemBase)

    UI_DECLARE_QUERY_INTERFACE(ListViewItemBase);

public:
    void  SetIListViewItemBase(IListViewItemBase* p) 
    { m_pIListViewItemBase = p; SetIMessageProxy(static_cast<IMessage*>(p)); }

protected:
    void  OnInitialize();
    void  OnPaint(IRenderTarget* pRenderTarget);
    LRESULT  OnSetSubItemText(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT  OnGetSubItemText(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT  OnAddColumn(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT  OnRemoveColumn(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT  OnClearColumn(UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
    void  OnDrawSubItem(DrawSubItemData* pSubItem);
    void  DestroySubItems();

protected:
    IListViewItemBase*  m_pIListViewItemBase;
    ListViewItemBaseShareData*  m_pShareData;

    UINT  m_nSubItemCount;
    SubItemInfo**  m_ppSubItem;  // ��0��Ԫ���ǵڶ��У���һ����Ϊ��Ԫ�أ�ֱ��ʹ��listitembase.text
};

}
