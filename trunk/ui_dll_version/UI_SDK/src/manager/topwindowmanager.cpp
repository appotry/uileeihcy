#include "stdafx.h"

void TopWindowManager::AddTopWindowObject( WindowBase* pObj )
{
	list<WindowBase*>::iterator  iter = m_lTopWindowObject.begin();
	list<WindowBase*>::iterator  iterEnd = m_lTopWindowObject.end();
	for( ; iter!=iterEnd; iter++ )
	{
		if( pObj == *iter )
			return;
	}

	this->m_lTopWindowObject.push_back( pObj );

	UI_LOG_DEBUG( _T("TopWindowManager::AddTopWindowObject, ID=%s"), pObj->m_strID.c_str() );
}

void TopWindowManager::RemoveTopWindowObject( WindowBase* pObj)
{
	list<WindowBase*>::iterator  iter = m_lTopWindowObject.begin();
	list<WindowBase*>::iterator  iterEnd = m_lTopWindowObject.end();
	for( ; iter!=iterEnd; iter++ )
	{
		if( pObj == *iter )
		{
			m_lTopWindowObject.erase( iter );

			UI_LOG_DEBUG( _T("TopWindowManager::RemoveTopWindowObject, ID=%s"), pObj->m_strID.c_str() );
			return;
		}
	}

}

void TopWindowManager::ClearTopWindowObject( )
{
	this->m_lTopWindowObject.clear();

	UI_LOG_DEBUG( _T("TopWindowManager::ClearTopWindowObject ") );
}

WindowBase* TopWindowManager::GetWindowBase(HWND hWnd)
{
	list<WindowBase*>::iterator  iter = m_lTopWindowObject.begin();
	list<WindowBase*>::iterator  iterEnd = m_lTopWindowObject.end();
	for( ; iter!=iterEnd; iter++ )
	{
		WindowBase* p = *iter;
		if( p->m_hWnd == hWnd )
			return p;
	}

	return NULL;
}

//
//	修改皮肤，让当前所有窗口重新加载
//
void TopWindowManager::ChangeSkin(SkinManager* pNewSkinMgr)
{
	if( NULL == pNewSkinMgr )
		return;

	vector<WindowBase*>  vecNotJoinChangeSkinWindows;  // 将不参与换肤的窗口放在该列表中，这样只需要发送一次UI_WM_SKINCHANGING消息

	list<WindowBase*>::iterator  iter = m_lTopWindowObject.begin();
	list<WindowBase*>::iterator  iterEnd = m_lTopWindowObject.end();
	for( ; iter!=iterEnd; iter++ )
	{
		WindowBase* pWindow = *iter;
		if (NULL == pWindow)
			continue;

		BOOL bChangeSkin = TRUE;
		UISendMessage(pWindow, UI_WM_SKINCHANGING, (WPARAM)&bChangeSkin, 0);
		if (FALSE == bChangeSkin)
		{
			vecNotJoinChangeSkinWindows.push_back(pWindow);
			continue;
		}

		//////////////////////////////////////////////////////////////////////////
		//
		// 1. 将所有的子元素重列，用于后面重新布局

		list<Object*>  listAllChildObjects;
		this->GetAllChildIntoList(*iter, listAllChildObjects);

		// 清除所有对象的组织关系
		list<Object*>::iterator iter = listAllChildObjects.begin();
		list<Object*>::iterator iterEnd = listAllChildObjects.end();
		for ( ; iter != iterEnd; iter++ )
		{
			(*iter)->ClearTreeObject();
		}
		pWindow->ClearTreeObject();

		pNewSkinMgr->ReLoadLayout(pWindow, listAllChildObjects);

		// 将剩余没有再被使用的object释放掉
		iter = listAllChildObjects.begin();
		iterEnd = listAllChildObjects.end();
		for ( ; iter != iterEnd; iter++ )
		{
			Object* pObj = *iter;
			delete pObj;
			pObj = NULL;
		}
		listAllChildObjects.clear();
	}
	
	//
	// 刷新窗口，放在后面统一刷新，因为ChangeSkin操作比较慢，会导致各个窗口之后有时间差
	//
	// 注：对于分层窗口，如果SetWindowPos前后SIZE未变的话，则分层窗口收不到WM_SIZE消息，
	//	   这也将导致分层窗口得不到刷新，因此在这里需要针对该情况单独处理。
	//	   TODO: 有没有直接无视大小相同也发送WM_SIZE的方法？
	//
	for( iter = m_lTopWindowObject.begin(); iter!=iterEnd; iter++ )
	{
		WindowBase* pWindow = *iter;
		if (NULL == pWindow)
			continue;

		// 该窗口不参与换肤
		int nSize = (int)vecNotJoinChangeSkinWindows.size();
		for (int i = 0; i < nSize; i++)
		{
			if (vecNotJoinChangeSkinWindows[i] == pWindow)
				continue;
		}

		int nOldWidth = pWindow->GetWidth();
		int nOldHeight = pWindow->GetHeight();

		::UI_UpdateTopWindowLayout(pWindow);
//		::SetWindowPos( pWindow->m_hWnd, NULL, pWindow->GetParentRectL(), pWindow->GetParentRectT(), 
//			pWindow->GetWidth(), pWindow->GetHeight(), SWP_NOZORDER|SWP_NOACTIVATE );  // 在这里由于资源都已经销毁了，应该防止窗口再刷新

		if( nOldWidth == pWindow->GetWidth() && nOldHeight == pWindow->GetHeight() )
		{
			if( WS_EX_LAYERED & ::GetWindowLong(pWindow->m_hWnd,GWL_EXSTYLE) )
			{
				pWindow->UpdateObject();
			}
		}

		// 通知
		UISendMessage(pWindow, UI_WM_SKINCHANGED, 0, 0);
	}
}

//
//	改变当前皮肤的色调
//
bool TopWindowManager::InvalidateWindow(  )
{
	list<WindowBase*>::iterator  iter = m_lTopWindowObject.begin();
	list<WindowBase*>::iterator  iterEnd = m_lTopWindowObject.end();

	// 刷新窗口
	for( iter = m_lTopWindowObject.begin(); iter!=iterEnd; iter++ )
	{
		WindowBase* pWindow = *iter;
		pWindow->RedrawObject(pWindow,NULL,true);
	}
	return true;
}

//
//	向所有的顶层窗口发送消息
//
void TopWindowManager::SendMessage( UIMSG* pMsg )
{
	list<WindowBase*>::iterator  iter = m_lTopWindowObject.begin();
	list<WindowBase*>::iterator  iterEnd = m_lTopWindowObject.end();
	for( ; iter!=iterEnd; iter++ )
	{
		 Object* pObj = *iter;
		 pMsg->pObjMsgTo = pObj;

		 ::UISendMessage( pMsg );
	}
}

//
//	将一个对象的子对象打散，全放在一个list中用于重新布局，对于一个OBJ_CONTROL类型则不再拆分，它的child和本身属性一个整体
//
void TopWindowManager::GetAllChildIntoList( Object* pParent, list<Object*>& listObjects )
{
	if( NULL == pParent || OBJ_CONTROL == pParent->GetObjectType() )
		return;

	Object*  pChild = NULL;
	while( pChild = pParent->EnumChildObject(pChild) )
	{
		listObjects.push_back(pChild);

		if( pChild->GetObjectType() != OBJ_CONTROL )
			this->GetAllChildIntoList(pChild, listObjects);
	}
}

void TopWindowManager::OnSkinHLSChanged()
{
	list<WindowBase*>::iterator  iter = m_lTopWindowObject.begin();
	list<WindowBase*>::iterator  iterEnd = m_lTopWindowObject.end();

	// 刷新窗口
	for( iter = m_lTopWindowObject.begin(); iter!=iterEnd; iter++ )
	{
		WindowBase* pWindow = *iter;
		if (NULL == pWindow)
			continue;

		UISendMessage(pWindow, UI_WM_SKINHLSCHANGED);
		pWindow->UpdateObject(); 
	}
	return;
}