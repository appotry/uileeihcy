#ifndef MESSAGE_H_F5C9E8CD_E8E2_4338_8F61_341CE9E0395A
#define MESSAGE_H_F5C9E8CD_E8E2_4338_8F61_341CE9E0395A

#include "UISDK\Kernel\Inc\Interface\imessage.h"
#include "UISDK\Kernel\Inc\Interface\iobject.h"

namespace UI
{

class Message;
interface IUIApplication;

//
// 用于其它对象拦截我的消息
//
class MsgHook
{
public:
	MsgHook()
	{
		pObj = NULL;
		nMsgMapIDToHook = 0;
		nMsgMapIDToNotify = 0;
	}

	IMessage* pObj;                 // 记录谁要拦截我的消息
	int      nMsgMapIDToHook;      // pObj要HOOK该map id的消息
	int      nMsgMapIDToNotify;    // HOOK到的消息，pObj自己使用该map id去响应
};
//
//	我有notify messsage，都需要通知什么人呢
//
class MsgNotify
{
public:
	MsgNotify()
	{
		pObj = NULL;
		nMsgMapIDToNotify = 0;
	}

	IMessage*  pObj;
	int       nMsgMapIDToNotify;    // 当有消息通知pObj时，pObj使用该id去响应
};

// 消息基类,object从该类继承从而拥有了消息功能
class Message
{
public:
	Message();
	virtual ~Message();

    void  SetIMessage(IMessage* p){ m_pIMessage = p; }
    IMessage*  GetIMessage();

	BOOL  IsMsgHandled()const;
	void  SetMsgHandled(BOOL);
    UIMSG*  GetCurMsg() { return m_pCurMsg; }
    void  SetCurMsg(UIMSG* p) { m_pCurMsg = p; }

	// 2012.12.23 由于多个notify无法处理返回值的问题，因此仍然修改为单一notify
// 	void AddNotify( Message* pObj, int nMsgMapID );
// 	void RemoveNotify( Message* pObj, int nMsgMapID );
// 	void RemoveNotify( Message* pObj );
	void  ClearNotify();
	void  SetNotify(IMessage* pObj, int nMsgMapID=0);
	void  CopyNotifyTo(IMessage* pObjCopyTo);
    IMessage*  GetNotifyObj() { return m_pObjNotify.pObj; }

	void  AddHook(IMessage* pObj, int nMsgMapIDToHook, int nMsgMapIDToNotify );
	void  RemoveHook(IMessage* pObj, int nMsgMapIDToHook, int nMsgMapIDToNotify );
	void  RemoveHook(IMessage* pObj );
	void  ClearHook();
	 
	// 返回TRUE，表示该消息已被处理，FALSE表示该消息没被处理
    virtual BOOL innerVirtualProcessMessage(UIMSG* pMsg, int nMsgMapID=0, bool bDoHook=false){ return FALSE; }

    BOOL  DoHook(UIMSG* pMsg, int nMsgMapID);
    long  DoNotify(UIMSG* pMsg, bool bPost=false, IUIApplication* pUIApp=NULL);

    static  void ForwardMessageToChildObject(Object* pParent, UIMSG* pMsg);
protected:
    list< MsgHook* >     m_lHookMsgMap;  // 例如ComboBox要hook Combobox中的下拉按钮的事件
//	list< MsgNotify* >   m_lNotifyMsgMap;// 例如按钮的点击事件需要通知窗口  
    MsgNotify   m_pObjNotify;   // 产生事件时，需要通知的对象

    UIMSG *     m_pCurMsg;     // 记录当前正在处理的消息
    IMessage*   m_pIMessage;
    BOOL        m_bCreateIMessage;
};

// 该类用于一个对象直接从Message派生，无Ixxx接口的情况。
// 因为发消息都是通过IMessage接口的，因此Message会创建一个IMessageInnerProxy类进行兼容
class IMessageInnerProxy : public IMessage
{
public:
    virtual BOOL  virtualProcessMessage(UIMSG* pMsg, int nMsgMapID, bool bDoHook)
    {
        return m_pMessageImpl->innerVirtualProcessMessage(pMsg, nMsgMapID, bDoHook);
    }
    virtual void  do_delete_this()
    {
        delete m_pMessageImpl;
        delete this;
    }

};

}// namespace

#endif // MESSAGE_H_F5C9E8CD_E8E2_4338_8F61_341CE9E0395A