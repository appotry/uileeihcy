#include "stdafx.h"

namespace UI
{

//
//	return
//		����pMsg->lRet��Ϊ����ֵ��
//			�¼���Ӧ�����Ľ������ֵ��ΪpMsg->lRet;
//			����¼���Ӧ����û�з���ֵ��pMsg->lRetĬ��Ϊ0
//	remark
//		��Ҫ֪�������Ϣ��û�б������ɵ���IsCurMsgHandled()
//
long UISendMessage(UIMSG* pMsg, int nMsgMapID)
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
	pMessageTo->ProcessMessage(pMsg, nMsgMapID);

	return pMsg->lRet;
}

long UISendMessage( Message* pObjMsgTo,  UINT message, 
					WPARAM   wParam,     LPARAM lParam, 
					UINT     code,       Message* pObjMsgFrom,
					int      nMsgMapID )
{
	assert( pObjMsgTo != NULL );

	UIMSG   msg;
	msg.pObjMsgFrom   = pObjMsgFrom;
	msg.pObjMsgTo     = pObjMsgTo;

	msg.message       = message;
	msg.code          = code;
	msg.wParam        = wParam;
	msg.lParam        = lParam;

	return ::UISendMessage(&msg, nMsgMapID);
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

// ��ȡ��ǰ��Ϣ�Ƿ��ѱ��������
// remark
//	  �ú���ֻ����ProcessMessage��Χ�ڵ��ã���Ϊm_pCurMsgֻ����η�Χ����Ч
BOOL Message::IsMsgHandled() const                         
{                  
	assert( NULL != this->m_pCurMsg );
	return this->m_pCurMsg->bHandled;                         
}   

// ���õ�ǰ��Ϣ�ѱ��������
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
			// TODO ����֤
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
			// TODO ����֤
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
	for( ; iter!=iterEnd; )
	{
		// TODO ����֤
		//assert( 0 );

		MsgNotify*  pNotify = *iter;
		delete      pNotify;
		iter = this->m_lNotifyMsgMap.erase( iter );
	}
}

// ����Notify��Ϣ��ע���Notify�Ķ���
void Message::DoNotify( UIMSG* pMsg )
{
	list< MsgNotify* >::iterator  iter    = m_lNotifyMsgMap.begin();
	list< MsgNotify* >::iterator  iterEnd = m_lNotifyMsgMap.end();
	for( ; iter!=iterEnd; iter++ )
	{
		MsgNotify*  pNotify = *iter;
		pNotify->pObj->ProcessMessage( pMsg, pNotify->nMsgMapIDToNotify );
	}
}


// 
//	����Ϣ����HOOK����
//
//	return
//		�����е�HOOK�б��У�ֻҪ��һ��HOOK��������󷵻�TRUE��������
//		HOOK���̽�����
//	remark
//		��HOOK�����У����� pMsg->pObjMsgTo���޸�
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
			// TODO ����֤
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
			// TODO ����֤
			assert( 0 );

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
	for( ; iter!=iterEnd;  )
	{
		MsgHook*  pHook = *iter;
		delete    pHook;
		iter = this->m_lHookMsgMap.erase( iter );
	}
}