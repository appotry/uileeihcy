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

 // 注意：不要在构造或者析构函数中调用虚函数
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
//	重绘这个对象(一般是鼠标移上移下，状态变化等时候的重绘)
//
//	1. 不透明的控件，直接在原处绘即可，包括了背景为COLOR、Image的情况
//		TODO: 考虑将ButtonBkRender再抽象出来，给所有的控件使用
//	2. 透明的控件...是否需要考虑下优化为直接取到自己在父窗口下的背景，而不是刷新整个窗口
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

// 当对象隐藏/移动的时候，刷新背景
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
//	重绘这个对象所在的区域（一般是窗口隐藏、移动、大小改变等时候的重绘）
//
//	prc 一般为 &m_rcWindow
//
// void Object::UpdateObject( RECT* prc, bool bUpdateNow ) 
// {
// //	由于调用该函数的地方该对象可能已经隐藏了，所以这里不对是否可见进行判断
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
//	当对象显示/隐藏，或者大小发生改变时，重新刷新自己所在layout的布局
//
//	1. 获取自己的所在Layout
//	2. 测量Layout的大小是否发生改变，如果这个Layout大小改变，再获取上一层Layout
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
//	获取自己的祖父对象列表(到window类型为止)，由近到远插入在vector中
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
//	获取自己在所在窗口中的clip区域（子对象必须中父对象内部）
//
//	返回的HRGN对象由调用者进行释放
//
HRGN Object::GetClipRgnInWindow()
{
	HRGN    hRetRgn = NULL;

	// TODO: 重写
// 	Object *pObject = NULL;
// 	while ( pObject = pObject->REnumParentObject(pObject) )
// 	{
// 		HRGN hClipRgn = NULL;
// 		if( pObject->GetRgn() != NULL )  // 该对象不是一个正规RECT区域，直接使用它的RGN对象
// 		{
// 			hClipRgn = pObject->GetRgn();
// 		}
// 		else
// 		{
// 			hClipRgn = ::CreateRectRgn( pObject->GetWindowRectL(), pObject->GetWindowRectT(), pObject->GetWindowRectR(), pObject->GetWindowRectB() );
// 		}
// 
// 		// 因为子控件的范围可能要比parent还大，因此在这里必须取子和父的交集clip rgn进行绘制
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

// 获取一个控件所在窗口句炳
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
**	[public] 获取某一具体路径下的子对象，但这里的子对象也可是子对象的子对象。
**
**	parameter
**		objIdPath
**			[in]	要获取的对象的路径，不包括自己本身，路径以/分隔
**	
**	return
**		成功返回该对象，失败返回NULL。失败可查看日志输出
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
// 		UI_LOG_ERROR( _T("Object::GetObjectItem，无效的参数:%s"), objIdPath.c_str() );
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
// 		// 没有找到指定的对象
// 		if( NULL == pTempObj )
// 		{
// 			UI_LOG_ERROR( _T("Object::GetObjectItem，未找到指定的对象在%s下："), this->m_strID.c_str(), objIdPath.c_str() );
// 			return NULL;
// 		}
// 	}
// 
// 	return pObjRet;
// }

//
//	在所有的子子孙孙中查找id为objId的对象，只返回第一个找到的对象
//
//	Parameter
//		objId
//			[in]	要查找的对象id
//
//	Return
//		成功返回对象地址，失败返回NULL
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
	//  1. 将属性拷贝过来
	//
	this->m_mapAttribute.clear();
	this->m_mapAttribute = mapAttrib;

	//
	//	2. 设置id属性
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

	//  3. 设置布局相关值
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

	// 设置背景渲染器
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
	// 设置前景绘制
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
	
	// 鼠标样式
	if( this->m_mapAttribute.count( XML_CURSOR ) )
	{
		String str = this->m_mapAttribute[ XML_CURSOR ];
		::UI_GetCursor( str, &m_pCursor );
		this->m_mapAttribute.erase( XML_CURSOR );
	}

	// 控件样式
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

	// 设置按钮的属性，读取以 btn.开头的
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
	return pChildObj->SetAttribute(mapChildObjAttrib,bReload);  // 即使为空，也要通知过去
}

// 设置padding的值，同时更新非客户区的大小
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

// 要绘制该对象之前，获取该对象在窗口中的实现位置，用于设置偏移量和裁剪区
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


// 通过将一个剪裁区域选进设备内容来建立自己的剪裁区域，这个剪裁区域使用设备坐标。
// GDI为剪裁区域建立一份副本，所以在将它选进设备内容之后，使用者可以删除它。

// [递归] 将该对象及其子对象绘制到hRDC上面
//
// 因为子控件的范围可能要比parent还大，因此在这里必须取子和父的交集clip rgn进行绘制  
void Object::DrawObject(HRDC hRDC, RenderOffsetClipHelper roc)
{
	if (!this->IsVisible())
		return;

	::UISendMessage(this, WM_ERASEBKGND, (WPARAM)hRDC, (LPARAM)1 );  // 将lparam置为1，与原始消息进行区分
	this->_drawNcChildObject(hRDC, roc);

	roc.DrawClient(hRDC, this, false);
	roc.Scroll(hRDC, this, false);
	roc.Update(hRDC);

	::UISendMessage(this, WM_PAINT, (WPARAM)hRDC, (LPARAM)1 );       // 将lparam置为1，与原始消息进行区分
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

// 如果是透明背景的话，需要获取父窗口中的背景
// 但即使是不透明的object，也需要一层层遍历下来获取它的剪裁区域
//
//   为了适应于对象隐藏的时候能够刷新自己的父对象背景，需要增加一个场景来忽略自己的透明属性
//   因此增加bSelfTransparent，当普通刷新的时候传递this->IsTransparen()，其它时候直接传递false
void Object::DrawObjectTransparentBkgnd(HRDC hRDC, RenderOffsetClipHelper& roc, bool bSelfTransparent)
{
	bool    bTransparent = bSelfTransparent;
	Object* pObjFirstParentToDrawBk = NULL;
	
	if (bTransparent)
	{
		// 检查祖父对象中有没有不透明的，从该父对象画起
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
		pObjFirstParentToDrawBk = this;   // 虽然不需要绘制，但还是需要依据父对象进行偏移
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
			if (pObjParent != pObjFirstParentToDrawBk)  // 从pObjFirstParentToDrawBk开始绘制
				continue;

			pObjFirstParentToDrawBk = NULL;             // 清除标记
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
// 获取该对象的偏移量
//
// ReturnQ
//		返回false表示该对象无滚动数据
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
//	设置Object的默认鼠标样式
//
//	Remark:
//		如果不实现该消息，可能导致鼠标移动到一个Object上面时，仍然显示的是Window的OnSetCursor
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
//	获取自己的字体,这里返回的对象只是一个临时对象，
//	如果需要保存使用，则需要调用UI_CopyFont/ui_releasefont
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
// 如果发现自己还没有字体，则向父对象索取
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

	// 由于使用通过UISendMessage效率没有直接GetFont高，因此放弃发送消息
	return GetFont();
}



//
//	[public] [virtual]  UINT HitTest( POINT* pt )
//
//	Parameters
//		pt
//			[in]  要试探的位置，通常为鼠标当前位置
//
//	Return
//		鼠标不在对象所有警围内返回HTNOWHERE，否则返回HTCLIENT，或其它（子类可扩展）
//
//	Remark
//		这个函数是用来解决一个问题，是为Object配备一个RECT对象表示范围，还是给一个更强大的RGN表示范围?
//		由于RGN更加消耗内存，而有时候就得使用RGN来实现某些功能，因此在这里用这个函数来告诉调用者，
//		你这个位置处于我什么部位。同时给Object配备一个RECT，来表示每一个对象的范围，因为大部分时候使用的都是RECT区域。
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
**			[in] 要测试的比特位，如OSB_VISIBLE、OSB_DISABLE、CSB_TABABLE、CSB_DEFAULT
**				 CSB_FOCUS、CSB_READONLY、CSB_HOVER、CSB_PRESS
**	
**	return
**		该位为1，返回true，该位为0，返回false
**
**	remark
**		为了节省内存占用，将一个控件的多种状态集中为bit位进行处理
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
	// 判断父对象不可见的情况 
	else
	{
		if( OBJ_WINDOW == m_pParent->GetObjectType() )
		{ 
			return true;           // 注：在这里不对最外层的窗口进行判断的原因是：在类型于窗口初始化的函数里面，虽然窗口暂时是不可见的，但里面的对象的IsVisible应该是返回true才好处理
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
//	备注：分层窗口中，隐藏一个对象时，不能直接调用::InvalidateRect(&rc..)
//        必须调用 GetWindowObject(); pWindow->InvalidateObject(GetWindowObject(),...);
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
		this->UpdateLayout(bUpdateNow);	   // 如果对象可见的话，则UpdateLayout会负责更新
		if(bUpdateNow )
		{
			if (b)
				this->UpdateObject(bUpdateNow); 
			else
				this->UpdateObjectBkgnd(bUpdateNow); 
		}
	}

	// 如果隐藏的对象是一个焦点对象，则将焦点重新切回到第一个对象
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

// 注：这里的clientrect 并不是0,0起点的，而是相对于Object左上角而言
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

// 注：这里的clientrect 是0,0起点的
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

// 	if( bMove || bSize )  // TODO: 恢复，重写
// 	{
// 		// 刷新移动前的区域位置
// 		if( !(nFlag & SWP_NOREDRAW) )
// 		{
// 			this->UpdateObject(&m_rcWindow, false);
// 		}
// 	}

	if( this->GetObjectType() == OBJ_WINDOW )
	{
		// 对于窗口来说，这里设置的是非客户区的大小
		WindowBase* pThis = (WindowBase*)this;
		::SetWindowPos(pThis->m_hWnd, NULL, x, y, cx, cy, SWP_NOZORDER|SWP_NOACTIVATE);
		::GetClientRect(pThis->m_hWnd, &m_rcParent);
	}
	else
	{
		::SetRect(&m_rcParent, x,y,x+cx,y+cy);
	}

// 	if( bMove || bSize )  // TODO: 重写，恢复
// 	{
// 		// 刷新移动后的区域位置
// 		if( !(nFlag & SWP_NOREDRAW) )
// 		{
// 			this->UpdateObject(&m_rcWindow, true);
// 		}
// 	}
	// MSDN: MoveWindow sends the WM_WINDOWPOSCHANGING, WM_WINDOWPOSCHANGED, WM_MOVE, WM_SIZE, and WM_NCCALCSIZE messages to the window. 
	// 在这里我们暂时只先发送WM_MOVE/WM_SIZE消息
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
//                               消息映射                               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


//
//   窗口收到WM_DESTROY消息之后，传递给所有的子对象
//
//void Object::OnDestroy()
//{
//	Object*  pObj = NULL;
//	while( pObj = EnumChildObject(pObj) )
//	{
//		// 向子对象发送WM_DESTROY消息
//		UIMsg  cloneMsg = *(this->m_pCurMsg);
//
//		cloneMsg.pObjMsgTo = pObj;
//		::UISendMessage( &cloneMsg );
//	}
//}


//void Object::OnPaint( HDC hDC )


// 在接收到OnSize消息的时候，需要改变自己的pos,rectP,rectW成员，
// 并通知自己的子对象去更新
//void Object::OnSize( UINT nType, int cx, int cy )
//{
//	this->UpdateRect();
//
//	Object*  pObj = NULL;
//	while( pObj = EnumChildObject(pObj) )
//	{
//		// 向子对象发送WM_ERASEBKGND消息
//		UIMsg  cloneMsg = *(this->m_pCurMsg);
//		cloneMsg.pObjMsgTo = pObj;
//		::UISendMessage( &cloneMsg );
//	}
//}

