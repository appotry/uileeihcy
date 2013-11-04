#include "stdafx.h"
#include "UISDK\Kernel\Inc\Interface\imessage.h"
#include "UISDK\Kernel\Src\Base\Message\message.h"

namespace UI
{

IMessage::IMessage()
{
    m_pMessageImpl = NULL;
    m_bCreateMessageImpl = FALSE;
}

IMessage::~IMessage()
{
    if (m_bCreateMessageImpl)
        SAFE_DELETE(m_pMessageImpl);
}

void  IMessage::CreateImpl(Message* p)
{
    if (p)
    {
        m_pMessageImpl = p;
    }
    else
    {
        m_pMessageImpl = new Message;
        m_bCreateMessageImpl = TRUE;
    }
    m_pMessageImpl->SetIMessage(this);
}

BOOL  IMessage::ProcessMessage(UIMSG* pMsg, int nMsgMapID, bool bDoHook) 
{
    return virtualProcessMessage(pMsg, nMsgMapID, bDoHook); 
}
void  IMessage::delete_this()
{
    this->do_delete_this();
}

BOOL  IMessage::virtualProcessMessage(UIMSG* pMsg, int nMsgMapID, bool bDoHook)
{
    return FALSE;
}

BOOL  IMessage::IsMsgHandled()const { return m_pMessageImpl->IsMsgHandled(); }
void  IMessage::SetMsgHandled(BOOL b) { m_pMessageImpl->SetMsgHandled(b); }
UIMSG*  IMessage::GetCurMsg() { return m_pMessageImpl->GetCurMsg(); }
void  IMessage::SetCurMsg(UIMSG* p) { m_pMessageImpl->SetCurMsg(p); }
BOOL  IMessage::DoHook(UIMSG* pMsg, int nMsgMapID) { return m_pMessageImpl->DoHook(pMsg, nMsgMapID); }

void  IMessage::ClearNotify()
{ 
    return m_pMessageImpl->ClearNotify(); 
}
void  IMessage::SetNotify(IMessage* pObj, int nMsgMapID) 
{ 
    return m_pMessageImpl->SetNotify(pObj, nMsgMapID); 
}
void  IMessage::CopyNotifyTo(IMessage* pObjCopyTo) 
{ 
    if (NULL == pObjCopyTo)
        return;

    return m_pMessageImpl->CopyNotifyTo(pObjCopyTo/*->GetMessageImpl()*/); 
}
long  IMessage::DoNotify(UIMSG* pMsg, bool bPost, IUIApplication* pUIApp) 
{
    return m_pMessageImpl->DoNotify(pMsg, bPost, pUIApp); 
}

IMessage*  IMessage::GetNotifyObj()
{
    return m_pMessageImpl->GetNotifyObj();
}

void  IMessage::AddHook(IMessage* pObj, int nMsgMapIDToHook, int nMsgMapIDToNotify ) 
{ 
    if (NULL == pObj)
        return;

    return m_pMessageImpl->AddHook(pObj/*->GetMessageImpl()*/, nMsgMapIDToHook, nMsgMapIDToNotify); 
}
void  IMessage::RemoveHook(IMessage* pObj, int nMsgMapIDToHook, int nMsgMapIDToNotify ) 
{ 
    if (NULL == pObj)
        return;

    return m_pMessageImpl->RemoveHook(pObj/*->GetMessageImpl()*/, nMsgMapIDToHook, nMsgMapIDToNotify); 
}
void  IMessage::RemoveHook(IMessage* pObj) 
{ 
    if (pObj)
        return;

    return m_pMessageImpl->RemoveHook(pObj/*->GetMessageImpl()*/); 
}
void  IMessage::ClearHook() 
{ 
    return m_pMessageImpl->ClearHook(); 
}

void*  IMessage::QueryInterface(REFIID iid)
{
    return (void*)UISendMessage(this, UI_WM_QUERYINTERFACE, (WPARAM)&iid);
}
}