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
//	�޸�Ƥ�����õ�ǰ���д������¼���
//
void TopWindowManager::ChangeSkin(SkinManager* pNewSkinMgr)
{
	if( NULL == pNewSkinMgr )
		return;

	list<WindowBase*>::iterator  iter = m_lTopWindowObject.begin();
	list<WindowBase*>::iterator  iterEnd = m_lTopWindowObject.end();
	for( ; iter!=iterEnd; iter++ )
	{
		WindowBase* pWindow = *iter;

		//////////////////////////////////////////////////////////////////////////
		//
		// 1. �����е���Ԫ�����У����ں������²���

		list<Object*>  listAllChildObjects;
		this->GetAllChildIntoList(*iter, listAllChildObjects);

		// ������ж������֯��ϵ
		list<Object*>::iterator iter = listAllChildObjects.begin();
		list<Object*>::iterator iterEnd = listAllChildObjects.end();
		for ( ; iter != iterEnd; iter++ )
		{
			(*iter)->ClearTreeObject();
		}
		pWindow->ClearTreeObject();

		pNewSkinMgr->ReLoadLayout(pWindow, listAllChildObjects);

		// ��ʣ��û���ٱ�ʹ�õ�object�ͷŵ�
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
	// ˢ�´��ڣ����ں���ͳһˢ�£���ΪChangeSkin�����Ƚ������ᵼ�¸�������֮����ʱ���
	//
	// ע�����ڷֲ㴰�ڣ����SetWindowPosǰ��SIZEδ��Ļ�����ֲ㴰���ղ���WM_SIZE��Ϣ��
	//	   ��Ҳ�����·ֲ㴰�ڵò���ˢ�£������������Ҫ��Ը������������
	//	   TODO: ��û��ֱ�����Ӵ�С��ͬҲ����WM_SIZE�ķ�����
	//
	for( iter = m_lTopWindowObject.begin(); iter!=iterEnd; iter++ )
	{
		WindowBase* pWindow = *iter;

		int nOldWidth = pWindow->GetWidth();
		int nOldHeight = pWindow->GetHeight();

		::UI_UpdateTopWindowLayout(pWindow);
		::SetWindowPos( pWindow->m_hWnd, NULL, pWindow->GetParentRectL(), pWindow->GetParentRectT(), 
			pWindow->GetWidth(), pWindow->GetHeight(), SWP_NOZORDER|SWP_NOACTIVATE );  // ������������Դ���Ѿ������ˣ�Ӧ�÷�ֹ������ˢ��

		if( nOldWidth == pWindow->GetWidth() && nOldHeight == pWindow->GetHeight() )
		{
			if( WS_EX_LAYERED & ::GetWindowLong(pWindow->m_hWnd,GWL_EXSTYLE) )
			{
				pWindow->UpdateObject();
			}
		}
	}
}

//
//	�ı䵱ǰƤ����ɫ��
//
bool TopWindowManager::InvalidateWindow(  )
{
	list<WindowBase*>::iterator  iter = m_lTopWindowObject.begin();
	list<WindowBase*>::iterator  iterEnd = m_lTopWindowObject.end();

	// ˢ�´���
	for( iter = m_lTopWindowObject.begin(); iter!=iterEnd; iter++ )
	{
		WindowBase* pWindow = *iter;
		pWindow->InvalidateObject(pWindow,NULL,true);
	}
	return true;
}

//
//	�����еĶ��㴰�ڷ�����Ϣ
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
//	��һ��������Ӷ����ɢ��ȫ����һ��list���������²��֣�����һ��OBJ_CONTROL�������ٲ�֣�����child�ͱ�������һ������
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