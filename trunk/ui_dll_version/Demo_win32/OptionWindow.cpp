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
L"    一个集播放、音效、转换、歌词等多种功能于一身的专业音频播放软件。\r\n\
\r\n\
    拥有自主研发的全新音频引擎，支持DirectSound、内核音频流(Kernel Streaming)和ASIO音频流输出、AddIn插件扩展技术，具有资源占用低、运行效率高、扩展能力强等优点。\r\n\
\r\n\
    支持MP3/mp3PRO、AAC/AAC+、M4A/MP4、WMA、APE、MPC、OGG、WAVE、CD、FLAC、RM、TTA、AIFF、AU等音频格式以及多种MOD和MIDI音乐，支持CUE音轨索引文件，支持所有格式到WAVE、MP3、APE、WMA等格式的转换，通过基于COM接口的AddIn插件可以支持更多格式的播放和转换。\r\n\
\r\n\
    支持采样频率转换(SSRC)和多种比特输出方式，支持回放增益，支持10波段均衡器、多级杜比环绕、淡入淡出音效，兼容并可同时激活多个Winamp2的音效插件。\r\n\
\r\n\
  支持ID3v1/v2、WMA、RM、APE和Vorbis标签，支持批量修改标签和以标签重命名文件。\r\n\
\r\n\
    支持同步歌词滚动显示和拖动定位播放，并且支持在线歌词搜索和歌词编辑功能。\r\n\
\r\n\
    支持多播放列表和音频文件搜索，支持多种视觉效果，采用XML格式的ZIP压缩的皮肤，同时具有磁性窗口、半透明/淡入淡出窗口、窗口阴影、任务栏图标、自定义快捷键、信息滚动、菜单功能提示等功能。\r\n\
\r\n\
    真正免费且无需注册，也不存在任何功能或时间限制。";

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
			m_pComboboxVisualType->AddStringEx(_T("<不显示>"))->SetData((void*)VISUALIZATION_NONE);
			m_pComboboxVisualType->AddStringEx(_T("梦幻星空"))->SetData((void*)VISUALIZATION_NONE);
			m_pComboboxVisualType->AddStringEx(_T("频谱分析"))->SetData((void*)VISUALIZATION_SPECTRUM);
			m_pComboboxVisualType->AddStringEx(_T("示波显示"))->SetData((void*)VISUALIZATION_WAVE);
			m_pComboboxVisualType->AddStringEx(_T("专辑封面"))->SetData((void*)VISUALIZATION_NONE);

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

		String strArray[] = { _T("关于"),_T("常规"),_T("播放"),_T("快捷键"),_T("视觉效果"),
			_T("播放列表"),_T("媒体库"),_T("歌词秀"),_T("歌词搜索"),_T("网络连接"),_T("音效插件"),
			_T("音频设备"),_T("皮肤"),_T("全屏显示"),_T("系统关联")};
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