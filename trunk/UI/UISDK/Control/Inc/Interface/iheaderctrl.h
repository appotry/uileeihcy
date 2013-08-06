#ifndef IHEADERCTRL_H_A101851E_A3A9_4db0_8A64_D1C9A87A1ABA
#define IHEADERCTRL_H_A101851E_A3A9_4db0_8A64_D1C9A87A1ABA

#include "UISDK\Kernel\Inc\Interface\icontrol.h"
#include "UISDK\Kernel\Inc\Interface\ilistctrlbase.h"
#include "UISDK\Kernel\Inc\Interface\ilistitembase.h"

namespace UI
{

    // Message/Notify

    // ֪ͨ��ʼ/����/������ק�ָ���
    // Notifies a header control's parent window that the user is dragging a divider in the header control
    //       message: UI_WM_NOTIFY
    //       code:    UI_HDCN_BEGIN_TRACK
    //       wparam:  nColumnIndex
    //       lparam:
    //       Return: Returns FALSE��Ĭ������to allow tracking of the divider, or TRUE to prevent tracking. 
#define UI_HDCN_BEGIN_TRACK 136131135

        //       message: UI_WM_NOTIFY
        //       code:    UI_HDCN_TRACKING
        //       wparam:  nColumnIndex
        //       lparam:  int* [in,out] pnColumnNewWidth
        //       return:  BOOL, TRUE��ʾ�Ѵ���FALSE��ʾδ�����ڲ����Լ���ˢ��
#define UI_HDCN_TRACKING 136131136

        // ͬUI_HDCN_BEGIN_TRACK���޷���ֵ
#define UI_HDCN_END_TRACK 136131137

        // ˫���ָ���
        // Notifies a header control's parent window that the user double-clicked the divider area of the control. 
        //		message: UI_WM_NOTIFY
        //      code:    UI_HDCN_DIVIDER_DBLCLICK
        //      wparam:  nColumnIndex
        //      lparam:
        //      return:
        //
#define UI_HDCN_DIVIDER_DBLCLICK 136131138

        // header ctrl��ȷ����ı��֪ͨ��Ϣ
        //      message: UI_WM_NOTIFY
        //      code:    UI_HDCN_TOTAL_WIDTH_CHANGED
        //      wparam:  int nTotalWidth
        //      lparam:
        //      return:
#define UI_HDCN_TOTAL_WIDTH_CHANGED 136131139
        // 
        // 	UI_HDCN_ITEM_CLICK,
        // 
        // 	UI_HDCN_ITEM_DBLCLICK,
        // 
        // 	UI_HDCN_ITEM_RCLICK,

    // ��ȡheader item����Ϣ����item�������ColumnInfo*���ֶ�
    // WPARAM: ColumnInfo*
#define  UI_HDCM_GETITEM_COLUMNINFO 136131150

    // ��������
#define HEADER_DIVIDER_EXTENT_SIZE 10  // ��ק���



struct ColumnInfo
{
    //int nMask;

    int nFormat;
    int nWidth;
    int nIndentLeft;
    int nIndentRight;

    int nOrder;
    IListItemBase*  pHeaderItem;
};


interface IColumnsInfo
{
    virtual UINT  GetCount() = 0;
    virtual ColumnInfo*  GetItemInfo(UINT n) = 0;
};

//////////////////////////////////////////////////////////////////////////
class HeaderListItem;
interface UICTRLAPI IHeaderListItem : public IListItemBase
{
    UI_DECLARE_Ixxx_INTERFACE(IHeaderListItem, HeaderListItem);
    
    void  SetTextAlign(int n);
    int   GetTextAlign();
};

class HeaderListCtrl;
interface UICTRLAPI IHeaderListCtrl : public IListCtrlBase
{
     UI_DECLARE_Ixxx_INTERFACE(IHeaderListCtrl, HeaderListCtrl);

     IHeaderListItem*  AddColumn(const TCHAR* szText, int nWidth, int nFormat=DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX|DT_END_ELLIPSIS);
     IHeaderListItem*  InsertColumn(int nPos, const TCHAR* szText, int nWidth, int nFormat=DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX|DT_END_ELLIPSIS);

     int   GetColumnCount();
     bool  CanDragColumn();
     void  SetHScrollOffset(int nhScrollOffset); 
     int   GetHScrollOffset();
     int   GetColumnTotalWidth();

     IColumnsInfo*  GetColumnsInfo();
};
}

#endif // IHEADERCTRL_H_A101851E_A3A9_4db0_8A64_D1C9A87A1ABA