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

		String strArray[] = { _T("����"),_T("����"),_T("����"),_T("��ݼ�"),_T("�Ӿ�Ч��"),
			_T("�����б�"),_T("ý���"),_T("�����"),_T("�������"),_T("��������"),_T("��Ч���"),
			_T("��Ƶ�豸"),_T("Ƥ��"),_T("ȫ����ʾ"),_T("ϵͳ����")};
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