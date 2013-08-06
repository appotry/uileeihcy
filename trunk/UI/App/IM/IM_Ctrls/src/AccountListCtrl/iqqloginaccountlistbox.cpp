#include "stdafx.h"
#include "App\IM\IM_Ctrls\inc\iqqloginaccountlistbox.h"
#include "App\IM\IM_Ctrls\src\AccountListCtrl\qqloginaccountlistitem.h"
#include "App\IM\IM_Ctrls\src\AccountListCtrl\qqloginaccountlistbox.h"

UI_IMPLEMENT_Ixxx_INTERFACE2(IQQLoginAccountListItem, QQLoginAccountListItem, IListItemBase)
UI_IMPLEMENT_Ixxx_INTERFACE2(IQQLoginAccountListBox, QQLoginAccountListBox, IListBoxBase)

void  IQQLoginAccountListItem::SetName(const TCHAR*  szName)            { m_pQQLoginAccountListItemImpl->SetName(szName); }
void  IQQLoginAccountListItem::SetAccount(const TCHAR*  szAccount)      { m_pQQLoginAccountListItemImpl->SetAccount(szAccount); }
void  IQQLoginAccountListItem::SetFaceImgFilePath(const TCHAR*  szFace) { m_pQQLoginAccountListItemImpl->SetFaceImgFilePath(szFace); }

const TCHAR*  IQQLoginAccountListItem::GetName()    { return m_pQQLoginAccountListItemImpl->GetName(); }
const TCHAR*  IQQLoginAccountListItem::GetAccount() { return m_pQQLoginAccountListItemImpl->GetAccount(); }

int   IQQLoginAccountListItem::GetHeight() { return m_pQQLoginAccountListItemImpl->GetHeight(); }
void  IQQLoginAccountListItem::GetHeightRef(int** pp) { m_pQQLoginAccountListItemImpl->GetHeightRef(pp); }
void  IQQLoginAccountListItem::SetHeight(int n) { m_pQQLoginAccountListItemImpl->SetHeight(n); }
UI::IRenderBitmap*  IQQLoginAccountListItem::GetFaceBitmap() { return m_pQQLoginAccountListItemImpl->GetFaceBitmap(); }

IQQLoginAccountListItem*  IQQLoginAccountListBox::AddAccount(const TCHAR* szId, const TCHAR* szName, const TCHAR* szFaceImgPath)
{ return m_pQQLoginAccountListBoxImpl->AddAccount(szId, szName, szFaceImgPath) ;}
void  IQQLoginAccountListBox::SetBindObject(IObject* pBindObj) { m_pQQLoginAccountListBoxImpl->SetBindObject(pBindObj); }