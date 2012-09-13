#include "stdafx.h"
//
//	Remark:
//		m_EditData.SetCaret( nCP, true, bUpdate1 );
//		this->CalcCaretPos( m_EditData.GetCaretIndex(), bUpdate2 );
//
//		SetCaret函数负责计算光标索引和选区索引
//		MoveCaret函数负责计算出光标索引对应的屏幕显示坐标，并负责计算横向滚动
//
//		最后再调用updateObject刷新Edit，调用SetCaretPos设置光标位置
//

EditData::EditData()
{
	m_pEdit = NULL;

	m_nMaxChar = -1;
	m_nSelStart = 0;
	m_nCaret = 0;

	ZeroMemory( &m_ScriptControl, sizeof( SCRIPT_CONTROL ) );
	ZeroMemory( &m_ScriptState, sizeof( SCRIPT_STATE ) );
	ScriptApplyDigitSubstitution( NULL, &m_ScriptControl, &m_ScriptState );
	m_Analysis = NULL;
	m_nTextWidth = 0;

	m_bInsertMode = true;

#ifdef _DEBUG
	m_strText = _T("leeihcy");
#endif
}

void EditData::BindToEdit(EditBase* pEdit)
{
	m_pEdit = pEdit;
}

EditData::~EditData()
{
	if( NULL != m_Analysis )
	{
		ScriptStringFree(&m_Analysis);
		m_Analysis = NULL;
	}
	m_pEdit = NULL;
}

void EditData::SetText(const String& str, bool& bUpdate)
{
	bUpdate = false;

	String strInput;
	if( false == this->FilterString( str, strInput ) )
		return;

	if( 0 == m_nMaxChar )
		return;

	bUpdate = true;
	if( m_nMaxChar != -1 )
	{
		if( (int)strInput.length() > m_nMaxChar )
		{
			m_strText = strInput.substr(0, m_nMaxChar);
		}
		else
		{
			m_strText = strInput;
		}
	}
	else
	{
		m_strText = strInput;
	}

	this->Fire_Text_Changed();
}

//
//	在当前位置上面插入一个字符
//
void EditData::ReplaceChar(const TCHAR& c, bool& bUpdate)
{
	bUpdate = false;

	TCHAR cInput = _T('');
	if( false == this->FilterChar(c, cInput) )
		return;

	bool bInsertOrOverride = m_bInsertMode;

	// 如果当前有被选择的文字，那么得覆盖这些文字
	if( IsSelectionExist() )
	{
		bInsertOrOverride = true;  // 如果是覆盖模式，在删除完选区之后，应该转为插入，形成覆盖选区的样子

		bUpdate = true;
		DeleteSelectionText();
	}

	if( m_nCaret >= (int)m_strText.length() )
	{
		bInsertOrOverride = true;  // 光标在文字末尾时，应该是插入
	}

	if( bInsertOrOverride )
	{
		// 长度限制
		if ( m_nMaxChar >= 0 && (int)this->m_strText.length() >= (UINT)m_nMaxChar )
		{

		}
		else
		{
			bUpdate = true;
			this->m_strText.insert( m_nCaret, 1, cInput );
			m_nCaret += 1;
			m_nSelStart = m_nCaret;
		}
	}
	else
	{	
		// if( cInput != m_strText[m_nCaret] ) 即使要覆盖的字符一样，也要发送改变的通知
		{
			bUpdate = true; 
			m_strText.replace(m_nCaret,1,1,cInput);
			m_nCaret += 1;
			m_nSelStart = m_nCaret;
		}
	}

	if( bUpdate )
	{
		this->Fire_Text_Changed();
	}
}

//
//	在当前位置上面插入字符,如Copy,Paste
//
void EditData::ReplaceString(const String& str, bool& bUpdate)
{
	bUpdate = false;

	String strInput;
	if( false == this->FilterString( str, strInput ) )
		return;

	bool bInsertOrOverride = m_bInsertMode;

	// 如果当前有被选择的文字，那么得覆盖这些文字
	if( IsSelectionExist() )
	{
		bInsertOrOverride = true;   // 如果是覆盖模式，在删除完选区之后，应该转为插入，形成覆盖选区的样子

		bUpdate = true;
		DeleteSelectionText();
	}

	if( m_nCaret >= (int)m_strText.length() )
	{
		bInsertOrOverride = true;  // 光标在文字末尾时，应该是插入
	}

	if( bInsertOrOverride )
	{
		// 长度限制
		if ( m_nMaxChar >= 0 )
		{
			int nRemain = m_nMaxChar - this->m_strText.length();
			if( 0 == nRemain )
			{
				if( bUpdate )
				{
					this->Fire_Text_Changed();  // 由于删除了选区导致text changed
				}
				return;
			}

			if( nRemain < (int)strInput.length() )
			{
				strInput = strInput.substr(0,nRemain);
			}
		}

		this->m_strText.insert( m_nCaret, strInput );
	}
	else
	{
		// 长度限制
		if ( m_nMaxChar >= 0 )
		{
			int nRemain = m_nMaxChar - m_nCaret;
			if( 0 == nRemain )
			{
				if( bUpdate )
				{
					this->Fire_Text_Changed();  // 由于删除了选区导致text changed
				}
				return;
			}

			if( nRemain < (int)strInput.length() )
			{
				strInput = strInput.substr(0,nRemain);
			}
		}
		this->m_strText.replace( m_nCaret, strInput.length(), strInput.c_str() );
	}

	bUpdate = true;
	m_nCaret += strInput.length();
	m_nSelStart = m_nCaret;

	if( bUpdate )
	{
		this->Fire_Text_Changed();
	}
}

//
//	往后删除
//
void EditData::Delete( bool& bUpdate )
{
	bUpdate = true;

	// 删除当前所选择的文字
	if ( IsSelectionExist() )
	{
		this->DeleteSelectionText();
	}
	// 往后删除一个字符
	else
	{
		if ( this->m_strText.length() == 0 || m_nCaret >= (int)this->m_strText.length() )
		{
			bUpdate = false;
		}
		else
		{
			this->m_strText.erase( m_nCaret, 1 );
		}
	}

	m_nSelStart = m_nCaret;
	if( bUpdate )
	{
		this->Fire_Text_Changed();
	}
}

//
//	往前删除
//
void EditData::BackSpace( bool& bUpdate )
{
	bUpdate = true;

	// 删除当前所选择的文字
	if ( IsSelectionExist() )
	{
		this->DeleteSelectionText();
	}
	// 往前删除一个字符
	else
	{
		if ( m_nCaret <= 0 || this->m_strText.length() == 0 )
		{
			bUpdate = false;
		}
		else
		{
			m_nCaret--;
			this->m_strText.erase( m_nCaret, 1 );
		}
	}

	m_nSelStart = m_nCaret;
	if( bUpdate )
	{
		this->Fire_Text_Changed();
	}
}

//
//	设置光标的位置，同时设置选区开始的位置
//
void EditData::SetCaret(int nCaret, bool bSetSelStart, bool& bUpdate )
{
	bUpdate = false;

	int nOldSelStart = m_nSelStart;
	int nOldCaret = m_nCaret;

	if( nCaret < 0 )
	{
		nCaret = 0;
	}
	else if ( nCaret > (int)m_strText.length() )
	{
		nCaret = m_strText.length();
	}

	m_nCaret = nCaret;
	if( bSetSelStart )
	{
		m_nSelStart = m_nCaret;
	}


	if( m_nSelStart==m_nCaret && nOldCaret==nOldSelStart)           // 没有选区的光标移动
	{
	}
	else if( m_nSelStart == nOldCaret && m_nCaret == nOldSelStart ) // 选择反向
	{
	}
	else
	{
		bUpdate = true;
	}
}
void EditData::GetPriorItemPos( int nCP, int* pPrior )
{
	if( NULL == pPrior )
		return ;

	*pPrior = nCP;  // Default is the char itself

	if( NULL == m_Analysis )
	{
		if( false ==this->StringAnalysis() )
			return;
	}

	const SCRIPT_LOGATTR* pLogAttr = ScriptString_pLogAttr( m_Analysis );
	if( !pLogAttr )
		return;

	if( !ScriptString_pcOutChars( m_Analysis ) )
		return;
	int nInitial = *ScriptString_pcOutChars( m_Analysis );
	if( nCP - 1 < nInitial )
		nInitial = nCP - 1;
	for( int i = nInitial; i > 0; --i )
		if( pLogAttr[i].fWordStop ||       // Either the fWordStop flag is set
			( !pLogAttr[i].fWhiteSpace &&  // Or the previous char is whitespace but this isn't.
			pLogAttr[i - 1].fWhiteSpace ) )
		{
			*pPrior = i;
			return;
		}
		// We have reached index 0.  0 is always a break point, so simply return it.
		*pPrior = 0;
}
void EditData::GetNextItemPos( int nCP, int* pNext ) 
{
	if( NULL == pNext )
		return;

	*pNext = nCP;  // Default is the char itself

	if( NULL == m_Analysis )
	{
		if( false ==this->StringAnalysis() )
			return;
	}

	const SCRIPT_LOGATTR* pLogAttr = ScriptString_pLogAttr( m_Analysis );
	if( !pLogAttr )
		return;

	if( !ScriptString_pcOutChars( m_Analysis ) )
		return;
	int nInitial = *ScriptString_pcOutChars( m_Analysis );
	if( nCP + 1 < nInitial )
		nInitial = nCP + 1;

	int i = nInitial;
	int limit = *ScriptString_pcOutChars( m_Analysis );
	while( limit > 0 && i < limit - 1 )
	{
		if( pLogAttr[i].fWordStop )      // Either the fWordStop flag is set
		{
			*pNext = i;
			return;
		}
		else if( pLogAttr[i].fWhiteSpace &&  // Or this whitespace but the next char isn't.
			!pLogAttr[i + 1].fWhiteSpace )
		{
			*pNext = i + 1;  // The next char is a word stop
			return;
		}

		++i;
		limit = *ScriptString_pcOutChars( m_Analysis );
	}
	// We have reached the end. It's always a word stop, so simply return it.
	*pNext = *ScriptString_pcOutChars( m_Analysis ) - 1;

}

//
//	删除当前选区内的文字
//
//	注：该函数不会触发text changed
//
void EditData::DeleteSelectionText()
{
 	if( IsSelectionExist() )
 	{
 		if( m_nCaret > m_nSelStart )  // 从前到后选择
 		{
 			this->m_strText.erase( m_nSelStart, m_nCaret-m_nSelStart );
 			m_nCaret = m_nSelStart;
 		}
 		else                   // 从后往前选择的
 		{
 			this->m_strText.erase( m_nCaret , m_nSelStart-m_nCaret );
			m_nSelStart = m_nCaret;
 		}
 	}
}
bool EditData::IsSelectionExist()
{
	return (m_nCaret!=m_nSelStart);
}


void EditData::Fire_Text_Changed()
{
	this->StringAnalysis();
	this->CP2X(m_strText.length(), &m_nTextWidth);
}
bool EditData::FilterString( const String& strSrc, String& strDest )
{
	strDest = strSrc;  // TODO
	return true;
}
bool EditData::FilterChar( const TCHAR& cSrc, TCHAR& cDest )
{
	cDest = cSrc;      // TODO
	return true;
}

void EditData::SetMaxChar(int nMax)
{
	bool bUpdate = false;
	this->SetMaxChar(nMax, bUpdate);
}
void EditData::SetMaxChar(int nMax, bool bUpdate)
{
	bUpdate = false;
	m_nMaxChar = nMax;

	if( (int)m_strText.length() > m_nMaxChar )
	{
		bUpdate = true;
		m_strText = m_strText.substr(0,m_nMaxChar);
		this->Fire_Text_Changed();
	}
}


void EditData::SetInsertMode( bool bInsertOrOverride )
{
	m_bInsertMode = bInsertOrOverride;
}
//
//	初始化当前字符串m_Analysis
//
bool EditData::StringAnalysis()
{
	if( NULL != m_Analysis )
	{
		ScriptStringFree(&m_Analysis);
		m_Analysis = NULL;
	}

	HDC    hDC = UI_GetCacheDC();
	HRFONT hRFont = m_pEdit->GetFont();
	HFONT  hFont = UI_GetFontHFONT(hRFont);
	HFONT  hOldFont = (HFONT)::SelectObject(hDC, hFont);

	HRESULT hr = ScriptStringAnalyse(
		hDC,
		this->m_strText.c_str(),
		this->m_strText.length() + 1,			// 加上NULL.保证光标可以到达最后一个字符的后面。
		this->m_strText.length()*3/2 + 16,      // MSDN推荐值
		-1,
		SSA_BREAK | SSA_GLYPHS | SSA_FALLBACK | SSA_LINK,
		0,
		&m_ScriptControl,
		&m_ScriptState,
		NULL,
		NULL,
		NULL,
		&m_Analysis
		);

	::SelectObject(hDC, hOldFont);
	UI_ReleaseCacheDC(hDC);
	if( FAILED(hr) || NULL == m_Analysis )
		return false;

	

	return true;
}

bool EditData::CP2X(int nCP, int* pX)
{
	if( NULL == pX )
		return false;

	if( NULL == m_Analysis )
	{
		if( false ==this->StringAnalysis() )
			return false;
	}

	int	nX = 0;
	HRESULT hr = ScriptStringCPtoX(
		m_Analysis, 
		nCP,           // 要计算第一个字符
		FALSE,		   // 光标在字符前面还是后面
		pX			   // 返回值
		);	

	if( FAILED(hr) )
		return false;

	return true;
}

bool EditData::X2CP( int nX, int* pnCP, int* pbTrailOrLead )
{
	if( NULL == pnCP || NULL == pbTrailOrLead )
		return false;

	if( NULL == m_Analysis )
	{
		if( false ==this->StringAnalysis() )
			return false;
	}

	HRESULT hr = ScriptStringXtoCP(
		m_Analysis, 
		nX,
		pnCP,			
		pbTrailOrLead		// 光标在字符前面还是后面
		);	

	if( FAILED(hr) )
		return false;

	return true;
}

void EditData::GetSelectionInfo( int& nLeft, int& nRight )
{
	if( m_nSelStart < m_nCaret )
	{
		nLeft = m_nSelStart;
		nRight = m_nCaret;
	}
	else
	{
		nLeft = m_nCaret;
		nRight = m_nSelStart;
	}
}

void EditData::GetSelectionText( String& str )
{
	int nLeft = 0, nRight = 0;
	this->GetSelectionInfo(nLeft, nRight);

	str = m_strText.substr(nLeft, nRight-nLeft );
}

void EditData::CutToClipboard()
{
	if( IsSelectionExist() )
	{
		this->CopyToClipboard();

		bool bUpdate = false;
		this->Delete(bUpdate);
	}
}

void EditData::CopyToClipboard()
{
	if( IsSelectionExist() && OpenClipboard( NULL ) )
	{
		EmptyClipboard();

		String strSelectionText;
		this->GetSelectionText(strSelectionText);

		int nSize = sizeof( TCHAR ) * ( strSelectionText.length() + 1 );
		HGLOBAL hBlock = GlobalAlloc( GMEM_MOVEABLE, nSize );
		if( hBlock )
		{
			void* p = GlobalLock( hBlock );
			memcpy(p, strSelectionText.c_str(), nSize);
			GlobalUnlock( hBlock );
		}
		SetClipboardData( CF_UNICODETEXT, hBlock );

		CloseClipboard();
		// We must not free the object until CloseClipboard is called.
		if( hBlock )
			GlobalFree( hBlock );
	}
}


void EditData::PasteFromClipboard()
{
	DeleteSelectionText();

	if( OpenClipboard( NULL ) )
	{
		HANDLE handle = GetClipboardData( CF_UNICODETEXT );
		if( handle )
		{
			// Convert the ANSI string to Unicode, then
			// insert to our buffer.
			WCHAR* pwszText = ( WCHAR* )GlobalLock( handle );
			if( pwszText )
			{
				// Copy all characters up to null.
				bool bUpdate = false;
				ReplaceString( String(pwszText), bUpdate );
				GlobalUnlock( handle );
			}
		}
		CloseClipboard();
	}
}




//////////////////////////////////////////////////////////////////////////

EditBase::EditBase()
{
	this->SetTabstop( true );  
 	CRegion4 rPadding(3,0,3,0);
	this->SetPaddingRegion(&rPadding);

	this->m_pColor = NULL;
	this->m_pColorSelect = NULL;
	this->m_pColorSelectBk= NULL;
	
	m_bMouseDrag   = false;

	m_nXScroll     = 0;
	m_nCaretHeight = 16;

	m_nXSelStart   = 0;
	m_nXSelEnd	   = 0;
	m_nXCaretPos   = 0;

	m_EditData.BindToEdit(this);
	m_caret.SetLayered(false);
}


EditBase::~EditBase()
{
	SAFE_RELEASE(m_pColor);
	SAFE_RELEASE(m_pColorSelect);
	SAFE_RELEASE(m_pColorSelectBk);
}

String EditBase::GetText()
{
	return this->m_EditData.GetText();;
}

// 
void EditBase::SetText( const String& strText )
{
	bool bUpdate = false;
	m_EditData.SetText(strText, bUpdate);		

	if( bUpdate )
	{
		this->UpdateObject();
	}
}

int  EditBase::GetEditStyle()
{
	return m_nStyle & EDIT_STYLE_MASK;
}
void EditBase::SetEditStyle(int n)
{
	if(n > EDIT_STYLE_MASK)
		return;

	m_nStyle &= ~EDIT_STYLE_MASK;
	m_nStyle |= n;
}

//////////////////////////////////////////////////////////////////////////
//                                                                      //
//                                 虚函数                               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

void EditBase::ResetAttribute()
{
	Control::ResetAttribute();

	m_nCaretHeight = 16;
	SAFE_RELEASE(m_pColor);
	SAFE_RELEASE(m_pColorSelect);
	SAFE_RELEASE(m_pColorSelectBk);
	this->m_EditData.SetMaxChar(-1);

	CRegion4 r(3,0,3,0);
	this->SetPaddingRegion(&r);
}
bool EditBase::SetAttribute( map<String,String>& mapAttrib, bool bReload )
{
	bool bRet = Control::SetAttribute( mapAttrib,bReload );
	if( false == bRet )	return bRet;

	// 颜色
	if( mapAttrib.count( XML_EDIT_COLOR ) )
	{
		UI_GetColor( mapAttrib[XML_EDIT_COLOR], &this->m_pColor );
		__super::m_mapAttribute.erase(XML_EDIT_COLOR);
	}
	if( mapAttrib.count( XML_EDIT_SELECT_COLOR ) )
	{
		UI_GetColor( mapAttrib[XML_EDIT_SELECT_COLOR], &this->m_pColorSelect );
		__super::m_mapAttribute.erase(XML_EDIT_SELECT_COLOR);
	}
	if( mapAttrib.count( XML_EDIT_SELECT_BK_COLOR ) )
	{
		UI_GetColor( mapAttrib[XML_EDIT_SELECT_BK_COLOR], &this->m_pColorSelectBk );
		__super::m_mapAttribute.erase(XML_EDIT_SELECT_BK_COLOR);
	}

	// 最大值
	if( mapAttrib.count( XML_EDIT_MAX_LENGTH ) )
	{
		this->m_EditData.SetMaxChar( _ttoi( mapAttrib[XML_EDIT_MAX_LENGTH].c_str() ) );
		__super::m_mapAttribute.erase( XML_EDIT_MAX_LENGTH );
	}

	// 背景绘制 
	if( NULL == m_pBkgndRender && 0 == this->GetEditStyle())
	{
		m_pBkgndRender = RenderFactory::GetRender( RENDER_TYPE_THEME, this);
		this->ModifyStyle(OBJECT_STYLE_TRANSPARENT);
	}

	// 内容(必须在设置了 maxlength之后才能赋值 -- 不暴露该属性给xml
// 	if( mapAttrib.count( XML_EDIT_TEXT ) )
// 	{
// 		this->SetText( mapAttrib[XML_EDIT_TEXT] );
// 		__super::m_mapAttribute.erase( XML_EDIT_TEXT );
// 	}

// 	if( NULL == m_pEraseBkgndRender )   // 设置一个默认白色背景
// 	{
// 		m_pEraseBkgndRender = ObjectEraseBkgndRenderFactory::GetRender(OBJECT_ERASEBKGND_TYPE_COLOR, this);
// 		((ColorEraseBkgndRender*)m_pEraseBkgndRender)->SetBkColor( RGB(255,255,255) ); 
// 		((ColorEraseBkgndRender*)m_pEraseBkgndRender)->SetBorderColor( RGB(183,217,237) ); 
// 	}

	HRFONT hRFont = GetFont();
	if( NULL != hRFont )
	{
		m_nCaretHeight = GetTextMetricsHeight(hRFont);
	}

	return bRet;
}


SIZE EditBase::GetAutoSize( HRDC hRDC )
{
	String strTest = this->m_EditData.GetText();
	if( strTest.empty() )
	{
		strTest = _T("T");  // 当text为空时，不能让GetTextExtentPoint32返回的高度为0
	}

	SIZE s = MeasureString( GetFont(), strTest.c_str() );
	s.cx += m_rcPadding.left + m_rcPadding.right;
	s.cy += m_rcPadding.top + m_rcPadding.bottom;

	s.cy +=6; // 防止edit过小而添加的值
	return s;
}


//////////////////////////////////////////////////////////////////////////
//                                                                      //
//                               私有方法                               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
 
/*
**	[virtual] TODO. 未实现
**
**	用在Edit的子类中过滤一些符号
**
**	返回true表示允许输入，false表示禁止输入
*/
bool EditBase::FilterInputChar( UINT nChar )
{
	return true;
}

void EditBase::DrawNormal( HRDC hRDC )
{
	CRect rcClient;
	this->GetClientRectAsWin32(&rcClient);

	// 文字

	HRFONT hRFont = this->GetFont();
	HFONT  hFont = UI_GetFontHFONT(hRFont);
	HDC    hDC = GetHDC(hRDC);
	SelectObject(hDC, hFont);
	COLORREF oldTextCol = ::SetTextColor( hDC, RGB( 0,0,0) );

	CRect rcText = rcClient;
	rcText.top = rcClient.top + (rcClient.Height() - m_nCaretHeight)/2;
	rcText.bottom = rcText.top + m_nCaretHeight;

	ExtTextOut( 
		hDC, rcClient.left - m_nXScroll, rcText.top, 
		ETO_CLIPPED, 
		&rcText, 
		m_EditData.GetText().c_str(), 
		m_EditData.GetTextLength(), NULL );
	
	::SetTextColor( hDC, oldTextCol );
	ReleaseHDC(hRDC,hDC);

#ifdef _DEBUG
	{
	//	UI_LOG_DEBUG(_T("Edit::Draw, CaretIndex:%d, CaretPos:%d, ScrollX:%d, String:%s"),
	//		m_EditData.GetCaretIndex(), m_nXCaretPos, m_nXScroll, m_EditData.GetText().c_str() );
	}
#endif
}

void EditBase::DrawFocus( HRDC hRDC )
{
	CRect rcClient;
	this->GetClientRectAsWin32(&rcClient);

	// 绘制被选择的文字
	if( m_EditData.IsSelectionExist() )
	{
		int nSelLeft = 0;
		int nSelRight = 0;
		m_EditData.GetSelectionInfo(nSelLeft, nSelRight);
		
		int x = 0; 
		if( m_EditData.GetCaretIndex() == nSelLeft )
			x = m_nXCaretPos+m_nXScroll;
		else
			m_EditData.CP2X( nSelLeft, &x ) ;

		x = x - m_nXScroll;

		HRFONT hRFont = this->GetFont();
		HFONT  hFont = UI_GetFontHFONT(hRFont);
		HDC    hDC = GetHDC(hRDC);
		SelectObject(hDC, hFont);

		COLORREF oldBkCol = ::SetBkColor( hDC, RGB(49,106,197));
		COLORREF oldTextCol = ::SetTextColor( hDC, RGB( 255,255,255));
		int		 oldBkMode = ::SetBkMode( hDC, OPAQUE );

		// m_nSelCount如果为负数的话，表明时候从后往前选的

		String selString;
		m_EditData.GetSelectionText(selString);

		// 是否可以考虑使用MaskBlt(还得考虑绘制Clip)
		CRect rcText = rcClient;
		rcText.top = rcClient.top + (rcClient.Height() - m_nCaretHeight)/2;
		rcText.bottom = rcText.top + m_nCaretHeight;

		ExtTextOut( 
			hDC, x, rcText.top, 
			ETO_CLIPPED, 
			&rcText, 
			selString.c_str(),
			nSelRight-nSelLeft, NULL );

		::SetBkMode( hDC, oldBkMode );
		::SetTextColor( hDC, oldTextCol );
		::SetBkColor( hDC, oldBkCol );

		ReleaseHDC(hRDC,hDC);
	}
}

//
//	将光标位置设置在第N个字符前面。该函数仅更新数据，不真正设置光标位置
//
void EditBase::CalcCaretPos(int nCaretIndex, bool& bUpdate)
{
	bUpdate = false;

	int nX = 0;
	m_EditData.CP2X(nCaretIndex, &nX);
	
	CRect rcClient;
	this->GetClientRect(&rcClient);

	int nEditW  = rcClient.Width();
	int nXLeft  = m_nXScroll;
	int nXRight = m_nXScroll + nEditW;

	// 计算文字滚动
	// 情况1：当有滚动时，Edit最后面不应该是空的。
	if(m_nXScroll  > 0)
	{
		int nRemain = nXRight - m_EditData.GetTextWidth();
		if( nRemain > 0 )	// 右边有剩余空间
		{
			m_nXScroll -= nRemain;
			if ( m_nXScroll < 0 )
				m_nXScroll = 0;

			bUpdate = true;
		}
	}
	nXLeft = m_nXScroll;
	nXRight = nXLeft + nEditW;

	// 情况2：光标超出左边界
	if( nX < nXLeft )
	{
		m_nXScroll = nX;
		bUpdate = true;
		m_nXCaretPos = 0;
	}
	// 情况3：光标超出右边界
	else if( nX > nXRight )
	{
		m_nXScroll += (nX - nXRight );
		bUpdate = true;
		m_nXCaretPos = nEditW;
	}
	// 仅光标移动
	else
	{
		m_nXCaretPos = nX-m_nXScroll;
	}
}

void EditBase::UpdateCaretByPos()
{
	POINT ptWindow = this->GetRealPosInWindow();

	// 将光标和TEXT一样，纵向居中
	CRect rcClient;
	this->GetClientRect(&rcClient);
	int y = ptWindow.y + m_rcNonClient.top + (rcClient.Height()-m_nCaretHeight)/2;

	//::SetCaretPos( m_nXCaretPos + ptWindow.x + m_rcPadding.left, y );
	m_caret.SetCaretPos(m_nXCaretPos + ptWindow.x + m_rcPadding.left, y);
}

//////////////////////////////////////////////////////////////////////////
//                                                                      //
//                                 消息响应                             //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


void EditBase::OnEraseBkgnd(HRDC hRDC)
{
	if( NULL != m_pBkgndRender )
	{
		CRect rc(0,0,GetWidth(),GetHeight());
		if( !this->IsEnable() )
		{
			m_pBkgndRender->DrawState(hRDC, &rc, EDIT_BKGND_RENDER_STATE_DISABLE);
		}
		else if( this->IsFocus() || this->IsPress() )
		{
			m_pBkgndRender->DrawState(hRDC, &rc, EDIT_BKGND_RENDER_STATE_PRESS);
		}
		else if( this->IsHover() )
		{
			m_pBkgndRender->DrawState(hRDC, &rc, EDIT_BKGND_RENDER_STATE_HOVER);
		}
		else
		{
			m_pBkgndRender->DrawState(hRDC, &rc, EDIT_BKGND_RENDER_STATE_NORMAL);
		}
	}
}
void EditBase::OnPaint( HRDC hRDC )
{
	this->DrawNormal( hRDC );

	if( this->IsFocus() )
		this->DrawFocus( hRDC );
}

BOOL EditBase::OnSetCursor( HWND hWnd, UINT nHitTest, UINT message )
{
	if( this->IsEnable() )
		::SetCursor( ::LoadCursor( NULL,IDC_IBEAM ) );
	else
		SetMsgHandled(FALSE);
	
	return TRUE;
}

void EditBase::OnSetFocus( Object* )
{
	HWND hWnd = GetHWND();
	//::CreateCaret( hWnd, NULL, 1, m_nCaretHeight );
	m_caret.CreateCaret(hWnd, NULL, 1, m_nCaretHeight);
	this->UpdateCaretByPos();
//	::ShowCaret( hWnd );  // 在响应完OnStateChanged刷新之后，再显示光标
}
void EditBase::OnKillFocus( Object* )
{
	m_bMouseDrag = false;
//	::HideCaret(GetHWND());
	m_caret.HideCaret();
//	::DestroyCaret(); 
	m_caret.DestroyCaret();
}

void EditBase::OnStateChanged(int nOld, int nNew)
{
	// 当Edit有焦点时，鼠标移动移出不刷新
	if( nOld&CSB_FOCUS && nNew&CSB_FOCUS )
	{
	}
	else
	{
		this->UpdateObject();
		if( nNew & CSB_FOCUS )
		{
			//::ShowCaret( GetHWND() );
			m_caret.ShowCaret();
		}
	}
}	

//
//	BACKSPACE 往前删除会产生一个WM_CHAR事件
//
//  case 24:  // Ctrl-X Cut
//  case VK_CANCEL: // Ctrl-C Copy
//  case VK_BACK:   // Backspace
//  case 22:  // Ctrl-V Paste
//  case 1:   // Ctrl-A Select All
//  case 26:  // Ctrl Z
//  case 2:   // Ctrl B
//  case 14:  // Ctrl N
//  case 19:  // Ctrl S
//  case 4:   // Ctrl D
//  case 6:   // Ctrl F
//  case 7:   // Ctrl G
//  case 10:  // Ctrl J
//  case 11:  // Ctrl K
//  case 12:  // Ctrl L
//  case 17:  // Ctrl Q
//  case 23:  // Ctrl W
//  case 5:   // Ctrl E
//  case 18:  // Ctrl R
//  case 20:  // Ctrl T
//  case 25:  // Ctrl Y
//  case 21:  // Ctrl U
//  case 9:   // Ctrl I
//  case 15:  // Ctrl O
//  case 16:  // Ctrl P
//  case 27:  // Ctrl [
//  case 29:  // Ctrl ]
//  case 28:  // Ctrl \ 
//
//
// case 0x09:          // Tab 
// 	// Tab stops exist every four spaces, so add 
// 	// spaces until the user hits the next tab. 
// 
// 	do 
// 	{ 
// 		SendMessage(hwnd, WM_CHAR, ' ', 1L); 
// 	} while (nCaretPosX % 4 != 0); 
// 	break; 
//
//
void EditBase::OnChar( UINT nChar, UINT nRepCnt, UINT nFlags )
{
	if( VK_ESCAPE == nChar || VK_RETURN == nChar )
		return;

	bool bCtrlDown = Util::IsKeyDown( VK_CONTROL );

	// 特殊消息处理
	if( VK_BACK == nChar )
	{
		this->OnKeyDown_Backspace(bCtrlDown);
		return ;
	}

	// 输入字符处理
	if( false == bCtrlDown )   
	{
		this->OnInputChar(nChar);
		return;
	}

	return ;
}
void EditBase::OnInputChar(UINT nChar)
{
	if( this->IsReadonly() )
		return;

	if ( false == FilterInputChar( nChar) )
		return;

	bool bUpdate1 = false, bUpdate2 = false;
	int  nOldXCaretPos = m_nXCaretPos;

	this->m_EditData.ReplaceChar(nChar, bUpdate1);
	this->CalcCaretPos(m_EditData.GetCaretIndex(), bUpdate2);

	if( bUpdate1 || bUpdate2 )
	{
	//	HideCaret(GetHWND());
		m_caret.HideCaret();
		UpdateObject();
	//	ShowCaret(GetHWND());
		m_caret.ShowCaret();
	}
	if(nOldXCaretPos != m_nXCaretPos)
	{
		this->UpdateCaretByPos();
	}
}

void EditBase::OnLButtonDown(UINT nFlags, POINT point)
{
	// 将鼠标位置转换为相对字符串的位置
	POINT ptClient;
	this->WindowPoint2ObjectClientPoint(&point, &ptClient);
	int x = ptClient.x + m_nXScroll;
	//int x = point.x - m_rcWindow.left - m_rcPadding.left + m_nXScroll;
	
	int nCP = 0, bTrailOrLead = 0;
	m_EditData.X2CP( x, &nCP, &bTrailOrLead );
	nCP += bTrailOrLead;
	 
	if( nCP > (int)this->m_EditData.GetTextLength() )
		nCP = this->m_EditData.GetTextLength();

	bool bUpdate1 = false, bUpdate2 = false;
	int  nOldXCaretPos = m_nXCaretPos;

	m_EditData.SetCaret( nCP, true, bUpdate1 );
	this->CalcCaretPos( m_EditData.GetCaretIndex(), bUpdate2 );

	if( bUpdate1 || bUpdate2 )
	{
	//	HWND hWnd = GetHWND();
	//	::HideCaret(hWnd);
		m_caret.HideCaret();
		this->UpdateObject();
	//	::ShowCaret(hWnd);
		m_caret.ShowCaret();
	}
	if( nOldXCaretPos != m_nXCaretPos && this->IsFocus() )
	{
		this->UpdateCaretByPos();
	}

	m_bMouseDrag = true;
}

void EditBase::OnLButtonUp(UINT nFlags, POINT point)
{
	m_bMouseDrag = false;
}	

//
//	TODO: 在 aa___bb
//			     |在这里双击，会导致aabb都被选中
//
void EditBase::OnLButtonDblClk(UINT nFlags, POINT point)
{
	bool bUpdate1 = false, bUpdate2 = false;
	int  nOldXCaretPos = m_nXCaretPos;

	int nCaretStart = 0, nCaretEnd = 0;
	m_EditData.GetPriorItemPos(m_EditData.GetCaretIndex(), &nCaretStart);
	m_EditData.GetNextItemPos(m_EditData.GetCaretIndex(), &nCaretEnd);

	m_EditData.SetCaret(nCaretStart, true,  bUpdate1 );
	m_EditData.SetCaret(nCaretEnd, false,  bUpdate1 );
	this->CalcCaretPos( m_EditData.GetCaretIndex(), bUpdate2 );

	//HWND hWnd = GetHWND();
	//::HideCaret(hWnd);
	m_caret.HideCaret();
	this->UpdateObject();
	//::ShowCaret(hWnd);
	m_caret.ShowCaret();
	
	if( nOldXCaretPos != m_nXCaretPos )
	{
		this->UpdateCaretByPos();
	}
}
void EditBase::OnMouseMove(UINT nFlags, POINT point)
{
	if( m_bMouseDrag )
	{
		// 将鼠标位置转换为相对字符串的位置
		POINT ptWindow = this->GetRealPosInWindow();
		int x = point.x - ptWindow.x - m_rcPadding.left + m_nXScroll;

		int nCP = 0, bTrailOrLead = 0;
		m_EditData.X2CP( x, &nCP, &bTrailOrLead );
		nCP += bTrailOrLead;

		if( nCP > (int)this->m_EditData.GetTextLength() )
			nCP = this->m_EditData.GetTextLength();

		bool bUpdate1 = false, bUpdate2 = false;
		int  nOldXCaretPos = m_nXCaretPos;

		m_EditData.SetCaret( nCP, false, bUpdate1 );
		this->CalcCaretPos( m_EditData.GetCaretIndex(), bUpdate2 );

		if( bUpdate1 || bUpdate2 )
		{
		//	HWND hWnd = GetHWND();
		//	::HideCaret(hWnd);
			m_caret.HideCaret();
			this->UpdateObject();
		//	::ShowCaret(hWnd);
			m_caret.ShowCaret();
		}
		if( nOldXCaretPos != m_nXCaretPos )
		{
			this->UpdateCaretByPos();
		}
	}
}



/*
**	DELETE          往后删除,删除选择
**  SHIFT + DELETE  往前删除,删除选择
**
**	VK_LEFT         光标往前移动
**  VK_RIGHT        光标往后移动
**  VK_UP           光标往前移动
**  VK_DOWN         光标往后移动
**
**	SHIFT + LEFT    往前选择
**	SHIFT + UP      往前选择
**	SHIFT + RIGHT   往后选择
**	SHIFT + DOWN    往后选择
**
*/
void EditBase::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags )
{
	bool bCtrlDown  =  Util::IsKeyDown( VK_CONTROL );
	if( bCtrlDown )
	{
		if ( _T('A') == nChar )
		{
			this->OnKeyDown_Ctrl_A();
			return ;
		}
		else if( _T('X') == nChar )
		{
			this->OnKeyDown_Ctrl_X();
			return;
		}
		else if( _T('C') == nChar )
		{
			this->OnKeyDown_Ctrl_C();
			return ;
		}
		else if( _T('V') == nChar )
		{
			this->OnKeyDown_Ctrl_V();
			return ;
		}
		else if( _T('Z') == nChar )
		{
			this->OnKeyDown_Ctrl_Z();
			return;
		}
		else if( _T('Y') == nChar )
		{
			this->OnKeyDown_Ctrl_Y();
			return;
		}
	}

	if( VK_DELETE == nChar )
	{
		this->OnKeyDown_Delete(bCtrlDown);
		return ;
	}
// 	else if( VK_BACK == nChar )   // 与VK_DELETE不一样，VK_BACK会产生WM_CHAR消息，因此将该消息放OnChar中处理
// 	{
// 		this->OnKeyDown_Backspace();
// 		return ;
// 	}
	else if( VK_LEFT == nChar || VK_UP == nChar )
	{
		this->OnKeyDown_Left_Top(bCtrlDown);
		return;
	}
	else if( VK_RIGHT == nChar || VK_DOWN == nChar )
	{
		this->OnKeyDown_Right_Down(bCtrlDown);
		return;
	}
	else if( VK_HOME == nChar )
	{
		this->OnKeyDown_Home(bCtrlDown);
		return;
	}
	else if( VK_END == nChar )
	{
		this->OnKeyDown_End(bCtrlDown);
		return ;
	}
	else if( VK_INSERT == nChar )
	{
		this->OnKeyDown_Insert(bCtrlDown);
	}
}

void EditBase::OnKeyDown_Ctrl_A()
{
	bool bUpdate1 = false, bUpdate2 = false;
	int  nOldXCaretPos = m_nXCaretPos;
	int  nOldSelectionLen = m_EditData.GetSelectionLength();
	
	m_EditData.SetCaret(0,true,bUpdate1);
	m_EditData.SetCaret(m_EditData.GetTextLength(),false,bUpdate1);
	this->CalcCaretPos(m_EditData.GetCaretIndex(),bUpdate2);

	if( nOldSelectionLen != m_EditData.GetTextLength() || bUpdate2 )  // 注：这里不由于是调用了两次SetCaret，导致第二次调用的bUpdate1必定为true，因此在这里通过判断旧的选区长度来决定是否需要刷新
	{
	//	HWND hWnd = GetHWND();
	//	::HideCaret(hWnd);
		m_caret.HideCaret();
		this->UpdateObject();
	//	::ShowCaret(hWnd);
		m_caret.ShowCaret();
	}
	if( nOldXCaretPos != m_nXCaretPos )
	{
		this->UpdateCaretByPos();
	}
}

void EditBase::OnKeyDown_Ctrl_X()
{
	if( this->IsReadonly() )
	{
		this->OnKeyDown_Ctrl_C();
		return;
	}

	if( m_EditData.IsSelectionExist() )
	{
		m_EditData.CutToClipboard();

		bool bUpdate = false;
		int  nOldXCaretPos = m_nXCaretPos;
		this->CalcCaretPos(m_EditData.GetCaretIndex(), bUpdate);

	//	HWND hWnd = GetHWND();
	//	::HideCaret(hWnd);
		m_caret.HideCaret();
		this->UpdateObject();
	//	::ShowCaret(hWnd);
		m_caret.ShowCaret();
		
		if( nOldXCaretPos != m_nXCaretPos )
		{
			this->UpdateCaretByPos();
		}
	}
}

void EditBase::OnKeyDown_Ctrl_C()
{
	m_EditData.CopyToClipboard();
}
void EditBase::OnKeyDown_Ctrl_V()
{
	if( this->IsReadonly() )
		return;

	bool bUpdate = false;
	int  nOldXCaretPos = m_nXCaretPos;

	m_EditData.PasteFromClipboard();
	this->CalcCaretPos(m_EditData.GetCaretIndex(), bUpdate);

	// 强制刷新
//	HWND hWnd = GetHWND();
//	::HideCaret(hWnd);  
	m_caret.HideCaret();  // <- 防止残影
	this->UpdateObject();
//	::ShowCaret(hWnd);
	m_caret.ShowCaret();

	if( nOldXCaretPos != m_nXCaretPos )
	{
		this->UpdateCaretByPos();
	}
}
void EditBase::OnKeyDown_Ctrl_Z()
{

}
void EditBase::OnKeyDown_Ctrl_Y()
{

}
void EditBase::OnKeyDown_Backspace(bool bCtrlDown)
{
	if( this->IsReadonly() )
		return;

	bool bUpdate1 = false, bUpdate2 = false;
	int  nOldXCaretPos = m_nXCaretPos;

	m_EditData.BackSpace(bUpdate1);
	this->CalcCaretPos(m_EditData.GetCaretIndex(), bUpdate2);

	if( bUpdate1 || bUpdate2 )
	{
	//	HWND hWnd = GetHWND();
	//	::HideCaret(hWnd);
		m_caret.HideCaret();
		this->UpdateObject();
	//	::ShowCaret(hWnd);
		m_caret.ShowCaret();
	}
	if( nOldXCaretPos != m_nXCaretPos )
	{
		this->UpdateCaretByPos();
	}
}
void EditBase::OnKeyDown_Delete(bool bCtrlDown)
{
	if( this->IsReadonly() )
		return;

	bool bUpdate1 = false, bUpdate2 = false;
	int  nOldXCaretPos = m_nXCaretPos;

	bool bShiftDown =  Util::IsKeyDown( VK_SHIFT );
	if( bShiftDown )  // SHIFT + DELETE 往前删除
	{
		 m_EditData.BackSpace(bUpdate1);
	}
	else             // DELETE 往后删除
	{ 
		m_EditData.Delete(bUpdate1);
	}

	this->CalcCaretPos(m_EditData.GetCaretIndex(), bUpdate2);

	if( bUpdate1 || bUpdate2 )
	{
	//	HWND hWnd = GetHWND();
	//	::HideCaret(hWnd);
		m_caret.HideCaret();
		this->UpdateObject();
	//	::ShowCaret(hWnd);
		m_caret.ShowCaret();
	}
	if( nOldXCaretPos != m_nXCaretPos )
	{
		this->UpdateCaretByPos();
	}
}

void EditBase::OnKeyDown_Left_Top(bool bCtrlDown)
{
	bool bShiftDown = Util::IsKeyDown( VK_SHIFT );

	bool bUpdate1 = false, bUpdate2 = false;
	int  nOldXCaretPos = m_nXCaretPos;

	int nDestCaret = 0;
	if(bCtrlDown)
	{
		m_EditData.GetPriorItemPos(m_EditData.GetCaretIndex(), &nDestCaret);
	}
	else
	{
		nDestCaret = m_EditData.GetCaretIndex()-1;
	}

	m_EditData.SetCaret(nDestCaret, !bShiftDown, bUpdate1 );
	this->CalcCaretPos( m_EditData.GetCaretIndex(), bUpdate2 );

	if( bUpdate1 || bUpdate2 )
	{
	//	HWND hWnd = GetHWND();
	//	::HideCaret(hWnd);
		m_caret.HideCaret();
		this->UpdateObject();
	//	::ShowCaret(hWnd);
		m_caret.ShowCaret();
	}
	if( nOldXCaretPos != m_nXCaretPos )
	{
		this->UpdateCaretByPos();
	}
}
void EditBase::OnKeyDown_Right_Down(bool bCtrlDown)
{
	bool bShiftDown = Util::IsKeyDown( VK_SHIFT );

	bool bUpdate1 = false, bUpdate2 = false;
	int  nOldXCaretPos = m_nXCaretPos;

	int nDestCaret = 0;
	if(bCtrlDown)
	{
		m_EditData.GetNextItemPos(m_EditData.GetCaretIndex(), &nDestCaret);
	}
	else
	{
		nDestCaret = m_EditData.GetCaretIndex()+1;
	}

	m_EditData.SetCaret(nDestCaret, !bShiftDown, bUpdate1 );
	this->CalcCaretPos( m_EditData.GetCaretIndex(), bUpdate2 );

	if( bUpdate1 || bUpdate2 )
	{
	//	HWND hWnd = GetHWND();
	//	::HideCaret(hWnd);
		m_caret.HideCaret();
		this->UpdateObject();
	//	::ShowCaret(hWnd);
		m_caret.ShowCaret();
	}
	if( m_nXCaretPos != nOldXCaretPos )
	{
		this->UpdateCaretByPos();
	}
}

void EditBase::OnKeyDown_Home(bool bCtrlDown)
{
	bool bShiftDown =  Util::IsKeyDown( VK_SHIFT );
	bool bUpdate1 = false, bUpdate2 = false;
	int  nOldXCaretPos = m_nXCaretPos;

	m_EditData.SetCaret(0, !bShiftDown, bUpdate1 );
	this->CalcCaretPos( m_EditData.GetCaretIndex(), bUpdate2 );

	if( bUpdate1 || bUpdate2 )
	{
	//	HWND hWnd = GetHWND();
	//	::HideCaret(hWnd);
		m_caret.HideCaret();
		this->UpdateObject();
	//	::ShowCaret(hWnd);
		m_caret.ShowCaret();
	}
	if( m_nXCaretPos != nOldXCaretPos )
	{
		this->UpdateCaretByPos();
	}
}
void EditBase::OnKeyDown_End(bool bCtrlDown)
{

	bool bShiftDown = Util::IsKeyDown( VK_SHIFT );
	bool bUpdate1 = false, bUpdate2 = false;
	int  nOldXCaretPos = m_nXCaretPos;

	m_EditData.SetCaret(m_EditData.GetTextLength(), !bShiftDown, bUpdate1 );
	this->CalcCaretPos( m_EditData.GetCaretIndex(), bUpdate2 );

	if( bUpdate1 || bUpdate2 )
	{
	//	HWND hWnd = GetHWND();
	//	::HideCaret(hWnd);
		m_caret.HideCaret();
		this->UpdateObject();
	//	::ShowCaret(hWnd);
		m_caret.ShowCaret();
	}
	if( m_nXCaretPos != nOldXCaretPos )
	{
		this->UpdateCaretByPos();
	}
}
void EditBase::OnKeyDown_Insert(bool bCtrlDown)
{
	if( bCtrlDown )
	{
		this->OnKeyDown_Ctrl_C();
	}
	else if(Util::IsKeyDown( VK_SHIFT ))
	{
		this->OnKeyDown_Ctrl_V();
	}
	else
	{
		m_EditData.SetInsertMode(!m_EditData.GetInsertMode());
	}
}
void EditBase::OnKeyUp( UINT nChar, UINT nRepCnt, UINT nFlags )
{
}
