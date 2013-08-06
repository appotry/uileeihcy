#ifndef IHEADERCTRL_H_A101851E_A3A9_4db0_8A64_D1C9A87A1ABA
#define IHEADERCTRL_H_A101851E_A3A9_4db0_8A64_D1C9A87A1ABA

#include "UISDK\Kernel\Inc\Interface\icontrol.h"
#include "UISDK\Kernel\Inc\Interface\ilistctrlbase.h"
#include "UISDK\Kernel\Inc\Interface\ilistitembase.h"

namespace UI
{

    // Message/Notify

    // 通知开始/正在/结束拖拽分隔条
    // Notifies a header control's parent window that the user is dragging a divider in the header control
    //       message: UI_WM_NOTIFY
    //       code:    UI_HDCN_BEGIN_TRACK
    //       wparam:  nColumnIndex
    //       lparam:
    //       Return: Returns FALSE（默认允许）to allow tracking of the divider, or TRUE to prevent tracking. 
#define UI_HDCN_BEGIN_TRACK 136131135

        //       message: UI_WM_NOTIFY
        //       code:    UI_HDCN_TRACKING
        //       wparam:  nColumnIndex
        //       lparam:  int* [in,out] pnColumnNewWidth
        //       return:  BOOL, TRUE表示已处理；FALSE表示未处理，内部将自己来刷新
#define UI_HDCN_TRACKING 136131136

        // 同UI_HDCN_BEGIN_TRACK，无返回值
#define UI_HDCN_END_TRACK 136131137

        // 双击分隔条
        // Notifies a header control's parent window that the user double-clicked the divider area of the control. 
        //		message: UI_WM_NOTIFY
        //      code:    UI_HDCN_DIVIDER_DBLCLICK
        //      wparam:  nColumnIndex
        //      lparam:
        //      return:
        //
#define UI_HDCN_DIVIDER_DBLCLICK 136131138

        // header ctrl宽度发生改变的通知消息
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

    // 获取header item的信息，由item负责填充ColumnInfo*各字段
    // WPARAM: ColumnInfo*
#define  UI_HDCM_GETITEM_COLUMNINFO 136131150

    // 其它定义
#define HEADER_DIVIDER_EXTENT_SIZE 10  // 拖拽宽度



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