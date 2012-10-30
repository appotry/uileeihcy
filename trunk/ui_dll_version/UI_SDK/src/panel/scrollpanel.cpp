#include "stdafx.h"

ScrollPanel::ScrollPanel()
{
	m_MgrScrollBar.SetBindObject(this);
}
ScrollPanel::~ScrollPanel()
{

}
void ScrollPanel::ResetAttribute()
{
	__super::ResetAttribute();
	m_MgrScrollBar.ResetAttribute();
}
bool ScrollPanel::SetAttribute(ATTRMAP& mapAttrib, bool bReload)
{
	bool bRet = __super::SetAttribute(mapAttrib, bReload);
	if (false == bRet)
		return false;

	m_MgrScrollBar.SetAttribute(mapAttrib, bReload);
	return true;
}

UINT ScrollPanel::OnHitTest( POINT* pt )
{
	UINT nRet = __super::OnHitTest(pt);
	if(HTBORDER == nRet)
	{
		int nSBHitTest = UISendMessage(&m_MgrScrollBar, UI_WM_HITTEST, (WPARAM)pt->x, (LPARAM)pt->y);
		if(HTNOWHERE == nSBHitTest)
			return nRet;
		else 
			return nSBHitTest;
	}
	return nRet;
}