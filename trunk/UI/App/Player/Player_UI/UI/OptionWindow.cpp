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
}

BOOL COptionWindow::OnInitDialog( HWND, LPARAM )
{
	m_pPanelAbout = (UI::IPanel*)this->FindChildObject(_T("panel_about"));
	m_pPanelVisualization = (UI::IPanel*)this->FindChildObject(_T("panel_visualization"));

// 	m_pPanelRichEditDemo = (UI::IPanel*)this->FindChildObject(_T("richedit_demo_panel"));
 	m_pPanelCtrlDemo = (UI::IPanel*)this->FindChildObject(_T("control_demo_panel"));
	m_pIntroduceRichEdit = (UI::IRichEdit*)this->FindChildObject(_T("option_introduce_text"));

	if (m_pPanelCtrlDemo)
		m_pPanelCtrlDemo->SetVisible(false, false);
	if (m_pPanelRichEditDemo)
		m_pPanelRichEditDemo->SetVisible(false, false);
	if (m_pPanelAbout)
		m_pPanelAbout->SetVisible(false,false);
	if (m_pPanelVisualization)
		m_pPanelVisualization->SetVisible(false, false);

	//////////////////////////////////////////////////////////////////////////

	if (m_pIntroduceRichEdit)
	{
//		m_pIntroduceRichEdit->GetRichEdit().SetReadOnly(true);
		m_pIntroduceRichEdit->SetSelBarWidth(10);
		m_pIntroduceRichEdit->SetRichTextFlag(true);

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
			m_pComboboxVisualType->AddStringEx(_T("<不显示>"))->SetData((LPARAM)VISUALIZATION_NONE);
			m_pComboboxVisualType->AddStringEx(_T("梦幻星空"))->SetData((LPARAM)VISUALIZATION_NONE);
			m_pComboboxVisualType->AddStringEx(_T("频谱分析"))->SetData((LPARAM)VISUALIZATION_SPECTRUM);
			m_pComboboxVisualType->AddStringEx(_T("示波显示"))->SetData((LPARAM)VISUALIZATION_WAVE);
			m_pComboboxVisualType->AddStringEx(_T("专辑封面"))->SetData((LPARAM)VISUALIZATION_NONE);

			m_pComboboxVisualType->SetComboboxStyleType(COMBOBOX_STYLE_DROPDOWN);

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

	//////////////////////////////////////////////////////////////////////////
	
// 	if (m_pPanelRichEditDemo)
// 	{
// 		m_pPanelRichEditDemo->SetVisible(false, false);
// 	}

    m_pListOption = (UI::IListBox*)this->FindChildObject(_T("optionlist") );
	if (m_pListOption)
	{
		m_pListOption->ModifyStyleEx(0, LISTCTRLBASE_STYLE_SORT_ASCEND|LISTCTRLBASE_STYLE_SORT_DESCEND, false);

		String strArray[] = { _T("关于"),_T("常规"),_T("播放"),_T("快捷键"),_T("视觉效果"),
			_T("播放列表"),_T("媒体库"),_T("歌词秀"),_T("歌词搜索"),_T("网络连接"),_T("音效插件"),
			_T("音频设备"),_T("皮肤"),_T("全屏显示"),_T("系统关联")};
		int nCount = sizeof(strArray)/sizeof(String);
		for (int i = 0; i < nCount; i++)
		{
            UI::IListBoxItem* pItem = m_pListOption->AddString(strArray[i].c_str(), LISTITEM_OPFLAG_NOALL);
			if (pItem)
			{
				switch(i)
				{
				case 0:
					pItem->SetData((LPARAM)m_pPanelAbout);
					break;
				case 4:
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

void COptionWindow::OnCbnSelChanged(UI::IMessage* pObjMsgFrom, UI::IListItemBase* pOldSelItem, UI::IListItemBase* pSelItem)
{
	if (m_pComboboxVisualType == pObjMsgFrom)
	{
		if (pSelItem)
		{
			int nType = (int)(pSelItem->GetData());
			GetMainMgr()->SetVisualizationType(nType);
		}
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