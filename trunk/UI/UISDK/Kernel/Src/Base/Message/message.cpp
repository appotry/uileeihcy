#include "stdafx.h"
#include "message.h"
#include "UISDK\Kernel\Src\Base\Message\message.h"
#include "UISDK\Kernel\Src\Base\Object\object.h"

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
//  注：对于系统的SendMessage，它的nMsg有范围限制的：
//     if(Msg&0xFFFE0000)// Msgs (>0xFFFF) are reserved by the system   
//     {  
//         User32._UserSetLaseError(0x57); //0x57 The parameter is incorrect.    
//         return 0;  
//     }  
long UISendMessage(UIMSG* pMsg, int nMsgMapID, BOOL* pbHandled)
{
	UIASSERT (pMsg != NULL);
	if (NULL == pMsg)
	{
		UI_LOG_ERROR(_T("UISendMessage, pMsg==NULL! ") );
		return -1;
	}
	
	UIASSERT(pMsg->pMsgTo != NULL);
//	UIASSERT(pMsg->message != 0);   // 有可能是WM_NULL

	if (NULL == pMsg->pMsgTo)
	{
		UI_LOG_ERROR(_T("UISendMessage, pMessageTo==NULL!  message=%d"), pMsg->message);
		return -1;
	}

	BOOL bRet = pMsg->pMsgTo->ProcessMessage(pMsg, nMsgMapID, true);
	if (pbHandled) 
	{
		*pbHandled = bRet;
	}

	return pMsg->lRet;
}

long  UISendMessage(IMessage* pMsgTo,  UINT message, 
					WPARAM wParam, LPARAM lParam, 
					UINT nCode, IMessage* pMsgFrom,
					int nMsgMapID,BOOL* pbHandled)
{
	if (NULL == pMsgTo)
	{
		UI_LOG_FATAL(_T("%s pObjMsgTo == NULL"), FUNC_NAME);
		return 0;
	}

    UIMSG msg;
	msg.pMsgFrom = pMsgFrom;
	msg.pMsgTo   = pMsgTo;

	msg.message = message;
	msg.nCode   = nCode;
	msg.wParam  = wParam;
	msg.lParam  = lParam;

	return ::UISendMessage(&msg, nMsgMapID, pbHandled);
}


// 由于pMsg.pObjTo是一个Message类型，无法获取pUIApp指针，只能再增加一个参数
long  UIPostMessage(IUIApplication* pUIApp, UIMSG* pMsg, int nMsgMapID)
{
    if (NULL == pUIApp)
        return 0;

    HWND hDestWnd = pUIApp->GetForwardPostMessageWnd();
    if (NULL == hDestWnd)
        return 0;

    return UIPostMessage(hDestWnd, pMsg, nMsgMapID);

}
long  UIPostMessage(HWND hForwardMsgWnd, UIMSG* pMsg, int nMsgMapID)
{
    UIMSG* pCloneMsg = new UIMSG;
    memcpy(pCloneMsg, pMsg, sizeof(UIMSG));
    ::PostMessage(hForwardMsgWnd, UI_WM_POSTMESSAGE, (WPARAM)pCloneMsg, (LPARAM)nMsgMapID);

    return 0;
}

long UISendMessage(Message* pMsgTo, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (NULL == pMsgTo)
        return 0;

    return UISendMessage(pMsgTo->GetIMessage(), message, wParam, lParam, 0, NULL, 0, NULL);
}
}


Message::Message()
{
 //   m_pOutMessageInstance = NULL;
	m_pCurMsg = NULL;
    m_pIMessage = NULL;
    m_bCreateIMessage = FALSE;
}
Message::~Message()
{
	m_pCurMsg = NULL;

	this->ClearNotify();
	this->ClearHook();

    if (m_bCreateIMessage)
    {
        SAFE_DELETE(m_pIMessage);
    }
}

// 当自己没有从IMessage创建时，为了支持IMessage，自己创建一个。
IMessage*  Message::GetIMessage()   
{ 
    if (NULL == m_pIMessage)
    {
        m_pIMessage = new IMessageInnerProxy;
        m_pIMessage->SetMessageImpl(this);
        m_bCreateIMessage = TRUE;
    }
    return m_pIMessage; 
}


// 获取当前消息是否已被处理过了
// remark
//	  该函数只能在ProcessMessage范围内调用，因为m_pCurMsg只在这段范围内有效
BOOL Message::IsMsgHandled() const                         
{                  
	UIASSERT(NULL != this->m_pCurMsg);
	return this->m_pCurMsg->bHandled;                         
}   

// 设置当前消息已被被处理过
void Message::SetMsgHandled(BOOL bHandled){	UIASSERT(NULL != this->m_pCurMsg);	this->m_pCurMsg->bHandled = bHandled; }


#if 0
void Message::AddNotify( Message* pObj, int nMsgMapID )
{
	list< MsgNotify* >::iterator  iter    = m_lNotifyMsgMap.begin();
	list< MsgNotify* >::iterator  iterEnd = m_lNotifyMsgMap.end();
	for (; iter!=iterEnd; iter++)
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


// 发送Notify消息到注册过Notify的对象
void Message::DoNotify(UIMSG* pMsg, bool bPost)
{
	if (pMsg->pObjMsgTo)
	{
		UISendMessage(pMsg);
	}

	list< MsgNotify* >::iterator  iter    = m_lNotifyMsgMap.begin();
	list< MsgNotify* >::iterator  iterEnd = m_lNotifyMsgMap.end();
	for (; iter!=iterEnd; iter++)
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
#endif

void Message::SetNotify(IMessage* pObj, int nMsgMapID)
{
	m_pObjNotify.pObj = pObj;
	m_pObjNotify.nMsgMapIDToNotify = nMsgMapID;
}

void Message::ClearNotify()
{
	m_pObjNotify.pObj = NULL;
	m_pObjNotify.nMsgMapIDToNotify = 0;
}

//
//  e.g. 菜单子项在弹出时，需要拷贝一次父窗口的notify对象
//
void Message::CopyNotifyTo(IMessage* pObjCopyTo)
{
	if (NULL == pObjCopyTo)
		return;

	pObjCopyTo->SetNotify(m_pObjNotify.pObj, m_pObjNotify.nMsgMapIDToNotify);
}

// 发送Notify消息到注册过Notify的对象.(修改为只通知给一个人.通知多个人太乱了)
// 当bPost为true时，才需要pUIApp参数
long Message::DoNotify(UIMSG* pMsg, bool bPost, IUIApplication* pUIApp)
{
	IMessage* pNotifyObj = pMsg->pMsgTo;
	int    nMsgMapID = 0;
	long   lRet = 0;     // 如果在UIMSG中指定了pObjMsgTo，则优先发送给该对象，并且nMsgMapID = 0;

	if (NULL == pMsg->pMsgTo)
	{
		pMsg->pMsgTo = m_pObjNotify.pObj/*->GetIMessage()*/;
		nMsgMapID = m_pObjNotify.nMsgMapIDToNotify;
	}
	if (NULL == pMsg->pMsgTo)
		return 0;

	if (bPost && NULL != pUIApp)
	{
		UIPostMessage(pUIApp, pMsg, nMsgMapID);
		lRet = 0;
	}
	else
	{
		lRet = UISendMessage(pMsg, nMsgMapID);
	}
	return lRet;
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

	if (0 == m_lHookMsgMap.size())
		return bRet;

	list<MsgHook*>::iterator  iter = m_lHookMsgMap.begin();
	for (; iter!=m_lHookMsgMap.end(); )
	{
		MsgHook* pHook = *iter;
		iter++;  // 将++放在这里，是为了解决在ProccessMessage中调用RemoveHook->erase(iter)的崩溃问题

		if (nMsgMapID == pHook->nMsgMapIDToHook)
		{
			if (TRUE == pHook->pObj->ProcessMessage(pMsg, (int)pHook->nMsgMapIDToNotify))
				return TRUE;
		}
	}

	return FALSE;
}

void Message::AddHook(IMessage* pObj, int nMsgMapIDToHook, int nMsgMapIDToNotify )
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

void Message::RemoveHook(IMessage* pObj, int nMsgMapIDToHook, int nMsgMapIDToNotify )
{
	list< MsgHook* >::iterator  iter    = m_lHookMsgMap.begin();
	list< MsgHook* >::iterator  iterEnd = m_lHookMsgMap.end();
	for (; iter!=iterEnd; iter++)
	{
		MsgHook* pHook = *iter;

		if (pObj == pHook->pObj &&
			nMsgMapIDToHook == pHook->nMsgMapIDToHook &&
			nMsgMapIDToNotify == pHook->nMsgMapIDToNotify)
		{
			
			delete    pHook;
			this->m_lHookMsgMap.erase( iter );

			return;
		}
	}
}
void Message::RemoveHook(IMessage* pObj)
{
	list< MsgHook* >::iterator  iter    = m_lHookMsgMap.begin();
	list< MsgHook* >::iterator  iterEnd = m_lHookMsgMap.end();
	for (; iter!=iterEnd;)
	{
		MsgHook*  pHook = *iter;
		if (pObj == pHook->pObj)
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
	for (; iter!=iterEnd; iter++)
	{
		MsgHook*  pHook = *iter;
		delete    pHook;
	}
	this->m_lHookMsgMap.clear();
}


//
//	static 给pParent的子子孙孙对象递归转发消息
//
 void Message::ForwardMessageToChildObject(Object* pParent, UIMSG* pMsg)
 {
 	Object*  pChild = NULL;
 	while (pChild = pParent->EnumChildObject(pChild))
 	{
        pMsg->pMsgTo = pChild->GetIMessage();
 		::UISendMessage(pMsg);
 		Message::ForwardMessageToChildObject(pChild, pMsg);
 	}
 
 	Object* pNcChild = NULL;
 	while (pNcChild = pParent->EnumNcChildObject(pNcChild))
 	{
        pMsg->pMsgTo = pNcChild->GetIMessage();
        ::UISendMessage(pMsg);
 		Message::ForwardMessageToChildObject(pNcChild, pMsg);
 	}
 }
