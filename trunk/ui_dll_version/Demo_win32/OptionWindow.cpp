#include "StdAfx.h"
#include "OptionWindow.h"


//////////////////////////////////////////////////////////////////////////

COptionWindow::COptionWindow()
{
	m_pListOption = NULL;
	m_pComboBox = NULL;
	m_pPanelCtrlDemo = NULL;
	m_pPanelRichEditDemo = NULL;
}


BOOL COptionWindow::OnInitDialog( HWND, LPARAM )
{
	m_pPanelRichEditDemo = (Panel*)this->FindChildObject(_T("richedit_demo_panel"));
	m_pPanelCtrlDemo = (Panel*)this->FindChildObject(_T("control_demo_panel"));

	if (NULL != m_pPanelCtrlDemo)
	{
		m_pPanelCtrlDemo->SetVisible(false, false);
	}
	if (NULL != m_pPanelRichEditDemo)
	{
		m_pPanelRichEditDemo->SetVisible(false, false);
	}

	m_pListOption = (ListBox*)this->FindChildObject(_T("optionlist") );
	if (NULL != m_pListOption)
	{
		m_pListOption->ModifyStyle( 0, LISTCTRLBASE_SORT_ASCEND|LISTCTRLBASE_SORT_DESCEND);
		m_pListOption->SetItemHeight(24, false);

		String strArray[] = { _T("关于"),_T("常规"),_T("播放"),_T("快捷键"),_T("视觉效果"),
			_T("播放列表"),_T("媒体库"),_T("歌词秀"),_T("歌词搜索"),_T("网络连接"),_T("音效插件"),
			_T("音频设备"),_T("皮肤"),_T("全屏显示"),_T("系统关联")};
		int nCount = sizeof(strArray)/sizeof(String);
		for (int i = 0; i < nCount; i++)
		{
			ListBoxItem* pItem = m_pListOption->AddString(strArray[i]);
			if (NULL != pItem)
			{
				if (0 == i)
				{
					pItem->SetData(m_pPanelRichEditDemo);
				}
				else if (1 == i)
				{
					pItem->SetData(m_pPanelCtrlDemo);
				}
			}
		}
	}

	m_pComboBox = (Combobox*)this->FindChildObject(_T("test_combo1"));
	if (NULL != m_pComboBox)
	{
		m_pComboBox->AddString(_T("123456"));
		m_pComboBox->AddString(_T("123456"));
		m_pComboBox->AddString(_T("123456"));
		m_pComboBox->AddString(_T("123456"));
	}

	CComboBox box = ::GetDlgItem(m_hWnd, IDC_COMBO1);
	box.AddString(_T("11234"));box.AddString(_T("11234"));box.AddString(_T("11234"));
	box.AddString(_T("11234"));box.AddString(_T("11234"));box.AddString(_T("11234"));

	return FALSE;
}
void COptionWindow::OnClose()
{
	this->EndDialog(IDCANCEL);
}
void COptionWindow::OnDestroy()
{
}

void COptionWindow::OnBtnSaveAll()
{
}
void COptionWindow::OnBtnResetAll()
{
}
void COptionWindow::OnBtnCancel()
{
	this->OnClose();	
}

void COptionWindow::OnBtnClick1(UINT uNotifyCode, int nID, HWND wndCtl)
{
	Object* pObj = this->FindChildObject(_T("test_check"));
	if( NULL != pObj )
	{
		pObj->SetVisible(!pObj->IsVisible());
	}
}

void COptionWindow::OnBtnClick2(UINT uNotifyCode, int nID, HWND wndCtl)
{
	Object* pObj = this->FindChildObject(_T("groupbox1"));
	if( NULL != pObj )
	{
		pObj->SetVisible(!pObj->IsVisible());
	}
}
void COptionWindow::OnRButtonUp(UINT nFlags, CPoint point)
{

}
void COptionWindow::OnLCNSelChanged(ListItemBase* pOldSelItem, ListItemBase* pSelItem)
{
	if (NULL != pOldSelItem)
	{
		Panel* pPanel = (Panel*)pOldSelItem->GetData();
		if (NULL != pPanel)
		{
			pPanel->SetVisible(false, true);
		}
	}
	if (NULL != pSelItem)
	{
		Panel* pPanel = (Panel*)pSelItem->GetData();
		if (NULL != pPanel)
		{
			pPanel->SetVisible(true, true);
		}
	}
}