#include "StdAfx.h"
#include "OptionWindow.h"
#include "MainMgr.h"

//////////////////////////////////////////////////////////////////////////

COptionWindow::COptionWindow()
{
	m_pListOption = NULL;
	m_pComboBox = NULL;
	m_pPanelAbout = NULL;
	m_pPanelCtrlDemo = NULL;
	m_pPanelRichEditDemo = NULL;
	m_pIntroduceRichEdit = NULL;
	m_pPanelVisualization = NULL;
	m_pComboboxVisualType = NULL;
}

BOOL COptionWindow::PreTranslateMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT* pRet)
{
	MSG msg = {m_hWnd, uMsg, wParam, lParam};
	if (UI_IsDialogMessage(&msg))
	{
		*pRet = 0;
		return TRUE;
	}
	return FALSE;
}

BOOL COptionWindow::OnInitDialog( HWND, LPARAM )
{
	m_pPanelAbout = (Panel*)this->FindChildObject(_T("panel_about"));
	m_pPanelVisualization = (Panel*)this->FindChildObject(_T("panel_visualization"));

// 	m_pPanelRichEditDemo = (Panel*)this->FindChildObject(_T("richedit_demo_panel"));
// 	m_pPanelCtrlDemo = (Panel*)this->FindChildObject(_T("control_demo_panel"));
	m_pIntroduceRichEdit = (RichEdit*)this->FindChildObject(_T("option_introduce_text"));

	if (NULL != m_pPanelCtrlDemo)
		m_pPanelCtrlDemo->SetVisible(false, false);
	if (NULL != m_pPanelRichEditDemo)
		m_pPanelRichEditDemo->SetVisible(false, false);
	if (NULL != m_pPanelAbout)
		m_pPanelAbout->SetVisible(false,false);
	if (NULL != m_pPanelVisualization)
		m_pPanelVisualization->SetVisible(false, false);

	//////////////////////////////////////////////////////////////////////////

	if (NULL != m_pIntroduceRichEdit)
	{
//		m_pIntroduceRichEdit->GetRichEdit().SetReadOnly(true);
		m_pIntroduceRichEdit->GetRichEdit().SetSelBarWidth(10);
		m_pIntroduceRichEdit->GetRichEdit().SetRichTextFlag(true);

		const String strText = 
L"    һ�������š���Ч��ת������ʵȶ��ֹ�����һ���רҵ��Ƶ���������\r\n\
\r\n\
    ӵ�������з���ȫ����Ƶ���棬֧��DirectSound���ں���Ƶ��(Kernel Streaming)��ASIO��Ƶ�������AddIn�����չ������������Դռ�õ͡�����Ч�ʸߡ���չ����ǿ���ŵ㡣\r\n\
\r\n\
    ֧��MP3/mp3PRO��AAC/AAC+��M4A/MP4��WMA��APE��MPC��OGG��WAVE��CD��FLAC��RM��TTA��AIFF��AU����Ƶ��ʽ�Լ�����MOD��MIDI���֣�֧��CUE���������ļ���֧�����и�ʽ��WAVE��MP3��APE��WMA�ȸ�ʽ��ת����ͨ������COM�ӿڵ�AddIn�������֧�ָ����ʽ�Ĳ��ź�ת����\r\n\
\r\n\
    ֧�ֲ���Ƶ��ת��(SSRC)�Ͷ��ֱ��������ʽ��֧�ֻط����棬֧��10���ξ��������༶�űȻ��ơ����뵭����Ч�����ݲ���ͬʱ������Winamp2����Ч�����\r\n\
\r\n\
  ֧��ID3v1/v2��WMA��RM��APE��Vorbis��ǩ��֧�������޸ı�ǩ���Ա�ǩ�������ļ���\r\n\
\r\n\
    ֧��ͬ����ʹ�����ʾ���϶���λ���ţ�����֧�����߸�������͸�ʱ༭���ܡ�\r\n\
\r\n\
    ֧�ֶಥ���б����Ƶ�ļ�������֧�ֶ����Ӿ�Ч��������XML��ʽ��ZIPѹ����Ƥ����ͬʱ���д��Դ��ڡ���͸��/���뵭�����ڡ�������Ӱ��������ͼ�ꡢ�Զ����ݼ�����Ϣ�������˵�������ʾ�ȹ��ܡ�\r\n\
\r\n\
    �������������ע�ᣬҲ�������κι��ܻ�ʱ�����ơ�";

		m_pIntroduceRichEdit->GetRichEdit().SetText(strText.c_str());
	}

	//////////////////////////////////////////////////////////////////////////
	const CConfigData* pConfigData = GetMainMgr()->GetConfigData();

	//////////////////////////////////////////////////////////////////////////

	if (NULL != m_pPanelVisualization)
	{
		m_pComboboxVisualType = (Combobox*)m_pPanelVisualization->FindChildObject(_T("combobox_visualization_type"));
		if (NULL != m_pComboboxVisualType)
		{
			m_pComboboxVisualType->GetListBox()->SetSortCompareProc(NULL);
			m_pComboboxVisualType->AddStringEx(_T("<����ʾ>"))->SetData((void*)VISUALIZATION_NONE);
			m_pComboboxVisualType->AddStringEx(_T("�λ��ǿ�"))->SetData((void*)VISUALIZATION_NONE);
			m_pComboboxVisualType->AddStringEx(_T("Ƶ�׷���"))->SetData((void*)VISUALIZATION_SPECTRUM);
			m_pComboboxVisualType->AddStringEx(_T("ʾ����ʾ"))->SetData((void*)VISUALIZATION_WAVE);
			m_pComboboxVisualType->AddStringEx(_T("ר������"))->SetData((void*)VISUALIZATION_NONE);

			m_pComboboxVisualType->ModifyStyle(COMBOBOX_STYLE_DROPDOWNLIST);

			switch(pConfigData->visual.m_nType)
			{
			case VISUALIZATION_SPECTRUM:
				m_pComboboxVisualType->SetCurSel(2);
				break;

			case VISUALIZATION_WAVE:
				m_pComboboxVisualType->SetCurSel(3);
				break;

			default:
				m_pComboboxVisualType->SetCurSel(0);
				break;
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	
// 	if (NULL != m_pPanelRichEditDemo)
// 	{
// 		m_pPanelRichEditDemo->SetVisible(false, false);
// 	}

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
				switch(i)
				{
				case 0:
					pItem->SetData(m_pPanelAbout);
					break;
				case 4:
					pItem->SetData(m_pPanelVisualization);
					break;
				case 13:
					pItem->SetData(m_pPanelRichEditDemo);
					break;
				case 14:
					pItem->SetData(m_pPanelCtrlDemo);
					break;
				}
			}
		}

		m_pListOption->SetSel(0);
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
	if (m_bDoModal)
	{
		this->EndDialog(IDCANCEL);
	}
	else
	{
		this->HideWindow();
	}
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
void COptionWindow::OnLCNSelChanged(Message* pObjMsgFrom, ListItemBase* pOldSelItem, ListItemBase* pSelItem)
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

void COptionWindow::OnCbnSelChanged(Message* pObjMsgFrom, ListItemBase* pOldSelItem, ListItemBase* pSelItem)
{
	if (m_pComboboxVisualType == pObjMsgFrom)
	{
		if (NULL != pSelItem)
		{
			int nType = (int)(pSelItem->GetData());
			GetMainMgr()->SetVisualizationType(nType);
		}
	}
}