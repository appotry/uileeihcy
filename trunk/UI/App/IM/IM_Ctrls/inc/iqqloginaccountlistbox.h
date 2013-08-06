#ifndef IQQLOGINACCOUNTLISTBOX_H_AC0A6AA8_EDFF_4b9e_BD88_20F9743B59CD
#define IQQLOGINACCOUNTLISTBOX_H_AC0A6AA8_EDFF_4b9e_BD88_20F9743B59CD
#include "UISDK\Control\Inc\Interface\ilistbox.h"
#include "UISDK\Kernel\Inc\Interface\ilistitembase.h"

// {5395F4B6-C3B0-463a-9D10-004E3A7AFD84}
static const GUID IID_UI_IQQLoginAccountListItem = 
{ 0x5395f4b6, 0xc3b0, 0x463a, { 0x9d, 0x10, 0x0, 0x4e, 0x3a, 0x7a, 0xfd, 0x84 } };
// {315FEB50-1903-4fcd-8B14-F3D3E7E2C66C}
static const GUID CLSID_UI_QQLoginAccountListItem = 
{ 0x315feb50, 0x1903, 0x4fcd, { 0x8b, 0x14, 0xf3, 0xd3, 0xe7, 0xe2, 0xc6, 0x6c } };

// {38C2D684-40DD-410b-9451-FF83EDDCFB9C}
static const GUID IID_UI_IQQLoginAccountListBox = 
{ 0x38c2d684, 0x40dd, 0x410b, { 0x94, 0x51, 0xff, 0x83, 0xed, 0xdc, 0xfb, 0x9c } };
// {076FCF6A-3B5C-4fcc-9A4B-F2EF4EA1FE1A}
static const GUID CLSID_UI_QQLoginAccountListBox = 
{ 0x76fcf6a, 0x3b5c, 0x4fcc, { 0x9a, 0x4b, 0xf2, 0xef, 0x4e, 0xa1, 0xfe, 0x1a } };

#define ITEM_NORMAL_HEIGHT  24
#define ITEM_NEAR_HEIGHT    33
#define ITEM_HILIGHT_HEIGHT 44

#define XML_QQLoginAccountList_NAMETEXT_RENDER_PREFIX  _T("nametext.")
#define XML_QQLoginAccountList_ERASEBTN_PREFIX         _T("erasebtn.")

class QQLoginAccountListItem;
interface  IQQLoginAccountListItem : public UI::IListItemBase
{
    UI_DECLARE_Ixxx_INTERFACE(IQQLoginAccountListItem, QQLoginAccountListItem);

    void  SetName(const TCHAR*  szName);
    void  SetAccount(const TCHAR*  szAccount);
    void  SetFaceImgFilePath(const TCHAR*  szFace);

    const TCHAR*  GetName();
    const TCHAR*  GetAccount();

    int   GetHeight();
    void  GetHeightRef(int ** pp);
    void  SetHeight(int n);
    UI::IRenderBitmap*  GetFaceBitmap();
};


class QQLoginAccountListBox;
interface IM_CTRLS_API IQQLoginAccountListBox : public UI::IListBoxBase
{
    UI_DECLARE_Ixxx_INTERFACE(IQQLoginAccountListBox, QQLoginAccountListBox);

    IQQLoginAccountListItem*  AddAccount(const TCHAR* szId, const TCHAR* szName, const TCHAR* szFaceImgPath);
    void  SetBindObject(IObject* pBindObj);
};


#endif // IQQLOGINACCOUNTLISTBOX_H_AC0A6AA8_EDFF_4b9e_BD88_20F9743B59CD