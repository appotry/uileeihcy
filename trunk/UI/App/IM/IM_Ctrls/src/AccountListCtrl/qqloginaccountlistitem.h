#pragma once
#include "App\IM\IM_Ctrls\inc\iqqloginaccountlistbox.h"

class QQLoginAccountListItem : public UI::MessageProxy/*ListBoxItem*/
{
public:
    QQLoginAccountListItem();
    ~QQLoginAccountListItem();

    UI_DECLARE_OBJECT3(QQLoginAccountListItem, UI::OBJ_CONTROL|UI::CONTROL_LISTCTRLITEM, _T("IMCtrl"))

    UI_BEGIN_MSG_MAP
//        UIMSG_WM_LBUTTONDOWN(OnLButtonDown)
//        UIMSG_WM_LBUTTONUP(OnLButtonUp)
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(QQLoginAccountListItem, UI::IListItemBase)

    void  SetIQQLoginAccountListItem(IQQLoginAccountListItem* p) { 
        m_pIQQLoginAccountListItem = p; SetIMessageProxy(static_cast<UI::IMessage*>(p)); }

public:
    void  SetName(const TCHAR*  szName);
    void  SetAccount(const TCHAR*  szAccount);
    void  SetFaceImgFilePath(const TCHAR*  szFace);

    const TCHAR*  GetName() { return m_strName.c_str(); }
    const TCHAR*  GetAccount() { return m_strAccount.c_str(); }

    int   GetHeight() { return m_nCurItemHeight; }
    void  GetHeightRef(int** pp);
    void  SetHeight(int n) { m_nCurItemHeight = n; }
    UI::IRenderBitmap*  GetFaceBitmap() { return m_pFaceBitmap; }

protected:
//    void  OnLButtonDown(UINT nFlags, POINT point);
//    void  OnLButtonUp(UINT nFlags, POINT point);

private:
    String  m_strName;
    String  m_strAccount;
    String  m_strFaceImgFilePath;

    UI::IRenderBitmap*  m_pFaceBitmap;
    int   m_nCurItemHeight;

    IQQLoginAccountListItem*  m_pIQQLoginAccountListItem;
};
