#include "stdafx.h"
#include "App\IM\IM_Ctrls\inc\iqqloginaccountcombobox.h"
#include "App\IM\IM_Ctrls\src\AccountComboBox\qqloginaccountcombobox.h"

UI_IMPLEMENT_Ixxx_INTERFACE2(IQQLoginAccountComboBox, QQLoginAccountComboBox, IComboBoxBase);


void  IQQLoginAccountComboBox::AddAccount(const TCHAR* szId, const TCHAR* szName, const TCHAR* szFaceImgPath)
{ m_pQQLoginAccountComboBoxImpl->AddAccount(szId, szName, szFaceImgPath); } 
bool  IQQLoginAccountComboBox::SetCurSel(int nIndex){ return m_pQQLoginAccountComboBoxImpl->SetCurSel(nIndex); }