#include "stdafx.h"
#include "UISDK\Control\Inc\Interface\iheaderctrl.h"
#include "UISDK\Control\Src\Control\HeaderCtrl\headerctrl.h"
#include "UISDK\Control\Src\Control\HeaderCtrl\headerlistctrl.h"
#include "UISDK\Control\Src\Control\HeaderCtrl\headerlistitem.h"

namespace UI
{
#if 0
UI_IMPLEMENT_Ixxx_INTERFACE2(IHeaderCtrl, HeaderCtrl, IControl)

int   IHeaderCtrl::GetColumnCount() { return m_pHeaderCtrlImpl->GetColumnCount(); }
void  IHeaderCtrl::GetColumnWidthArray(int* pWidthArray) { m_pHeaderCtrlImpl->GetColumnWidthArray(pWidthArray); }

bool  IHeaderCtrl::CanDragColumn() { return m_pHeaderCtrlImpl->CanDragColumn(); }
void  IHeaderCtrl::SetHScrollOffset(int nhScrollOffset) { m_pHeaderCtrlImpl->SetHScrollOffset(nhScrollOffset); }

bool  IHeaderCtrl::AddColumn(const TCHAR* szText, int nWidth, int nFormat) { return m_pHeaderCtrlImpl->AddColumn(szText, nWidth, nFormat); }
bool  IHeaderCtrl::InsertColumn(int nPos, const TCHAR* szText, int nWidth, int nFormat) { return m_pHeaderCtrlImpl->InsertColumn(nPos, szText, nWidth, nFormat); }
#endif

//////////////////////////////////////////////////////////////////////////
UI_IMPLEMENT_Ixxx_INTERFACE2(IHeaderListCtrl, HeaderListCtrl, IListCtrlBase);
UI_IMPLEMENT_Ixxx_INTERFACE2(IHeaderListItem, HeaderListItem, IListItemBase);

IHeaderListItem*  IHeaderListCtrl::AddColumn(const TCHAR* szText, int nWidth, int nFormat) { return m_pHeaderListCtrlImpl->AddColumn(szText, nWidth, nFormat); }
IHeaderListItem*  IHeaderListCtrl::InsertColumn(int nPos, const TCHAR* szText, int nWidth, int nFormat) { return m_pHeaderListCtrlImpl->InsertColumn(nPos, szText, nWidth, nFormat); }

int   IHeaderListCtrl::GetColumnCount()                       { return m_pHeaderListCtrlImpl->GetColumnCount(); }
bool  IHeaderListCtrl::CanDragColumn()                        { return m_pHeaderListCtrlImpl->CanDragColumn(); }
void  IHeaderListCtrl::SetHScrollOffset(int nhScrollOffset)   { m_pHeaderListCtrlImpl->SetHScrollOffset(nhScrollOffset); }
int   IHeaderListCtrl::GetHScrollOffset()                     { return m_pHeaderListCtrlImpl->GetHScrollOffset(); }
IColumnsInfo*  IHeaderListCtrl::GetColumnsInfo()              { return m_pHeaderListCtrlImpl->GetColumnsInfo(); }
int   IHeaderListCtrl::GetColumnTotalWidth()                  { return m_pHeaderListCtrlImpl->GetColumnTotalWidth(); }

void  IHeaderListItem::SetTextAlign(int n)                    { m_pHeaderListItemImpl->SetTextAlign(n); }
int   IHeaderListItem::GetTextAlign()                         { return m_pHeaderListItemImpl->GetTextAlign(); }
}
