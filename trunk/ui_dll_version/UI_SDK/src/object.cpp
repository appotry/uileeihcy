#include "stdafx.h"


Object::Object(void)
{
	::SetRectEmpty(&m_rcParent);
	::SetRectEmpty(&m_rcNonClient);
	::SetRectEmpty(&m_rcMargin);
	::SetRectEmpty(&m_rcPadding);
	::SetRectEmpty(&m_rcBorder);

	m_nConfigWidth    = m_nConfigHeight    = AUTO;
// 	minwidth = minheight = NDEF;
// 	maxwidth = maxheight = NDEF;
//	desiredWidth = desiredHeight = NDEF;
	
	this->m_hRgn = NULL;
	this->m_nStateBit = 0;
	this->m_nDropDragBit = 0;
	this->m_nStyleEx = 0;
	this->m_nStyle = 0;

	m_pBkgndRender = NULL;
	m_pForegndRender = NULL;
	m_pTextRender = NULL;
	m_pCursor = NULL;
}

 // ע�⣺��Ҫ�ڹ���������������е����麯��
Object::~Object(void)
{
}

void Object::InitialRelease()
{
	this->DestroyUI();  
}

void Object::DestroyUI()
{
	ObjTree::DestroyUI();

	m_rcParent.left = m_rcParent.right = m_rcParent.top = m_rcParent.bottom = 0;
	SAFE_DELETE(m_pBkgndRender);
	SAFE_DELETE(m_pForegndRender);
	SAFE_DELETE(m_pTextRender);
	SAFE_RELEASE(m_pCursor);
	SAFE_DELETE_GDIOBJECT(m_hRgn);
}

//
//	�ػ��������(һ��������������£�״̬�仯��ʱ����ػ�)
//
//	1. ��͸���Ŀؼ���ֱ����ԭ���漴�ɣ������˱���ΪCOLOR��Image�����
//		TODO: ���ǽ�ButtonBkRender�ٳ�������������еĿؼ�ʹ��
//	2. ͸���Ŀؼ�...�Ƿ���Ҫ�������Ż�Ϊֱ��ȡ���Լ��ڸ������µı�����������ˢ����������
//
void Object::UpdateObject( bool bUpdateNow )
{
	if( !this->IsVisible() )
		return ;

	if( this->testStateBit(CSB_PREVENTREDRAW))
		return ;

	WindowBase* pWindow = this->GetWindowObject();
	if (NULL != pWindow)
	{
		pWindow->InvalidateObject(this, NULL, bUpdateNow);
	}
}

// ����������/�ƶ���ʱ��ˢ�±���
void Object::UpdateObjectBkgnd( bool bUpdateNow )
{
	if( this->testStateBit(CSB_PREVENTREDRAW))
		return ;

	WindowBase* pWindow = this->GetWindowObject();
	if (NULL != pWindow)
	{
		pWindow->InvalidateObjectBkgnd(this, NULL, bUpdateNow);
	}
}

//
//	�ػ�����������ڵ�����һ���Ǵ������ء��ƶ�����С�ı��ʱ����ػ棩
//
//	prc һ��Ϊ &m_rcWindow
//
// void Object::UpdateObject( RECT* prc, bool bUpdateNow ) 
// {
// //	���ڵ��øú����ĵط��ö�������Ѿ������ˣ��������ﲻ���Ƿ�ɼ������ж�
// // 	if( !this->IsVisible() )
// // 	{
// // 		return;
// // 	}
// 
// 	if( this->testStateBit( CSB_PREVENTREDRAW) )
// 		return ;
// 
// 	WindowBase* pWindow = this->GetWindowObject();
//	WindowBase* pWindow = this->GetWindowObject();
//	if (NULL != pWindow)
// 		pWindow->InvalidateObject( pWindow, prc, bUpdateNow );
// }

//
//	��������ʾ/���أ����ߴ�С�����ı�ʱ������ˢ���Լ�����layout�Ĳ���
//
//	1. ��ȡ�Լ�������Layout
//	2. ����Layout�Ĵ�С�Ƿ����ı䣬������Layout��С�ı䣬�ٻ�ȡ��һ��Layout
//
void Object::UpdateLayout( bool bUpdate )
{
	Object* pParent = this->GetParentObject();
	if( NULL == pParent || OBJ_CONTROL == pParent->GetObjectType() )
	{
		return;
	}
	
	if( NULL == ((Panel*)pParent)->GetLayout() )
		return;

	HRDC hRDC = GetHRDC( this->GetHWND() );
	((Panel*)pParent)->GetLayout()->ArrangeChildObject(hRDC, this, bUpdate);
	ReleaseHRDC(hRDC);

	if( pParent->GetParentObject() != NULL )
	{
		pParent->UpdateLayout(bUpdate);
	}
}

//
//	��ȡ�Լ����游�����б�(��window����Ϊֹ)���ɽ���Զ������vector��
//
// int Object::GetParentList( vector<Object*>& vParents )
// {
// 	vParents.clear();
// 
// 	if( OBJ_WINDOW == this->GetObjectType() )
// 		return 0;
// 
// 	Object* pParent = m_pParent;
// 	if( NULL == pParent )
// 		return 0;
// 
// 	while(pParent)
// 	{
// 		vParents.push_back(pParent);
// 		if( OBJ_WINDOW == pParent->GetObjectType() )
// 		{
// 			break;
// 		}
// 
// 		pParent = pParent->GetParentObject();
// 	}
// 	
// 	return (int)vParents.size();
// }

//
//	��ȡ�Լ������ڴ����е�clip�����Ӷ�������и������ڲ���
//
//	���ص�HRGN�����ɵ����߽����ͷ�
//
HRGN Object::GetClipRgnInWindow()
{
	HRGN    hRetRgn = NULL;

	// TODO: ��д
// 	Object *pObject = NULL;
// 	while ( pObject = pObject->REnumParentObject(pObject) )
// 	{
// 		HRGN hClipRgn = NULL;
// 		if( pObject->GetRgn() != NULL )  // �ö�����һ������RECT����ֱ��ʹ������RGN����
// 		{
// 			hClipRgn = pObject->GetRgn();
// 		}
// 		else
// 		{
// 			hClipRgn = ::CreateRectRgn( pObject->GetWindowRectL(), pObject->GetWindowRectT(), pObject->GetWindowRectR(), pObject->GetWindowRectB() );
// 		}
// 
// 		// ��Ϊ�ӿؼ��ķ�Χ����Ҫ��parent����������������ȡ�Ӻ͸��Ľ���clip rgn���л���
// 		if( NULL == hRetRgn )
// 		{
// 			hRetRgn = ::CreateRectRgn(0,0,0,0);
// 			::CombineRgn(hRetRgn, hClipRgn, hRetRgn, RGN_COPY );
// 		}
// 		else
// 		{
// 			::CombineRgn( hRetRgn, hRetRgn, hClipRgn, RGN_AND );
// 		}
// 		
// 		if( NULL == pObject->GetRgn() )
// 		{
// 			::DeleteObject(hClipRgn);
// 			hClipRgn = NULL;
// 		}
// 	}

	return hRetRgn;
}

void Object::SetReDraw( bool bReDraw )
{
	if( bReDraw )
	{
		this->clearStateBit( CSB_PREVENTREDRAW );
	}
	else
	{
		this->setStateBit( CSB_PREVENTREDRAW );
	}

}

// ��ȡһ���ؼ����ڴ��ھ��
WindowBase* Object::GetWindowObject()
{
	Object* pObj = this;
	while(NULL != pObj)
	{
		if (pObj->GetObjectType() == OBJ_WINDOW)
			return (WindowBase*)pObj;
		else
			pObj = pObj->m_pParent;
	}
	return NULL;
}

HWND Object::GetHWND()
{
	WindowBase* pWindow = this->GetWindowObject();
	if (NULL == pWindow)
		return NULL;

	return pWindow->m_hWnd;
}
/*
**	[public] ��ȡĳһ����·���µ��Ӷ��󣬵�������Ӷ���Ҳ�����Ӷ�����Ӷ���
**
**	parameter
**		objIdPath
**			[in]	Ҫ��ȡ�Ķ����·�����������Լ�����·����/�ָ�
**	
**	return
**		�ɹ����ظö���ʧ�ܷ���NULL��ʧ�ܿɲ鿴��־���
*/
// Object*  Object::GetChildObject( const String& objIdPath )
// {
// 	Object*   pObjRet  = this;
// 
// 	vector<String>  vPath;
// 	UI_Split( objIdPath, XML_PATH_SEPERATOR, vPath );
// 
// 	if( vPath.empty() )
// 	{
// 		UI_LOG_ERROR( _T("Object::GetObjectItem����Ч�Ĳ���:%s"), objIdPath.c_str() );
// 		return NULL;
// 	}
// 
// 	size_t  s = vPath.size();
// 	for( size_t i = 0; i < s; i++ )
// 	{
// 		Object* pTempObj = NULL;
// 		while( pTempObj = pObjRet->EnumChildObject( pTempObj ) )
// 		{
// 			if( vPath[i] == pTempObj->m_strID )
// 			{
// 				pObjRet = pTempObj;
// 				break;  // jump out of while
// 			}
// 		}
// 
// 		// û���ҵ�ָ���Ķ���
// 		if( NULL == pTempObj )
// 		{
// 			UI_LOG_ERROR( _T("Object::GetObjectItem��δ�ҵ�ָ���Ķ�����%s�£�"), this->m_strID.c_str(), objIdPath.c_str() );
// 			return NULL;
// 		}
// 	}
// 
// 	return pObjRet;
// }

//
//	�����е����������в���idΪobjId�Ķ���ֻ���ص�һ���ҵ��Ķ���
//
//	Parameter
//		objId
//			[in]	Ҫ���ҵĶ���id
//
//	Return
//		�ɹ����ض����ַ��ʧ�ܷ���NULL
//
Object*  Object::FindChildObject( const String& objId )
{
	Object* pRet = this->_findChildObjectItem(objId);
	if( NULL == pRet )
	{
		UI_LOG_WARN(_T("Object::FindChildObject Find \"%s\" from \"%s\" failed."), objId.c_str(), this->m_strID.c_str() );
	}
	return pRet;
}
Object* Object::_findChildObjectItem( const String& objId )
{
	Object* pObjChild = NULL;
	while( pObjChild = this->EnumChildObject(pObjChild) )
	{
		if( pObjChild->m_strID == objId )
			return pObjChild;
		else
		{
			Object* pRet = pObjChild->_findChildObjectItem(objId );
			if( NULL != pRet )
				return pRet;
		}
	}	
	return NULL;
}

void Object::ResetAttribute()
{
	::SetRectEmpty(&m_rcMargin);
	::SetRectEmpty(&m_rcPadding);
	::SetRectEmpty(&m_rcNonClient);
	::SetRectEmpty(&m_rcBorder);
	m_nConfigWidth = m_nConfigHeight = AUTO;

	SAFE_DELETE(m_pBkgndRender);
	SAFE_DELETE(m_pForegndRender);
	SAFE_DELETE(m_pTextRender);
	SAFE_RELEASE(m_pCursor);

}
bool Object::SetAttribute(ATTRMAP& mapAttrib, bool bReload )
{
	//
	//  1. �����Կ�������
	//
	this->m_mapAttribute.clear();
	this->m_mapAttribute = mapAttrib;

	//
	//	2. ����id����
	//
	if( this->m_mapAttribute.count( XML_ID ) )
	{
		this->m_strID = this->m_mapAttribute[ XML_ID ];
		this->m_mapAttribute.erase( XML_ID );
	}
	
	if (m_strID.empty())
	{
		UI_LOG_WARN( _T("Object::SetAttrib, not find name=%s attribute: \"%s\""), this->GetObjectName(), XML_ID ) ;
	}

	//  3. ���ò������ֵ
	if( this->m_mapAttribute.count( XML_WIDTH ) )
	{
		String str = this->m_mapAttribute[ XML_WIDTH ];
		if( XML_AUTO == str )
		{
			this->m_nConfigWidth = AUTO;
		}
		else
			this->m_nConfigWidth = _ttoi( str.c_str() );

		this->m_mapAttribute.erase( XML_WIDTH );
	}
	if( this->m_mapAttribute.count( XML_HEIGHT ) )
	{
		String str = this->m_mapAttribute[ XML_HEIGHT ];
		if( XML_AUTO == str )
		{
			this->m_nConfigHeight = AUTO;
		}
		else
			this->m_nConfigHeight = _ttoi( str.c_str() );

		this->m_mapAttribute.erase( XML_HEIGHT );
	}
// 	if( this->m_mapAttribute.count( XML_MINWIDTH ) )
// 	{
// 		String str = this->m_mapAttribute[ XML_MINWIDTH ];
// 		this->minwidth = _ttoi( str.c_str() );
// 		this->m_mapAttribute.erase( XML_MINWIDTH );
// 	}
// 	if( this->m_mapAttribute.count( XML_MINHEIGHT ) )
// 	{
// 		String str = this->m_mapAttribute[ XML_MINHEIGHT ];
// 		this->minheight = _ttoi( str.c_str() );
// 		this->m_mapAttribute.erase( XML_MINHEIGHT );
// 	}
// 	if( this->m_mapAttribute.count( XML_MAXWIDTH ) )
// 	{
// 		String str = this->m_mapAttribute[ XML_MAXWIDTH ];
// 		this->maxwidth = _ttoi( str.c_str() );
// 		this->m_mapAttribute.erase( XML_MAXWIDTH );
// 	}
// 	if( this->m_mapAttribute.count( XML_MAXWIDTH ) )
// 	{
// 		String str = this->m_mapAttribute[ XML_MAXWIDTH ];
// 		this->maxwidth = _ttoi( str.c_str() );
// 		this->m_mapAttribute.erase( XML_MAXWIDTH );
//	}

	if( this->m_mapAttribute.count( XML_MARGIN ) )
	{
		String str = this->m_mapAttribute[ XML_MARGIN ];
		Util::TranslateRECT( str, &this->m_rcMargin );
		this->m_mapAttribute.erase( XML_MARGIN );
	}
	if( this->m_mapAttribute.count( XML_PADDING ) )
	{
		String str = this->m_mapAttribute[ XML_PADDING ];
		REGION4 rcPadding;
		Util::TranslateRECT( str, &rcPadding );
		this->m_mapAttribute.erase( XML_PADDING );

		this->SetPaddingRegion((CRegion4*)&rcPadding);
	}
	if( this->m_mapAttribute.count( XML_BORDER ) )
	{
		String str = this->m_mapAttribute[ XML_BORDER ];
		REGION4 rcBorder;
		Util::TranslateRECT( str, &rcBorder );
		this->m_mapAttribute.erase( XML_BORDER );

		this->SetBorderRegion((CRegion4*)&rcBorder);
	}

	// ���ñ�����Ⱦ��
	if( this->m_mapAttribute.count( XML_BACKGND_RENDER_PRIFIX XML_RENDER_TYPE ) )
	{
		SAFE_DELETE(m_pBkgndRender);

		const String& strBkgndRenderType = mapAttrib[XML_BACKGND_RENDER_PRIFIX XML_RENDER_TYPE];
		this->m_pBkgndRender = RenderFactory::GetRender( strBkgndRenderType, this );
		if( NULL != this->m_pBkgndRender )
		{
			this->m_pBkgndRender->SetAttribute( XML_BACKGND_RENDER_PRIFIX, mapAttrib );
		}
		else
		{
			UI_LOG_WARN( _T("Object::SetAttribute, Invalid %s=\"%s\""), XML_BACKGND_RENDER_PRIFIX XML_RENDER_TYPE, strBkgndRenderType.c_str() );
		}
		this->m_mapAttribute.erase(XML_BACKGND_RENDER_PRIFIX XML_RENDER_TYPE);
	}
	// ����ǰ������
	if( this->m_mapAttribute.count( XML_FOREGND_RENDER_PRIFIX XML_RENDER_TYPE ) )
	{
		SAFE_DELETE(m_pForegndRender);

		const String& strForegndRenderType = mapAttrib[XML_FOREGND_RENDER_PRIFIX XML_RENDER_TYPE];
		this->m_pForegndRender = RenderFactory::GetRender( strForegndRenderType, this );
		if( NULL != this->m_pForegndRender )
		{
			this->m_pForegndRender->SetAttribute( XML_FOREGND_RENDER_PRIFIX, mapAttrib );
		}
		else
		{
			UI_LOG_WARN( _T("Object::SetAttribute, Invalid%s=\"%s\""), XML_FOREGND_RENDER_PRIFIX XML_RENDER_TYPE, strForegndRenderType.c_str() );
		}
		this->m_mapAttribute.erase(XML_FOREGND_RENDER_PRIFIX XML_RENDER_TYPE);
	}

	if( NULL == m_pTextRender )
	{
		m_pTextRender = TextRenderFactory::GetTextRender( TEXTRENDER_TYPE_NORMAL, this );
	}
	m_pTextRender->SetAttribute(_T(""),mapAttrib);

	if( this->m_mapAttribute.count( XML_BACKGND_IS_TRANSPARENT ) )
	{
		if( _ttoi(this->m_mapAttribute[ XML_BACKGND_IS_TRANSPARENT ].c_str() ) )
		{
			this->ModifyStyle(OBJECT_STYLE_TRANSPARENT);
		}
		else
		{
			this->ModifyStyle(0,OBJECT_STYLE_TRANSPARENT);
		}
		this->m_mapAttribute.erase( XML_BACKGND_IS_TRANSPARENT );
	}
	
	// �����ʽ
	if( this->m_mapAttribute.count( XML_CURSOR ) )
	{
		String str = this->m_mapAttribute[ XML_CURSOR ];
		::UI_GetCursor( str, &m_pCursor );
		this->m_mapAttribute.erase( XML_CURSOR );
	}

	// �ؼ���ʽ
	if( this->m_mapAttribute.count( XML_CONTROL_STYLE_TABABLE ))
	{
		String str = this->m_mapAttribute[XML_CONTROL_STYLE_TABABLE];
		if(_T("1") == str)
		{
			m_nStyle |= CONTROL_STYLE_TABSTOP;
		}
		else
		{
			m_nStyle &= ~ CONTROL_STYLE_TABSTOP;
		}
	}

	if( this->m_mapAttribute.count( XML_CONTROL_STYLE_GROUP ))
	{
		String str = this->m_mapAttribute[XML_CONTROL_STYLE_GROUP];
		if(_T("1") == str)
		{
			m_nStyle |= CONTROL_STYLE_GROUP;
		}
		else
		{
			m_nStyle &= ~CONTROL_STYLE_GROUP;
		}
	}

	return true;
}


bool Object::SetChildObjectAttribute( Object* pChildObj, const String& strPrifix, map<String,String>& mapAttrib, bool bReload )
{
	if( NULL == pChildObj )
		return false;

	// ���ð�ť�����ԣ���ȡ�� btn.��ͷ��
	map<String,String>::iterator  iter = mapAttrib.begin();
	map<String,String>::iterator  iterEnd = mapAttrib.end();

	map<String,String>  mapChildObjAttrib;
	for (; iter != iterEnd; iter++ )
	{
		TCHAR*  szKey = (TCHAR*)iter->first.c_str();
		if( _tcsstr( szKey, strPrifix.c_str() ) == szKey )
		{
			mapChildObjAttrib[ szKey + strPrifix.length() ] = iter->second;
			this->m_mapAttribute.erase(iter->first);
		}
	}
	String strStyleClass;
	if( mapChildObjAttrib.count(XML_STYLECLASS) )
	{
		strStyleClass = mapChildObjAttrib[XML_STYLECLASS];
	}
	::UI_LoadStyle(pChildObj->GetObjectName(), strStyleClass, pChildObj->m_strID, mapChildObjAttrib );
	return pChildObj->SetAttribute(mapChildObjAttrib,bReload);  // ��ʹΪ�գ�ҲҪ֪ͨ��ȥ
}

// ����padding��ֵ��ͬʱ���·ǿͻ����Ĵ�С
void Object::SetPaddingRegion(CRegion4* prc)
{
	if (NULL == prc)
		return;

	REGION4 rcOther = { 
		m_rcNonClient.left   - m_rcPadding.left,
		m_rcNonClient.top    - m_rcPadding.top,
		m_rcNonClient.right  - m_rcPadding.right,
		m_rcNonClient.bottom - m_rcPadding.bottom };

	m_rcPadding.CopyRect(prc);
	m_rcNonClient.SetRect(
		m_rcPadding.left   + rcOther.left,
		m_rcPadding.top    + rcOther.top,
		m_rcPadding.right  + rcOther.right,
		m_rcPadding.bottom + rcOther.bottom );
}

void Object::SetBorderRegion( CRegion4* prc )
{
	if (NULL == prc)
		return;

	REGION4 rcOther = { 
		m_rcNonClient.left   - m_rcBorder.left,
		m_rcNonClient.top    - m_rcBorder.top,
		m_rcNonClient.right  - m_rcBorder.right,
		m_rcNonClient.bottom - m_rcBorder.bottom };

	m_rcBorder.CopyRect(prc);
	m_rcNonClient.SetRect(
		m_rcBorder.left   + rcOther.left,
		m_rcBorder.top    + rcOther.top,
		m_rcBorder.right  + rcOther.right,
		m_rcBorder.bottom + rcOther.bottom );
}

// Ҫ���Ƹö���֮ǰ����ȡ�ö����ڴ����е�ʵ��λ�ã���������ƫ�����Ͳü���
POINT Object::GetRealPosInWindow()
{
	int xOffset = 0;
	int yOffset = 0;

	Object* pObjParent = NULL;
	Object* pObjChild = this;
	while (pObjParent = this->EnumParentObject(pObjParent))
	{
		xOffset += pObjChild->m_rcParent.left;
		yOffset += pObjChild->m_rcParent.top;

		if (!pObjChild->IsNcObject())
		{
			int xScroll = 0, yScroll = 0;
			if (pObjParent->GetScrollOffset(&xScroll, &yScroll))
			{
				xOffset -= xScroll;
				yOffset -= yScroll;
			}
		}

		pObjChild = pObjParent;
	}

	POINT pt = {xOffset, yOffset};
	return pt;
}


// ͨ����һ����������ѡ���豸�����������Լ��ļ������������������ʹ���豸���ꡣ
// GDIΪ����������һ�ݸ����������ڽ���ѡ���豸����֮��ʹ���߿���ɾ������

// [�ݹ�] ���ö������Ӷ�����Ƶ�hRDC����
//
// ��Ϊ�ӿؼ��ķ�Χ����Ҫ��parent����������������ȡ�Ӻ͸��Ľ���clip rgn���л���  
void Object::DrawObject(HRDC hRDC, RenderOffsetClipHelper roc)
{
	if (!this->IsVisible())
		return;

	::UISendMessage(this, WM_ERASEBKGND, (WPARAM)hRDC, (LPARAM)1 );  // ��lparam��Ϊ1����ԭʼ��Ϣ��������
	this->_drawNcChildObject(hRDC, roc);

	roc.DrawClient(hRDC, this, false);
	roc.Scroll(hRDC, this, false);
	roc.Update(hRDC);

	::UISendMessage(this, WM_PAINT, (WPARAM)hRDC, (LPARAM)1 );       // ��lparam��Ϊ1����ԭʼ��Ϣ��������
	this->_drawChildObject(hRDC, roc);
}

void Object::_drawChildObject(HRDC hRDC, RenderOffsetClipHelper& roc)
{
	Object*  pChild = NULL;
	while( pChild = EnumChildObject(pChild) )
	{
		if( pChild->IsVisible() && !testStateBit(CSB_PREVENTREDRAW) )
		{
			RenderOffsetClipHelper rocChild = roc;
			if (rocChild.DrawChild(pChild, hRDC))
				pChild->DrawObject(hRDC, rocChild);
		}
	}
}

void Object::_drawNcChildObject(HRDC hRDC, RenderOffsetClipHelper& roc)
{
	Object*  pChild = NULL;
	while( pChild = EnumNcChildObject(pChild) )
	{
		if( pChild->IsVisible() && !testStateBit(CSB_PREVENTREDRAW) )
		{
			RenderOffsetClipHelper rocChild = roc;
			if (rocChild.DrawChild(pChild, hRDC))
				pChild->DrawObject(hRDC, rocChild);
		}
	}
}

// �����͸�������Ļ�����Ҫ��ȡ�������еı���
// ����ʹ�ǲ�͸����object��Ҳ��Ҫһ������������ȡ���ļ�������
//
//   Ϊ����Ӧ�ڶ������ص�ʱ���ܹ�ˢ���Լ��ĸ����󱳾�����Ҫ����һ�������������Լ���͸������
//   �������bSelfTransparent������ͨˢ�µ�ʱ�򴫵�this->IsTransparen()������ʱ��ֱ�Ӵ���false
void Object::DrawObjectTransparentBkgnd(HRDC hRDC, RenderOffsetClipHelper& roc, bool bSelfTransparent)
{
	bool    bTransparent = bSelfTransparent;
	Object* pObjFirstParentToDrawBk = NULL;
	
	if (bTransparent)
	{
		// ����游��������û�в�͸���ģ��Ӹø�������
		Object* pObjParent = NULL;
		while (pObjParent = this->EnumParentObject(pObjParent))
		{
			if (!pObjParent->IsTransparent())
			{
				pObjFirstParentToDrawBk = pObjParent;
				break;
			}
		}
	}
	else
	{
		pObjFirstParentToDrawBk = this;   // ��Ȼ����Ҫ���ƣ���������Ҫ���ݸ��������ƫ��
	}

	Object* pObjParent = NULL;
	while (pObjParent = this->REnumParentObject(pObjParent))
	{
		if (OBJ_WINDOW != pObjParent->GetObjectType())
		{
			if (!pObjParent->IsNcObject())
			{
				roc.Scroll(hRDC, pObjParent->GetParentObject(), false);
				roc.DrawClient(hRDC, pObjParent->GetParentObject(), false);
			}
			roc.DrawChild(pObjParent,hRDC);
		}

		if (NULL != pObjFirstParentToDrawBk)
		{
			if (pObjParent != pObjFirstParentToDrawBk)  // ��pObjFirstParentToDrawBk��ʼ����
				continue;

			pObjFirstParentToDrawBk = NULL;             // ������
		}

		::UISendMessage(pObjParent, WM_ERASEBKGND, (WPARAM)hRDC, (LPARAM)1 );
	}

	if (!this->IsNcObject())
	{
		roc.Scroll(hRDC, this->GetParentObject(), false);
		roc.DrawClient(hRDC, this->GetParentObject(), false);
	}
	roc.DrawChild(this, hRDC);
}

//
// ��ȡ�ö����ƫ����
//
// ReturnQ
//		����false��ʾ�ö����޹�������
//
bool Object::GetScrollOffset(int* pxOffset, int* pyOffset)
{
	if(NULL == pxOffset || NULL == pyOffset)
		return false;

	*pxOffset = 0; *pyOffset = 0;

	bool bHScroll = this->TestStyle(OBJECT_STYLE_HSCROLL);
	bool bVScroll = this->TestStyle(OBJECT_STYLE_VSCROLL);
	if (bHScroll || bVScroll)
	{
		::UISendMessage(this, UI_WM_GETSCROLLOFFSET, (WPARAM)pxOffset, (LPARAM)pyOffset);
		return true;
	}

	return false;
}

void Object::WindowPoint2ObjectPoint(const POINT* ptWindow, POINT* ptObj)
{
	if (NULL == ptObj || NULL == ptWindow)
		return;

	POINT pt = this->GetRealPosInWindow();
	ptObj->x = ptWindow->x - pt.x;
	ptObj->y = ptWindow->y - pt.y;
}
void Object::WindowPoint2ObjectClientPoint(const POINT* ptWindow, POINT* ptClient)
{
	if (NULL == ptClient || NULL == ptWindow)
		return;

	POINT pt = this->GetRealPosInWindow();
	ptClient->x = ptWindow->x - pt.x - m_rcNonClient.left;
	ptClient->y = ptWindow->y - pt.y - m_rcNonClient.top;
}

void Object::ObjectPoint2ObjectClientPoint(const POINT* ptObj, POINT* ptClient)
{
	if (NULL == ptObj || NULL == ptClient)
		return;

	ptClient->x = ptObj->x - m_rcNonClient.left;
	ptClient->y = ptObj->y - m_rcNonClient.top;
}
void Object::GetWindowRect(CRect* lprc)
{
	if (NULL == lprc)
		return;
	
	lprc->CopyRect(&m_rcParent);
	if (NULL != this->GetParentObject())
	{
		POINT pt = this->GetParentObject()->GetRealPosInWindow();
		lprc->OffsetRect(pt.x, pt.y);
	}
}
//////////////////////////////////////////////////////////////////////////
//

BOOL Object::OnEraseBkgnd( HRDC hRDC )
{
	if( NULL == m_pBkgndRender )
	{
		return FALSE;
	}
	else
	{
		CRect rc(0,0, this->GetWidth(), this->GetHeight());
		m_pBkgndRender->DrawState(hRDC, &rc, 0);
		return TRUE;
	}
}

//
//	����Object��Ĭ�������ʽ
//
//	Remark:
//		�����ʵ�ָ���Ϣ�����ܵ�������ƶ���һ��Object����ʱ����Ȼ��ʾ����Window��OnSetCursor
//
BOOL Object::OnSetCursor( HWND hWnd, UINT nHitTest, UINT message )
{
	if( NULL == m_pCursor || NULL == m_pCursor->GetCursor())
	{
		::SetCursor( ::LoadCursor(NULL, IDC_ARROW) );
 		return TRUE;
	}

	::SetCursor( m_pCursor->GetCursor() );
	return TRUE;
}

//
//	��ȡ�Լ�������,���ﷵ�صĶ���ֻ��һ����ʱ����
//	�����Ҫ����ʹ�ã�����Ҫ����UI_CopyFont/ui_releasefont
//
HRFONT Object::GetFont()
{
	if( NULL == m_pTextRender || NULL == m_pTextRender->GetHRFONT() )
	{
		WindowBase* pWindow = GetWindowObject();
		if (NULL != pWindow)
		{
			return pWindow->GetHRFONT();
		}
	}
	else
	{
		return m_pTextRender->GetHRFONT();
	}

	return NULL;
}

//
// ��������Լ���û�����壬���򸸶�����ȡ
//
HRFONT Object::OnGetFont()
{
// 	if( NULL == m_hFont )
// 	{
// 		if( NULL != m_pParent )
// 		{
// 			return (HRFONT)UISendMessage(m_pParent, WM_GETFONT );
// 		}
// 	}
// 	else
// 	{
// 		return m_hFont;
// 	}

	// ����ʹ��ͨ��UISendMessageЧ��û��ֱ��GetFont�ߣ���˷���������Ϣ
	return GetFont();
}



//
//	[public] [virtual]  UINT HitTest( POINT* pt )
//
//	Parameters
//		pt
//			[in]  Ҫ��̽��λ�ã�ͨ��Ϊ��굱ǰλ��
//
//	Return
//		��겻�ڶ������о�Χ�ڷ���HTNOWHERE�����򷵻�HTCLIENT�����������������չ��
//
//	Remark
//		����������������һ�����⣬��ΪObject�䱸һ��RECT�����ʾ��Χ�����Ǹ�һ����ǿ���RGN��ʾ��Χ?
//		����RGN���������ڴ棬����ʱ��͵�ʹ��RGN��ʵ��ĳЩ���ܣ������������������������ߵ����ߣ�
//		�����λ�ô�����ʲô��λ��ͬʱ��Object�䱸һ��RECT������ʾÿһ������ķ�Χ����Ϊ�󲿷�ʱ��ʹ�õĶ���RECT����
//
UINT Object::OnHitTest( POINT* pt )
{
	BOOL  bIn = FALSE;

	if( this->m_hRgn != NULL )
	{
		bIn = ::PtInRegion(m_hRgn, pt->x, pt->y);
		return HTCLIENT;
	}
	else
	{
		bIn = ::PtInRect(&m_rcParent, *pt);

		if (bIn)
		{
			RECT rcClient = {
				m_rcParent.left + m_rcNonClient.left,
				m_rcParent.top  + m_rcNonClient.top,
				m_rcParent.right - m_rcNonClient.right,
				m_rcParent.bottom - m_rcNonClient.bottom
			};

			if (::PtInRect(&rcClient, *pt))
				return HTCLIENT;
			else
				return HTBORDER;
		}
	}

	return HTNOWHERE;
}


/*
**	[private] bool    testStateBit( BYTE nbit );
**	[private] bool    setStateBit( BYTE nbit );
**
**	parameters
**		nbit
**			[in] Ҫ���Եı���λ����OSB_VISIBLE��OSB_DISABLE��CSB_TABABLE��CSB_DEFAULT
**				 CSB_FOCUS��CSB_READONLY��CSB_HOVER��CSB_PRESS
**	
**	return
**		��λΪ1������true����λΪ0������false
**
**	remark
**		Ϊ�˽�ʡ�ڴ�ռ�ã���һ���ؼ��Ķ���״̬����Ϊbitλ���д���
*/
bool Object::testStateBit( UINT bit )
{
	if( m_nStateBit & bit )
		return true;
	else 
		return false;
}
void Object::setStateBit( UINT bit )
{
	m_nStateBit |= bit;
}
void Object::clearStateBit( UINT nbit )
{
	m_nStateBit &= ~nbit;
}

void Object::ModifyStyle( UINT nStyleAdd, UINT nStyleRemove )
{
	if( nStyleAdd != 0 )
	{
		m_nStyle |= nStyleAdd;
	}
	if( nStyleRemove != 0 )
	{
		m_nStyle &= ~nStyleRemove;
	}
}

bool Object::IsTransparent()
{
	if( m_nStyle & OBJECT_STYLE_TRANSPARENT )
		return true;

	return false;
}

bool Object::IsFocus()
{
	return this->testStateBit( CSB_FOCUS );
}
void Object::SetFocus( bool b )
{
	if( b )
	{
		this->setStateBit( CSB_FOCUS );
	}
	else
	{
		this->clearStateBit( CSB_FOCUS );
	}
}
bool Object::IsTabstop()
{
	if( this->GetObjectType() == OBJ_HWNDHOST )
	{
		return (::GetWindowLong(((HwndHost*)this)->m_hWnd, GWL_STYLE) & WS_TABSTOP) ?true:false;
	}
	return (m_nStyle & CONTROL_STYLE_TABSTOP)?true:false;
}
bool Object::IsGroup()
{
	return  (m_nStyle&CONTROL_STYLE_GROUP)?true:false;
}

void Object::SetTabstop( bool b )
{
	if( b )
		this->ModifyStyle( CONTROL_STYLE_TABSTOP );
	else
		this->ModifyStyle( 0, CONTROL_STYLE_TABSTOP );
}
void Object::SetGroup( bool b )
{
	if( b )
		this->ModifyStyle( CONTROL_STYLE_GROUP );
	else
		this->ModifyStyle( 0, CONTROL_STYLE_GROUP );
}

bool Object::IsMySelfVisible()
{
	bool bVisible = !this->testStateBit( OSB_UNVISIBLE );
	return bVisible;
}
bool Object::IsVisible()
{
	if( this->GetObjectType() == OBJ_WINDOW )
	{
		return ::IsWindowVisible(((WindowBase*)this)->m_hWnd)?true:false;
	}
	if( this->GetObjectType() == OBJ_HWNDHOST )
	{
		return ::IsWindowVisible(((HwndHost*)this)->m_hWnd)?true:false;
	}
	bool bVisible = !this->testStateBit( OSB_UNVISIBLE );
	if( false == bVisible || NULL == m_pParent )
	{
		return bVisible;
	}
	// �жϸ����󲻿ɼ������ 
	else
	{
		if( OBJ_WINDOW == m_pParent->GetObjectType() )
		{ 
			return true;           // ע�������ﲻ�������Ĵ��ڽ����жϵ�ԭ���ǣ��������ڴ��ڳ�ʼ���ĺ������棬��Ȼ������ʱ�ǲ��ɼ��ģ�������Ķ����IsVisibleӦ���Ƿ���true�źô���
		}
		else
		{
			return m_pParent->IsVisible();
		}
	}
}

bool Object::IsCollapsed()
{
	// TODO:  
	UIASSERT(0);
	return false;
}

bool Object::IsEnable()
{
	if( this->GetObjectType() == OBJ_WINDOW )
	{
		return ::IsWindowEnabled(((WindowBase*)this)->m_hWnd)?true:false;
	}
	if( this->GetObjectType() == OBJ_HWNDHOST )
	{
		return ::IsWindowEnabled(((HwndHost*)this)->m_hWnd)?true:false;
	}
	return !this->testStateBit( OSB_DISABLE );
}

//
//	��ע���ֲ㴰���У�����һ������ʱ������ֱ�ӵ���::InvalidateRect(&rc..)
//        ������� GetWindowObject(); pWindow->InvalidateObject(GetWindowObject(),...);
//
void Object::SetVisible( bool b, bool bUpdateNow )
{
	bool bOld = IsVisible();

	if( this->GetObjectType() == OBJ_WINDOW )
	{
		::ShowWindow(((WindowBase*)this)->m_hWnd, b?SW_SHOW:SW_HIDE);
		return;
	}
	else if( this->GetObjectType() == OBJ_HWNDHOST )
	{
		::ShowWindow(((HwndHost*)this)->m_hWnd, b?SW_SHOW:SW_HIDE);
		return;
	}

	if( b )
		this->clearStateBit( OSB_UNVISIBLE );
	else
		this->setStateBit( OSB_UNVISIBLE );

	if( b != bOld )
	{
		this->UpdateLayout(bUpdateNow);	   // �������ɼ��Ļ�����UpdateLayout�Ḻ�����
		if(bUpdateNow )
		{
			if (b)
				this->UpdateObject(bUpdateNow); 
			else
				this->UpdateObjectBkgnd(bUpdateNow); 
		}
	}

	// ������صĶ�����һ����������򽫽��������лص���һ������
	if( false == b )
	{
		WindowBase* pWindow = this->GetWindowObject();
		if (NULL != pWindow)
		{
			pWindow->GetKeyboardMgr().OnObjectHideInd(this);
		}
	}
}
void Object::SetEnable( bool b )
{
	if( this->GetObjectType() == OBJ_WINDOW )
	{
		::EnableWindow(((WindowBase*)this)->m_hWnd, b?TRUE:FALSE);
		return;
	}
	else if( this->GetObjectType() == OBJ_HWNDHOST )
	{
		::EnableWindow(((HwndHost*)this)->m_hWnd, b?TRUE:FALSE);
		return;
	}

	bool bOld = IsEnable();
	if( b )
		this->clearStateBit( OSB_DISABLE );
	else
		this->setStateBit( OSB_DISABLE );
		
	if( b != bOld )
		this->UpdateObject();
}


bool Object::IsDefault()
{
	return this->testStateBit( CSB_DEFAULT );
}
bool Object::IsReadonly()
{
	return this->testStateBit( CSB_READONLY );
}
bool Object::IsHover()
{
	return this->testStateBit( CSB_FORCEHOVER ) || this->testStateBit( CSB_HOVER );
}
bool Object::IsPress()
{
	return this->testStateBit( CSB_FORCEPRESS ) || this->testStateBit( CSB_PRESS );
}
bool Object::IsForeHover()
{
	return this->testStateBit( CSB_FORCEHOVER );
}
bool Object::IsForePress()
{
	return this->testStateBit( CSB_FORCEPRESS );
}

void Object::SetDefault( bool b )
{
	if( b )
		this->setStateBit( CSB_DEFAULT );
	else
		this->clearStateBit( CSB_DEFAULT );
}

void Object::SetReadonly( bool b )
{
	if( b )
		this->setStateBit( CSB_READONLY );
	else
		this->clearStateBit( CSB_READONLY );
}
void Object::SetForceHover( bool b )
{
	if( b )
		this->setStateBit( CSB_FORCEHOVER );
	else
		this->clearStateBit( CSB_FORCEHOVER );
}
void Object::SetForcePress( bool b )
{
	if( b )
		this->setStateBit( CSB_FORCEPRESS );
	else
		this->clearStateBit( CSB_FORCEPRESS );
}
void Object::SetHover( bool b )
{
	if( b )
		this->setStateBit( CSB_HOVER );
	else
		this->clearStateBit( CSB_HOVER );
}
void Object::SetPress( bool b )
{
	if( b )
		this->setStateBit( CSB_PRESS );
	else
		this->clearStateBit( CSB_PRESS );
}
void Object::SetActive(bool b)
{
	if( b )
		this->setStateBit( WSB_ACTIVE );
	else
		this->clearStateBit( WSB_ACTIVE );
}
bool Object::IsActive()
{
	return this->testStateBit( WSB_ACTIVE );
}

int Object::GetWidth()
{
	return m_rcParent.Width();
}
int Object::GetHeight()
{
	return m_rcParent.Height();
}

// ע�������clientrect ������0,0���ģ����������Object���ϽǶ���
void Object::GetClientRect(CRect* prc)
{
	if (NULL == prc)
		return;

	prc->SetRect(
		m_rcNonClient.left, 
		m_rcNonClient.top, 
		this->GetWidth() - m_rcNonClient.right, 
		this->GetHeight()- m_rcNonClient.bottom); 
}

// ע�������clientrect ��0,0����
void Object::GetClientRectAsWin32( CRect* prc )
{
	if (NULL == prc)
		return;

	prc->SetRect(
		0, 
		0, 
		this->GetWidth() - m_rcNonClient.Width(), 
		this->GetHeight()- m_rcNonClient.Height()); 
}
void Object::SetNonClientRegion( CRegion4* prc )
{
	m_rcNonClient.SetRect(
		prc->left + m_rcPadding.left + m_rcBorder.left,
		prc->top + m_rcPadding.top + m_rcBorder.top,
		prc->right + m_rcPadding.right + m_rcBorder.right,
		prc->bottom + m_rcPadding.bottom + m_rcBorder.bottom);
}
void Object::GetParentRect( CRect* prc )
{
	if (NULL == prc)
		return;

	prc->CopyRect(&m_rcParent);
}

void Object::SetObjectPos( int x, int y, int cx, int cy, int nFlag )
{
	bool bMove = (nFlag&SWP_NOMOVE)?false:true;
	bool bSize = (nFlag&SWP_NOSIZE)?false:true;

	if( bMove&&bSize )
	{
		
	}
	else if( bMove )
	{
		cx = this->GetWidth();
		cx = this->GetHeight();
	}
	else if( bSize )
	{
		x = m_rcParent.left;
		y = m_rcParent.top;
	}
	else
	{
		return;  // DONOTHING
	}

// 	if( bMove || bSize )  // TODO: �ָ�����д
// 	{
// 		// ˢ���ƶ�ǰ������λ��
// 		if( !(nFlag & SWP_NOREDRAW) )
// 		{
// 			this->UpdateObject(&m_rcWindow, false);
// 		}
// 	}

	if( this->GetObjectType() == OBJ_WINDOW )
	{
		// ���ڴ�����˵���������õ��Ƿǿͻ����Ĵ�С
		WindowBase* pThis = (WindowBase*)this;
		::SetWindowPos(pThis->m_hWnd, NULL, x, y, cx, cy, SWP_NOZORDER|SWP_NOACTIVATE);
		::GetClientRect(pThis->m_hWnd, &m_rcParent);
	}
	else
	{
		::SetRect(&m_rcParent, x,y,x+cx,y+cy);
	}

// 	if( bMove || bSize )  // TODO: ��д���ָ�
// 	{
// 		// ˢ���ƶ��������λ��
// 		if( !(nFlag & SWP_NOREDRAW) )
// 		{
// 			this->UpdateObject(&m_rcWindow, true);
// 		}
// 	}
	// MSDN: MoveWindow sends the WM_WINDOWPOSCHANGING, WM_WINDOWPOSCHANGED, WM_MOVE, WM_SIZE, and WM_NCCALCSIZE messages to the window. 
	// ������������ʱֻ�ȷ���WM_MOVE/WM_SIZE��Ϣ
	if (bMove)
		UISendMessage( this, WM_MOVE, 0, MAKELPARAM(m_rcParent.left,m_rcParent.top) );
	if (bSize)
		UISendMessage( this, WM_SIZE, 0, MAKELPARAM(m_rcParent.Width(),m_rcParent.Height()) );
}
void Object::SetObjectPos( CRect* prc, int nFlag )
{
	if (NULL == prc)
		return;

	this->SetObjectPos(prc->left, prc->top, prc->Width(), prc->Height(), nFlag);
}

bool Object::GetAttribute( const String& strKey, String& strValue )
{
	if( m_mapAttribute.count(strKey) )
	{
		strValue = m_mapAttribute[strKey];
		return true;
	}

	return false;
}

void Object::SetBkRender(RenderBase* p)
{
	SAFE_DELETE(m_pBkgndRender);
	m_pBkgndRender = p;
}
void Object::SetForeRender(RenderBase* p)
{
	SAFE_DELETE(m_pForegndRender);
	m_pForegndRender = p;
}
//////////////////////////////////////////////////////////////////////////
//                                                                      //
//                               ��Ϣӳ��                               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


//
//   �����յ�WM_DESTROY��Ϣ֮�󣬴��ݸ����е��Ӷ���
//
//void Object::OnDestroy()
//{
//	Object*  pObj = NULL;
//	while( pObj = EnumChildObject(pObj) )
//	{
//		// ���Ӷ�����WM_DESTROY��Ϣ
//		UIMsg  cloneMsg = *(this->m_pCurMsg);
//
//		cloneMsg.pObjMsgTo = pObj;
//		::UISendMessage( &cloneMsg );
//	}
//}


//void Object::OnPaint( HDC hDC )


// �ڽ��յ�OnSize��Ϣ��ʱ����Ҫ�ı��Լ���pos,rectP,rectW��Ա��
// ��֪ͨ�Լ����Ӷ���ȥ����
//void Object::OnSize( UINT nType, int cx, int cy )
//{
//	this->UpdateRect();
//
//	Object*  pObj = NULL;
//	while( pObj = EnumChildObject(pObj) )
//	{
//		// ���Ӷ�����WM_ERASEBKGND��Ϣ
//		UIMsg  cloneMsg = *(this->m_pCurMsg);
//		cloneMsg.pObjMsgTo = pObj;
//		::UISendMessage( &cloneMsg );
//	}
//}

