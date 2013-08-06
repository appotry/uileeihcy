#ifndef IQQLOGINACCOUNTCOMBOBOX_H_10ECAA9D_D4A9_4023_9A9D_2B54F6792C97
#define IQQLOGINACCOUNTCOMBOBOX_H_10ECAA9D_D4A9_4023_9A9D_2B54F6792C97
#include "UISDK\Control\Inc\Interface\icombobox.h"

// {87E0914A-4B11-448d-9AF9-F8D1F93FA77D}
static const GUID IID_UI_IQQLoginAccountComboBox = 
{ 0x87e0914a, 0x4b11, 0x448d, { 0x9a, 0xf9, 0xf8, 0xd1, 0xf9, 0x3f, 0xa7, 0x7d } };
// {5742FC6D-2492-4f2b-BF88-8D5D6A2AA478}
static const GUID CLSID_UI_QQLoginAccountComboBox = 
{ 0x5742fc6d, 0x2492, 0x4f2b, { 0xbf, 0x88, 0x8d, 0x5d, 0x6a, 0x2a, 0xa4, 0x78 } };


class QQLoginAccountComboBox;
interface IM_CTRLS_API IQQLoginAccountComboBox : public UI::IComboBoxBase
{
    UI_DECLARE_Ixxx_INTERFACE(IQQLoginAccountComboBox, QQLoginAccountComboBox);

    void  AddAccount(const TCHAR* szId, const TCHAR* szName, const TCHAR* szFaceImgPath);
    bool  SetCurSel(int nIndex);
};

#endif