#include "stdafx.h"


GridPanel::GridPanel(void)
{
	m_pData = NULL;
	m_bDrawBorder = false;
	::SetRectEmpty( &m_rcMargin );

#if _DEBUG
	m_bDrawBorder = true;
#endif


	// TODO. �����඼���Ż�
}

GridPanel::~GridPanel(void)
{
	// TODO . �ͷ���Դ
}

// ��ʼ�����Ĭ�ϴ���һ�� 1*1 ��С�ı��
bool GridPanel::Initialize( )
{
	bool  bRet = true;

	Cell* pCell = new Cell;
	this->m_pData = pCell;	

	this->m_vWidthInXml.push_back( _T("*") );
	this->m_vHeightInXml.push_back( _T("*") );

	return bRet;
}

bool GridPanel::AddRow()
{
	return true;
}

bool GridPanel::AddCol()
{
	return true;
}
void GridPanel::UpdateRect()
{
	int nTotalWidth  = U::RectW( &rectP );
	int nTotalHeight = U::RectH( &rectP );

	int nWSize = (int)m_vWidthInXml.size();
	int nHSize = (int)m_vHeightInXml.size();

	for( int i = 0; i < nWSize; i++ )
	{
		String value = m_vWidthInXml[i];
		int  nIndex = (int)value.find( _T('*') );
		if( String::npos != nIndex )
		{
			int nScale = 0;
			if( 0 != nIndex )
			{
				nScale = _ttoi( value.substr( 0, nIndex ).c_str() );
			}
			
		}
	}
}

void GridPanel::OnPaint( HDC hDC )
{

}


Cell::Cell()
{
	this->m_nRow      =  0;
	this->m_nCol      =  0;
	this->m_bCombined = false;

	this->m_pObject   = NULL;
	this->m_pObjCombined = NULL;

	this->m_pLeft     =  NULL;        
	this->m_pRight    =  NULL;
	this->m_pTop      =  NULL;
	this->m_pBottom   =  NULL;
}