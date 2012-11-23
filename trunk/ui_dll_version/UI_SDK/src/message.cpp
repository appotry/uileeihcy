#include "stdafx.h"

namespace UI
{

//
//	return
//		利用pMsg->lRet作为返回值。
//			事件响应函数的结果返回值作为pMsg->lRet;
//			如果事件响应函数没有返回值，pMsg->lRet默认为0
//	remark
//		想要知道这个消息有没有被处理，可调用IsCurMsgHandled()
//
long UISendMessage(UIMSG* pMsg, int nMsgMapID, BOOL* pbHandled)
{
	UIASSERT( pMsg != NULL );
	if( NULL == pMsg )
	{
		UI_LOG_ERROR(_T("UISendMessage, pMsg==NULL! ") );
		return -1;
	}
	
	UIASSERT(pMsg->pObjMsgTo != NULL);
	UIASSERT(pMsg->message != 0);

	Message* pMessageTo = (Message*)(pMsg->pObjMsgTo);
	if (NULL == pMessageTo)
	{
		UI_LOG_ERROR(_T("UISendMessage, pMessageTo==NULL!  message=%d"), pMsg->message );
		return -1;
	}
	BOOL bRet = pMessageTo->ProcessMessage(pMsg, nMsgMapID);
	if (NULL != pbHandled) 
	{
		*pbHandled = bRet;
	}

	return pMsg->lRet;
}

long  UIPostMessage( UIMSG* pMsg, int nMsgMapID )
{
	HWND hDestWnd = UI_GetForwardPostMessageWnd();
	if (NULL == hDestWnd)
		return -1;

	UIMSG* pCloneMsg = new UIMSG;
	memcpy(pCloneMsg, pMsg, sizeof(UIMSG));
	::PostMessage(hDestWnd, UI_WM_POSTMESSAGE, (WPARAM)pCloneMsg, (LPARAM)nMsgMapID);

	return 0;
}

long UISendMessage( Message* pObjMsgTo,  UINT message, 
					WPARAM   wParam,     LPARAM lParam, 
					UINT     code,       Message* pObjMsgFrom,
					int      nMsgMapID,  BOOL* pbHandled )
{
	assert( pObjMsgTo != NULL );

	UIMSG   msg;
	msg.pObjMsgFrom   = pObjMsgFrom;
	msg.pObjMsgTo     = pObjMsgTo;

	msg.message       = message;
	msg.code          = code;
	msg.wParam        = wParam;
	msg.lParam        = lParam;

	return ::UISendMessage(&msg, nMsgMapID, pbHandled);
}

}


Message::Message()
{
	m_pCurMsg = NULL;
}
Message::~Message()
{
	m_pCurMsg = NULL;

	this->ClearNotify();
	this->ClearHook();
}

// 获取当前消息是否已被处理过了
// remark
//	  该函数只能在ProcessMessage范围内调用，因为m_pCurMsg只在这段范围内有效
BOOL Message::IsMsgHandled() const                         
{                  
	assert( NULL != this->m_pCurMsg );
	return this->m_pCurMsg->bHandled;                         
}   

// 设置当前消息已被被处理过
void Message::SetMsgHandled( BOOL bHandled )
{
	assert( NULL != this->m_pCurMsg );
	this->m_pCurMsg->bHandled = bHandled;
}



void Message::AddNotify( Message* pObj, int nMsgMapID )
{
	list< MsgNotify* >::iterator  iter    = m_lNotifyMsgMap.begin();
	list< MsgNotify* >::iterator  iterEnd = m_lNotifyMsgMap.end();
	for( ; iter!=iterEnd; iter++ )
	{
		MsgNotify*  pNotify = *iter;
		if( pObj == pNotify->pObj &&
			nMsgMapID == pNotify->nMsgMapIDToNotify )
			return;
	}

	MsgNotify*  pNotify = new MsgNotify;
	pNotify->pObj = pObj;
	pNotify->nMsgMapIDToNotify = nMsgMapID;

	this->m_lNotifyMsgMap.push_back( pNotify );
}

void Message::RemoveNotify( Message* pObj, int nMsgMapID )
{
	list< MsgNotify* >::iterator  iter    = m_lNotifyMsgMap.begin();
	list< MsgNotify* >::iterator  iterEnd = m_lNotifyMsgMap.end();
	for( ; iter!=iterEnd; iter++ )
	{
		MsgNotify*  pNotify = *iter;
		if( pObj == pNotify->pObj &&
			nMsgMapID == pNotify->nMsgMapIDToNotify )
		{
			// TODO 待验证
			//assert( 0 );

			delete      pNotify;
			this->m_lNotifyMsgMap.erase( iter );

			return;
		}
	}
}

void Message::RemoveNotify( Message* pObj )
{
	list< MsgNotify* >::iterator  iter    = m_lNotifyMsgMap.begin();
	list< MsgNotify* >::iterator  iterEnd = m_lNotifyMsgMap.end();
	for( ; iter!=iterEnd;  )
	{
		MsgNotify*  pNotify = *iter;
		if( pObj == pNotify->pObj )
		{
			// TODO 待验证
			//assert( 0 );

			delete      pNotify;
			iter = this->m_lNotifyMsgMap.erase( iter );
			continue;
		}
		iter++;
	}
}

void Message::ClearNotify( )
{
	list< MsgNotify* >::iterator  iter    = m_lNotifyMsgMap.begin();
	list< MsgNotify* >::iterator  iterEnd = m_lNotifyMsgMap.end();
	for( ; iter!=iterEnd; iter++)
	{
		MsgNotify*  pNotify = *iter;
		delete      pNotify;
	}
	this->m_lNotifyMsgMap.clear();
}

//
//  e.g. 菜单子项在弹出时，需要拷贝一次父窗口的notify对象
//
void Message::CopyNotify(Message* pObjCopyFrom)
{
	if (NULL == pObjCopyFrom)
		return;

	this->ClearNotify();

	list< MsgNotify* >::iterator  iter    = pObjCopyFrom->m_lNotifyMsgMap.begin();
	list< MsgNotify* >::iterator  iterEnd = pObjCopyFrom->m_lNotifyMsgMap.end();
	for( ; iter!=iterEnd; iter++ )
	{
		this->AddNotify((*iter)->pObj, (*iter)->nMsgMapIDToNotify);
	}
}

// 发送Notify消息到注册过Notify的对象
void Message::DoNotify( UIMSG* pMsg, bool bPost )
{
	if (NULL != pMsg->pObjMsgTo)
	{
		UISendMessage(pMsg);
	}

	list< MsgNotify* >::iterator  iter    = m_lNotifyMsgMap.begin();
	list< MsgNotify* >::iterator  iterEnd = m_lNotifyMsgMap.end();
	for( ; iter!=iterEnd; iter++ )
	{
		MsgNotify*  pNotify = *iter;
		if (pNotify->pObj != pMsg->pObjMsgTo)
		{
			if (bPost)
			{
				pMsg->pObjMsgTo = pNotify->pObj;
				UIPostMessage(pMsg, pNotify->nMsgMapIDToNotify);
			}
			else
			{
				pNotify->pObj->ProcessMessage( pMsg, pNotify->nMsgMapIDToNotify );
			}
		}
	}
}


// 
//	对消息进行HOOK处理
//
//	return
//		在所有的HOOK列表中，只要有一个HOOK处理结束后返回TRUE，则整个
//		HOOK过程结束。
//	remark
//		在HOOK过程中，不对 pMsg->pObjMsgTo作修改
//
BOOL Message::DoHook( UIMSG* pMsg, int nMsgMapID )
{
	BOOL  bRet = FALSE;

	if( 0 == m_lHookMsgMap.size() )
		return bRet;

	list< MsgHook* >::iterator  iter    = m_lHookMsgMap.begin();
	list< MsgHook* >::iterator  iterEnd = m_lHookMsgMap.end();
	for( ; iter!=iterEnd; iter++ )
	{
		MsgHook* pHook = *iter;
		if( nMsgMapID == pHook->nMsgMapIDToHook )
		{
			if( TRUE == pHook->pObj->ProcessMessage(pMsg, (int)pHook->nMsgMapIDToNotify ))
				return TRUE;
		}
	}

	return FALSE;
}

void Message::AddHook( Message* pObj, int nMsgMapIDToHook, int nMsgMapIDToNotify )
{
	list< MsgHook* >::iterator  iter    = m_lHookMsgMap.begin();
	list< MsgHook* >::iterator  iterEnd = m_lHookMsgMap.end();
	for( ; iter!=iterEnd; iter++ )
	{
		MsgHook* pHook = *iter;

		if( pObj == pHook->pObj &&
			nMsgMapIDToHook == pHook->nMsgMapIDToHook &&
			nMsgMapIDToNotify == pHook->nMsgMapIDToNotify )
			return;
	}

	MsgHook*  pHook = new MsgHook;
	pHook->pObj = pObj;
	pHook->nMsgMapIDToHook = nMsgMapIDToHook;
	pHook->nMsgMapIDToNotify = nMsgMapIDToNotify;

	this->m_lHookMsgMap.push_back( pHook );
}

void Message::RemoveHook( Message* pObj, int nMsgMapIDToHook, int nMsgMapIDToNotify )
{
	list< MsgHook* >::iterator  iter    = m_lHookMsgMap.begin();
	list< MsgHook* >::iterator  iterEnd = m_lHookMsgMap.end();
	for( ; iter!=iterEnd; iter++ )
	{
		MsgHook* pHook = *iter;

		if( pObj == pHook->pObj &&
			nMsgMapIDToHook == pHook->nMsgMapIDToHook &&
			nMsgMapIDToNotify == pHook->nMsgMapIDToNotify )
		{
			// TODO 待验证
			assert( 0 );

			delete    pHook;
			this->m_lHookMsgMap.erase( iter );

			return;
		}
	}
}
void Message::RemoveHook( Message* pObj )
{
	list< MsgHook* >::iterator  iter    = m_lHookMsgMap.begin();
	list< MsgHook* >::iterator  iterEnd = m_lHookMsgMap.end();
	for( ; iter!=iterEnd;  )
	{
		MsgHook*  pHook = *iter;
		if( pObj == pHook->pObj )
		{
			delete    pHook;
			iter = this->m_lHookMsgMap.erase( iter );
			continue;
		}
		iter++;
	}
}

void Message::ClearHook( )
{
	list< MsgHook* >::iterator  iter    = m_lHookMsgMap.begin();
	list< MsgHook* >::iterator  iterEnd = m_lHookMsgMap.end();
	for( ; iter!=iterEnd; iter++ )
	{
		MsgHook*  pHook = *iter;
		delete    pHook;
	}
	this->m_lHookMsgMap.clear();
}


//
//	static 给pParent的子子孙孙对象递归转发消息
//
void Message::ForwardMessageToChildObject( Object* pParent, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	Object*  pChild = NULL;
	while ( pChild = pParent->EnumChildObject(pChild) )
	{
		::UISendMessage( pChild, uMsg, wParam, lParam );
		Message::ForwardMessageToChildObject(pChild,uMsg,wParam,lParam);
	}
}