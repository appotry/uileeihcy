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
	m_btnInsertSpace.Attach(GetDlgItem(IDC_BTN_SPACE));
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
	CFileDialog  dlg(FALSE, _T(".png"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("*.png\0*.png\0*.jpg\0*.jpg\0*.bmp\0*.bmp\0*.*\0*.*\0\0"));
	if (IDOK == dlg.DoModal())
	{
		_tcscpy(m_szFileSavePath, dlg.m_szFileName);
		m_image.SaveAsPng(m_szFileSavePath);
		GetDlgItem(IDC_BTN_SAVE).EnableWindow(TRUE);
	}
}

void CMainDlg::OnSave(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	if (0 == _tcslen(m_szFileSavePath))
		return OnSaveAs(uNotifyCode, nID, wndCtl);

	m_image.SaveAsPng(m_szFileSavePath);
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
	m_btnInsert.SetCheck(BST_UNCHECKED);
	this->SetDlgItemInt(IDC_EDIT_ITEM_WIDTH, m_nItemWidth);
	this->SetDlgItemInt(IDC_EDIT_ITEM_HEIGHT, m_nItemHeight);

	m_EditItemCount.EnableWindow(FALSE);
	m_EditItemWidth.EnableWindow(FALSE);
	m_EditItemHeight.EnableWindow(FALSE);
	m_btnRadioH.EnableWindow(FALSE);
	m_btnRadioV.EnableWindow(FALSE);
	m_btnAddLast.EnableWindow(FALSE);
	m_btnInsert.EnableWindow(FALSE);
	m_btnInsertSpace.EnableWindow(FALSE);
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

void CMainDlg::OnInsertSpace(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	AppendFile(NULL);
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
		// 初始化
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
		m_btnInsertSpace.EnableWindow(TRUE);
		m_EditInsertPos.EnableWindow(TRUE);
		m_eDirType = DIRECTION_H;
		
	}
	else
	{

		UI::IImage imageTemp;
		if (NULL != szFileName)  // 为NULL表示插入空白
		{
			imageTemp.Load(szFileName);
			if (imageTemp.IsNull())
				return;
		}

		UI::IImage imageLast;
		if (m_eDirType == DIRECTION_H)
		{
			imageLast.Create(m_nImageWidth+m_nItemWidth, m_nImageHeight, 32, UI::IImage::createAlphaChannel);

			if (m_btnAddLast.GetCheck()==BST_CHECKED)
			{
				HDC hDC = imageLast.BeginDrawToMyself();
				m_image.Draw(hDC, 0, 0);
				imageTemp.Draw(hDC, m_nItemWidth*m_nImageItemCount,0);
				imageLast.EndDrawToMyself();
				m_nImageWidth += m_nItemWidth;
			}
			else
			{
				int nPos = this->GetDlgItemInt(m_EditInsertPos.GetDlgCtrlID());
				if (nPos < 0)
					nPos = 0;

				if (nPos > m_nImageItemCount)
					nPos = m_nImageItemCount;

				HDC hDC = imageLast.BeginDrawToMyself();
				if (0 == nPos)
				{
					m_image.Draw(hDC, m_nItemWidth, 0);
					imageTemp.Draw(hDC, 0,0);
				}
				else if (nPos >=m_nImageItemCount)
				{
					m_image.Draw(hDC, 0, 0);
					imageTemp.Draw(hDC, m_nItemWidth*m_nImageItemCount,0);
				}
				else
				{
					imageTemp.Draw(hDC, m_nItemWidth*nPos, 0);
					m_image.Draw(hDC, 0, 0, m_nItemWidth*nPos, m_nItemHeight, 0,0, m_nItemWidth*nPos, m_nItemHeight);
					m_image.Draw(hDC, 
						m_nItemWidth*(nPos+1), 0, 
						m_nItemWidth*(m_nImageItemCount-nPos), m_nItemHeight, 
						m_nItemWidth*nPos,0, 
						m_nItemWidth*(m_nImageItemCount-nPos), m_nItemHeight );
				}
				imageLast.EndDrawToMyself();

				m_nImageWidth += m_nItemWidth;
			}
		}
		else
		{
 			imageLast.Create(m_nImageWidth, m_nImageHeight+m_nItemHeight, 32, UI::IImage::createAlphaChannel);

			if (m_btnAddLast.GetCheck()==BST_CHECKED)
			{
				HDC hDC = imageLast.BeginDrawToMyself();
				m_image.Draw(hDC, 0, 0);
				imageTemp.Draw(hDC, 0, m_nItemHeight*m_nImageItemCount);
				imageLast.EndDrawToMyself();
				m_nImageHeight += m_nItemHeight;
			}
			else
			{
				int nPos = this->GetDlgItemInt(m_EditInsertPos.GetDlgCtrlID());
				if (nPos < 0)
					nPos = 0;

				if (nPos > m_nImageItemCount)
					nPos = m_nImageItemCount;

				HDC hDC = imageLast.BeginDrawToMyself();
				if (0 == nPos)
				{
					m_image.Draw(hDC, 0, m_nItemHeight);
					imageTemp.Draw(hDC, 0,0);
				}
				else if (nPos >= m_nImageItemCount)
				{
					m_image.Draw(hDC, 0, 0);
					imageTemp.Draw(hDC, 0, m_nItemHeight*m_nImageItemCount);
				}
				else
				{
					imageTemp.Draw(hDC, 0, m_nItemHeight*nPos);
					m_image.Draw(hDC, 0, 0, m_nItemWidth, m_nItemHeight*nPos, 0,0, m_nItemWidth, m_nItemHeight*nPos );
					m_image.Draw(hDC, 
						0, m_nItemHeight*(nPos+1), 
						m_nItemWidth, m_nItemHeight*(m_nImageItemCount-nPos), 
						0, m_nItemHeight*nPos, 
						m_nItemWidth, m_nItemHeight*(m_nImageItemCount-nPos) );
				}
				imageLast.EndDrawToMyself();

				m_nImageHeight += m_nItemHeight;
			}
		}
		imageTemp.Destroy();
		m_image.Destroy();
		m_image.Attach(imageLast.Detach(), true);

		m_btnRadioV.EnableWindow(FALSE);
		m_btnRadioH.EnableWindow(FALSE);
	}
	m_nImageItemCount++;

	this->SetDlgItemInt(IDC_EDIT_WIDTH, m_nImageWidth);
	this->SetDlgItemInt(IDC_EDIT_HEIGHT, m_nImageHeight);
	this->SetDlgItemInt(IDC_EDIT_ITEMCOUNT, m_nImageItemCount);
	this->Invalidate();
}