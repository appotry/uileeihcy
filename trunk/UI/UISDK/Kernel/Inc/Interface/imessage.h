#ifndef IMESSAGE_H_3180BE93_DA9E_4c55_A49A_C1C79BEBC947
#define IMESSAGE_H_3180BE93_DA9E_4c55_A49A_C1C79BEBC947

namespace UI
{

struct UIMSG;
interface IUIApplication;
interface  IMessage;

// 消息结构定义。系统控件仍然使用MSG，但UI控件使用UIMsg进行代码
struct UIMSG : public MSG
{
    UIMSG() { memset(this, 0, sizeof(UIMSG)); }

    IMessage*   pMsgFrom;    // 消息发送者
    IMessage*   pMsgTo;      // 消息接受者 
    UINT        nCode;       // 针对 WM_COMMAND,WM_NOTIFY
    long        lRet;        // 消息处理结束后的返回值
    BOOL        bHandled;    // 该消息是否已被处理过
};

class Message;
interface UISDKAPI IMessage
{
    IMessage();
    BOOL  ProcessMessage(UIMSG* pMsg, int nMsgMapID=0, bool bDoHook=false);
    void  delete_this();

protected:
    virtual ~IMessage();  // 虚函数1. 保证正确释放整个对象
private:
    virtual BOOL  DoProcessMessage(UIMSG* pMsg, int nMsgMapID, bool bDoHook);  // 虚函数2. 消息处理
    virtual void  do_delete_this(){ }  // 由UIObjCreator负责实现

public:
    void    CreateImpl(Message* p = NULL);
    Message* GetImpl() { return m_pMessageImpl; }
    void    SetMessageImpl(Message* p) { m_pMessageImpl = p; } // 用于UISDK内部直接继承，不需要Ixxx接口时使用，例如PopupControlWindow

    BOOL    IsMsgHandled()const;
    void    SetMsgHandled(BOOL b);

    UIMSG*  GetCurMsg();
    void    SetCurMsg(UIMSG* p);
    BOOL    DoHook(UIMSG* pMsg, int nMsgMapID);

    void    ClearNotify();
    void    SetNotify(IMessage* pObj, int nMsgMapID);
    void    CopyNotifyTo(IMessage* pObjCopyTo);
    long    DoNotify(UIMSG* pMsg, bool bPost=false, IUIApplication* pUIApp=NULL);
    IMessage*  GetNotifyObj();

    void    AddHook(IMessage* pObj, int nMsgMapIDToHook, int nMsgMapIDToNotify );
    void    RemoveHook(IMessage* pObj, int nMsgMapIDToHook, int nMsgMapIDToNotify );
    void    RemoveHook(IMessage* pObj );
    void    ClearHook();

    void*   QueryInterface(REFIID iid);

protected:
    Message*   m_pMessageImpl;
    BOOL       m_bCreateMessageImpl;
};

// 由外部的类继承，用于支持消息映射宏定义
class MessageProxy
{
public:
    MessageProxy() { m_pIMessageProxy = NULL; }
    virtual ~MessageProxy() {}

    void    SetIMessageProxy(IMessage* p) { m_pIMessageProxy = p; }

    BOOL    IsMsgHandled()const   { return m_pIMessageProxy->IsMsgHandled(); }
    void    SetMsgHandled(BOOL b) { return m_pIMessageProxy->SetMsgHandled(b); }
    UIMSG*  GetCurMsg()           { return m_pIMessageProxy->GetCurMsg(); }
    void    SetCurMsg(UIMSG* p)   { m_pIMessageProxy->SetCurMsg(p); }
    BOOL    DoHook(UIMSG* pMsg, int nMsgMapID) { return m_pIMessageProxy->DoHook(pMsg, nMsgMapID); }

protected:
    IMessage*  m_pIMessageProxy;
};

#define DO_PARENT_PROCESS(IMyInterface, IParentInterface) \
    static_cast<IParentInterface*>(m_p##IMyInterface)->xProcessMessage(GetCurMsg(), 0, 0)

}

#endif  IMESSAGE_H_3180BE93_DA9E_4c55_A49A_C1C79BEBC947