#include "StdAfx.h"
#include "OptionWindow.h"
#include "App\Player\Player_UI\Mgr\MainMgr.h"
#include "UISDK\Control\Inc\Interface\icombobox.h"
#include "UISDK\Control\Inc\Interface\irichedit.h"
#include "UISDK\Control\Inc\Interface\isliderctrl.h"
#include "UISDK\Control\Inc\Interface\icombobox.h"
#include "UISDK\Kernel\Inc\Interface\ilistitembase.h"
#include "UISDK\Control\Inc\Interface\ilabel.h"
#include "UISDK\Control\Inc\Interface\ilistbox.h"
#include "UISDK\Control\Inc\Interface\icheckbutton.h"

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
	m_pSliderVisualFps = NULL;
	m_pLabelVisualFps = NULL;
    m_pPanelCommon = NULL;
    m_pCheckBtnWndTopMost = NULL;
}

BOOL COptionWindow::OnInitDialog( HWND, LPARAM )
{
	m_pPanelAbout = (UI::IPanel*)this->FindChildObject(_T("panel_about"));
	m_pPanelVisualization = (UI::IPanel*)this->FindChildObject(_T("panel_visualization"));
    m_pPanelCommon = (UI::IPanel*)this->FindChildObject(_T("panel_common"));

// 	m_pPanelRichEditDemo = (UI::IPanel*)this->FindChildObject(_T("richedit_demo_panel"));
 	m_pPanelCtrlDemo = (UI::IPanel*)this->FindChildObject(_T("control_demo_panel"));
	m_pIntroduceRichEdit = (UI::IRichEdit*)this->FindChildObject(_T("option_introduce_text"));

	if (m_pPanelCtrlDemo)
		m_pPanelCtrlDemo->SetVisible(false, false, false);
	if (m_pPanelRichEditDemo)
		m_pPanelRichEditDemo->SetVisible(false, false, false);
	if (m_pPanelAbout)
		m_pPanelAbout->SetVisible(false,false, false);
	if (m_pPanelVisualization)
		m_pPanelVisualization->SetVisible(false, false, false);
    if (m_pPanelCommon)
        m_pPanelCommon->SetVisible(false, false, false);
	//////////////////////////////////////////////////////////////////////////

	if (m_pIntroduceRichEdit)
	{
//		m_pIntroduceRichEdit->GetRichEdit().SetReadOnly(true);
		m_pIntroduceRichEdit->SetSelBarWidth(10);
		m_pIntroduceRichEdit->SetRichTextFlag(true);

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

		m_pIntroduceRichEdit->SetText(strText.c_str());
	}

	//////////////////////////////////////////////////////////////////////////
	const CConfigData* pConfigData = GetConfigData();

	//////////////////////////////////////////////////////////////////////////

	if (m_pPanelVisualization)
	{
		m_pComboboxVisualType = (UI::IComboBox*)m_pPanelVisualization->FindChildObject(_T("combobox_visualization_type"));
		m_pSliderVisualFps = (UI::ISliderCtrl*)m_pPanelVisualization->FindChildObject(_T("slider_visualization_fps"));
		m_pLabelVisualFps = (UI::ILabel*)m_pPanelVisualization->FindChildObject(_T("label_visualizatoin_fps"));

		if (m_pComboboxVisualType)
		{
			m_pComboboxVisualType->GetListBox()->SetSortCompareProc(NULL);
			m_pComboboxVisualType->AddStringEx(_T("<����ʾ>"))->SetData((LPARAM)VISUALIZATION_NONE);
			m_pComboboxVisualType->AddStringEx(_T("�λ��ǿ�"))->SetData((LPARAM)VISUALIZATION_NONE);
			m_pComboboxVisualType->AddStringEx(_T("Ƶ�׷���"))->SetData((LPARAM)VISUALIZATION_SPECTRUM);
			m_pComboboxVisualType->AddStringEx(_T("ʾ����ʾ"))->SetData((LPARAM)VISUALIZATION_WAVE);
			m_pComboboxVisualType->AddStringEx(_T("ר������"))->SetData((LPARAM)VISUALIZATION_NONE);

			m_pComboboxVisualType->SetReadOnly(true);

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
		if (m_pSliderVisualFps)
		{
			m_pSliderVisualFps->SetRange(10,40,false);
			m_pSliderVisualFps->SetTickFreq(1);
			m_pSliderVisualFps->SetPos(pConfigData->visual.m_nFps);
			this->OnVisualFpsChanged(pConfigData->visual.m_nFps, 0);
			m_pSliderVisualFps->ModifyStyle(SLIDER_STYLE_AUTOTICKS,0, false);
		}
	}

    if (m_pPanelCommon)
    {
        m_pCheckBtnWndTopMost = (UI::ICheckButton*)m_pPanelCommon->FindChildObject(_T("checkbtn_topmost"));

        if (m_pCheckBtnWndTopMost)
        {
            m_pCheckBtnWndTopMost->SetCheck(pConfigData->player.m_bWndTopMost? BST_CHECKED:BST_UNCHECKED);
        }
    }
	//////////////////////////////////////////////////////////////////////////
	
// 	if (m_pPanelRichEditDemo)
// 	{
// 		m_pPanelRichEditDemo->SetVisible(false, false);
// 	}

    m_pListOption = (UI::IListBox*)this->FindChildObject(_T("optionlist") );
	if (m_pListOption)
	{
		m_pListOption->ModifyStyleEx(0, LISTCTRLBASE_STYLE_SORT_ASCEND|LISTCTRLBASE_STYLE_SORT_DESCEND, false);

		String strArray[] = { _T("����"),_T("����"),_T("����"),_T("��ݼ�"),_T("�Ӿ�Ч��"),
			_T("�����б�"),_T("ý���"),_T("�����"),_T("�������"),_T("��������"),_T("��Ч���"),
			_T("��Ƶ�豸"),_T("Ƥ��"),_T("ȫ����ʾ"),_T("ϵͳ����")};
		int nCount = sizeof(strArray)/sizeof(String);
		for (int i = 0; i < nCount; i++)
		{
            UI::IListBoxItem* pItem = m_pListOption->AddString(strArray[i].c_str(), LISTITEM_OPFLAG_NOALL);
			if (pItem)
			{
				switch(i)
				{
				case 0:  // ����
					pItem->SetData((LPARAM)m_pPanelAbout);
					break;
                case 1:  // ����
                    pItem->SetData((LPARAM)m_pPanelCommon);
                    break;
				case 4: // �Ӿ�Ч��
					pItem->SetData((LPARAM)m_pPanelVisualization);
					break;
				case 13:
					pItem->SetData((LPARAM)m_pPanelRichEditDemo);
					break;
				case 14:
					pItem->SetData((LPARAM)m_pPanelCtrlDemo);
					break;
				}
			}
		}
		m_pListOption->UpdateItemRect();
		m_pListOption->SetSel(0);
	}

	m_pComboBox = (UI::IComboBox*)this->FindChildObject(_T("test_combo1"));
	if (m_pComboBox)
	{
		m_pComboBox->AddString(_T("123456"));
		m_pComboBox->AddString(_T("123456"));
		m_pComboBox->AddString(_T("123456"));
		m_pComboBox->AddString(_T("123456"));
	}

	CComboBox box = ::GetDlgItem(GetHWND(), IDC_COMBO1);
	box.AddString(_T("11234"));box.AddString(_T("11234"));box.AddString(_T("11234"));
	box.AddString(_T("11234"));box.AddString(_T("11234"));box.AddString(_T("11234"));

	return FALSE;
}
void COptionWindow::OnClose()
{
	if (IsDoModal())
	{
		this->EndDialog(IDCANCEL);
	}
	else
	{
        // ������������ǰ����
        HWND hWndParent = ::GetWindow(GetHWND(), GW_OWNER);
        if ( hWndParent!= NULL && ::GetActiveWindow() == GetHWND())
            ::SetActiveWindow(hWndParent);

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
	IObject* pObj = this->FindChildObject(_T("test_check"));
	if (pObj)
		pObj->SetVisible(!pObj->IsVisible());
}

void COptionWindow::OnBtnClick2(UINT uNotifyCode, int nID, HWND wndCtl)
{
	IObject* pObj = this->FindChildObject(_T("groupbox1"));
	if (pObj)
		pObj->SetVisible(!pObj->IsVisible());
}
void COptionWindow::OnRButtonUp(UINT nFlags, CPoint point)
{

}
void COptionWindow::OnLCNSelChanged(UI::IMessage* pObjMsgFrom, UI::IListItemBase* pOldSelItem, UI::IListItemBase* pSelItem)
{
	if (pOldSelItem)
	{
		UI::IPanel* pPanel = (UI::IPanel*)pOldSelItem->GetData();
		if (pPanel)
		{
			pPanel->SetVisible(false, true);
		}
	}
	if (pSelItem)
	{
		UI::IPanel* pPanel = (UI::IPanel*)pSelItem->GetData();
		if (pPanel)
		{
			pPanel->SetVisible(true, true);
		}
	}
}

void COptionWindow::OnCbnSelChanged(UI::IListItemBase* pOldSelItem, UI::IListItemBase* pSelItem)
{
	if (pSelItem)
	{
		int nType = (int)(pSelItem->GetData());
		GetMainMgr()->SetVisualizationType(nType);
	}
}

void COptionWindow::OnVisualFpsChanged( int nPos, int nScrollType )
{
	if (nScrollType != SB_ENDSCROLL)
	{
		if (m_pLabelVisualFps)
		{	
			TCHAR szInfo[8] = _T("");
			_itot(nPos, szInfo, 10);
			m_pLabelVisualFps->SetText(szInfo);
		}

		::GetMainMgr()->SetVisualizationFps(nPos);
	}
}

void COptionWindow::OnClickBtnWndTopMost()
{
    bool bTopMost = m_pCheckBtnWndTopMost->IsChecked();
    GetConfigData()->player.m_bDirty = true;
    GetConfigData()->player.m_bWndTopMost = bTopMost;

    GetFrameWork()->FireEvent(NULL, EVENT_TYPE_UI, UI_EVENT_ID_ON_WNDTOPMOST_CHANGED, 0, bTopMost?1:0);
}