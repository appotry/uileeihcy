#include "StdAfx.h"
#include "MainDlg.h"

CMainDlg::CMainDlg(void)
{
	m_nImageItemCount = 0;
	m_nImageWidth = 0;
	m_nImageHeight = 0;
	m_nItemWidth = 0;
	m_nItemHeight = 0;
	_tcscpy(m_szFileSavePath, _T(""));
}

CMainDlg::~CMainDlg(void)
{
}

void CMainDlg::OnClose()
{
	EndDialog(0);
}

BOOL CMainDlg::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	m_EditWidth.Attach(GetDlgItem(IDC_EDIT_WIDTH));
	m_EditHeight.Attach(GetDlgItem(IDC_EDIT_HEIGHT));
	m_EditItemCount.Attach(GetDlgItem(IDC_EDIT_ITEMCOUNT));
	m_EditItemWidth.Attach(GetDlgItem(IDC_EDIT_ITEM_WIDTH));
	m_EditItemHeight.Attach(GetDlgItem(IDC_EDIT_ITEM_HEIGHT));

	m_btnRadioH.Attach(GetDlgItem(IDC_RADIO_H));
	m_btnRadioV.Attach(GetDlgItem(IDC_RADIO_V));
	m_btnAddLast.Attach(GetDlgItem(IDC_BUTTON_ADD));
	m_btnInsert.Attach(GetDlgItem(IDC_BUTTON_INSERT));
	m_EditInsertPos.Attach(GetDlgItem(IDC_EDIT_INSERT_POS));
	
	this->CenterWindow();
	return TRUE;
}
void CMainDlg::OnPaint(CDCHandle dc)
{
	PAINTSTRUCT  ps;
	HDC hDC = BeginPaint(&ps);

	if (!m_image.IsNull())
	{
		RECT  rcCanvas;
		GetDlgItem(IDC_STATIC_PREV).GetWindowRect(&rcCanvas);
		this->ScreenToClient(&rcCanvas);

		int   nX = rcCanvas.left + (rcCanvas.right-rcCanvas.left - m_image.GetWidth())/2;
		int   nY = rcCanvas.top + (rcCanvas.bottom-rcCanvas.top - m_image.GetHeight())/2;

		m_image.Draw(hDC, nX, nY);
	}	

	EndPaint(&ps);
}
void CMainDlg::OnCancel(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	EndDialog(IDCANCEL);
}

void CMainDlg::OnSaveAs(UINT uNotifyCode, int nID, CWindow wndCtl)
{	
	CFileDialog  dlg(FALSE);
	if (IDOK == dlg.DoModal())
	{
		_tcscpy(m_szFileSavePath, dlg.m_szFileName);
		m_image.Save(m_szFileSavePath, Gdiplus::ImageFormatPNG);
		GetDlgItem(IDC_BTN_SAVE).EnableWindow(TRUE);
	}
}

void CMainDlg::OnSave(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	if (0 == _tcslen(m_szFileSavePath))
		return OnSaveAs(uNotifyCode, nID, wndCtl);

	m_image.Save(m_szFileSavePath, Gdiplus::ImageFormatPNG);
}

void CMainDlg::OnReset(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	m_nItemWidth = 0;
	m_nItemHeight = 0;
	m_nImageWidth = 0;
	m_nImageHeight = 0;
	m_nImageItemCount = 0;
	m_btnRadioH.SetCheck(BST_UNCHECKED);
	m_btnRadioV.SetCheck(BST_UNCHECKED);
	m_btnAddLast.SetCheck(BST_CHECKED);
	this->SetDlgItemInt(IDC_EDIT_ITEM_WIDTH, m_nItemWidth);
	this->SetDlgItemInt(IDC_EDIT_ITEM_HEIGHT, m_nItemHeight);

	m_EditItemCount.EnableWindow(FALSE);
	m_EditItemWidth.EnableWindow(FALSE);
	m_EditItemHeight.EnableWindow(FALSE);
	m_btnRadioH.EnableWindow(FALSE);
	m_btnRadioV.EnableWindow(FALSE);
	m_btnAddLast.EnableWindow(FALSE);
	m_btnInsert.EnableWindow(FALSE);
	m_EditInsertPos.EnableWindow(FALSE);
	m_eDirType = DIRECTION_H;

	this->GetDlgItem(IDC_BTN_SAVE).EnableWindow(FALSE);
	this->GetDlgItem(IDC_BTN_SAVEAS).EnableWindow(FALSE);
	this->SetDlgItemInt(IDC_EDIT_WIDTH, m_nImageWidth);
	this->SetDlgItemInt(IDC_EDIT_HEIGHT, m_nImageHeight);
	this->SetDlgItemInt(IDC_EDIT_ITEMCOUNT, m_nImageItemCount);
	
	_tcscpy(m_szFileSavePath, _T(""));
	m_image.Destroy();

	this->Invalidate();
}

void CMainDlg::OnRadioH(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	m_eDirType = DIRECTION_H;
}
void CMainDlg::OnRadioV(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	m_eDirType = DIRECTION_V;
}

void CMainDlg::OnDropFiles(HDROP hDropInfo)
{
	int nFileCount = ::DragQueryFile(hDropInfo, -1, NULL, 0);
	TCHAR szFileName[MAX_PATH] = _T("");;
	for (int i = 0; i < nFileCount; i++)
	{
		::DragQueryFile(hDropInfo,i,szFileName,MAX_PATH);
		this->AppendFile(szFileName);
	}
}


void CMainDlg::AppendFile(TCHAR* szFileName)
{
	if (0 == m_nImageItemCount)
	{
		// ³õÊ¼»¯
		m_image.Load(szFileName);
		if (m_image.IsNull())
			return;

		m_nItemWidth = m_image.GetWidth();
		m_nItemHeight = m_image.GetHeight();
		m_nImageWidth = m_image.GetWidth();
		m_nImageHeight = m_image.GetHeight();
		m_btnRadioH.SetCheck(BST_CHECKED);
		m_btnAddLast.SetCheck(BST_CHECKED);
		this->SetDlgItemInt(IDC_EDIT_ITEM_WIDTH, m_nItemWidth);
		this->SetDlgItemInt(IDC_EDIT_ITEM_HEIGHT, m_nItemHeight);
		this->GetDlgItem(IDC_BTN_SAVEAS).EnableWindow(TRUE);

		m_EditItemCount.EnableWindow(TRUE);
		m_EditItemWidth.EnableWindow(TRUE);
		m_EditItemHeight.EnableWindow(TRUE);
		m_btnRadioH.EnableWindow(TRUE);
		m_btnRadioV.EnableWindow(TRUE);
		m_btnAddLast.EnableWindow(TRUE);
		m_btnInsert.EnableWindow(TRUE);
		m_EditInsertPos.EnableWindow(TRUE);
		m_eDirType = DIRECTION_H;
		
	}
	else
	{
		UI::Image imageTemp;
		imageTemp.Load(szFileName);
		if (imageTemp.IsNull())
			return;

		UI::Image imageLast;
		if (m_eDirType == DIRECTION_H)
		{
			imageLast.Create(m_nImageWidth+m_nItemWidth, m_nImageHeight, 32, UI::Image::createAlphaChannel);

			HDC hDC = imageLast.BeginDrawToMyself();
			m_image.Draw(hDC, 0, 0);
			imageTemp.Draw(hDC, m_nItemWidth*m_nImageItemCount,0);
			imageLast.EndDrawToMyself();
			m_nImageWidth += m_nItemWidth;
		}
		else
		{
			imageLast.Create(m_nImageWidth, m_nImageHeight+m_nItemHeight, 32, UI::Image::createAlphaChannel);
			HDC hDC = imageLast.BeginDrawToMyself();
			m_image.Draw(hDC, 0, 0);
			imageTemp.Draw(hDC, 0, m_nItemHeight*m_nImageItemCount);
			imageLast.EndDrawToMyself();
			m_nImageHeight += m_nItemHeight;
		}
		imageTemp.Destroy();
		m_image.Destroy();
		m_image.Attach(imageLast.Detach());
		m_image.ForceUseAlpha();

		m_btnRadioV.EnableWindow(FALSE);
		m_btnRadioH.EnableWindow(FALSE);
	}
	m_nImageItemCount++;

	this->SetDlgItemInt(IDC_EDIT_WIDTH, m_nImageWidth);
	this->SetDlgItemInt(IDC_EDIT_HEIGHT, m_nImageHeight);
	this->SetDlgItemInt(IDC_EDIT_ITEMCOUNT, m_nImageItemCount);
	this->Invalidate();
}