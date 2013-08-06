// IM_Ctrls.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "App\IM\IM_Ctrls\src\AccountListCtrl\qqloginaccountlistbox.h"
#include "App\IM\IM_Ctrls\src\AccountComboBox\QQLoginAccountCombobox.h"
#include "App\IM\IM_Ctrls\src\Contactlist\contactlist.h"

extern "C" 
{
    __declspec(dllexport) bool RegisterUIObject(UI::IUIApplication* p)
	{
        REGISTER_UI_OBJECT(p, QQLoginAccountListBox);
        REGISTER_UI_OBJECT(p, QQLoginAccountComboBox);
        REGISTER_UI_OBJECT(p, ContactList)
		return true;
	}

	// ����ֱ����IM_UI���̵��õĵ����ӿ�
	bool IM_Ctrl_RegisterUIObject(UI::IUIApplication* p)
	{
		return RegisterUIObject(p);
	}
}
