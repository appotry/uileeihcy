#pragma once
#include "UISDK\Control\Inc\Interface\imenu.h"

namespace UI
{

class MenuStringItemShareData : public MessageProxy 
{
public:
    MenuStringItemShareData();
    ~MenuStringItemShareData();

    UI_BEGIN_MSG_MAP
        UIMSG_WM_RESETATTRIBUTE(ResetAttribute)
        UIMSG_WM_SETATTRIBUTE(SetAttribute)
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(MenuStringItemShareData, IListItemTypeShareData)

    void  SetIMenuStringItemShareData(IMenuStringItemShareData* p)
    { m_pIMenuStringItemShareData = p; SetIMessageProxy(static_cast<IMessage*>(p)); }

protected:
    void  ResetAttribute();
    void  SetAttribute(IMapAttribute* pMapAttr, bool bReload);

public:
    IMenuStringItemShareData*  m_pIMenuStringItemShareData;

    IRenderBase*  m_pRadioIconRender;
    IRenderBase*  m_pCheckIconRender;
    IRenderBase*  m_pRadioCheckIconBkRender;

    int  nIconGutterWidth;
    int  nPopupTriangleWidth;
    int  nTextIndentLeft;
    int  nTextIndentRight;
};

class MenuStringItem : public MessageProxy
{
public:
    MenuStringItem();

    UI_BEGIN_MSG_MAP
        UIMSG_WM_PAINT(OnPaint)
        UIMSG_WM_GETDESIREDSIZE(GetDesiredSize)
        UIMSG_WM_INITIALIZE(OnInitialize)
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(MenuStringItem, IListItemBase)

    void  SetIMenuStringItem(IMenuStringItem* p)
    { m_pIMenuStringItem = p; SetIMessageProxy(static_cast<IMessage*>(p)); }

protected:
    void  GetDesiredSize(SIZE* pSize);
    void  OnPaint(IRenderTarget* pRenderTarget);
    void  OnInitialize();

private:
    IMenuStringItem*  m_pIMenuStringItem;
    MenuStringItemShareData*  m_pShareData;

    int nMenuIconGutter;
};


}