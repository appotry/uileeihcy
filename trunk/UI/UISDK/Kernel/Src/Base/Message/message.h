#ifndef MESSAGE_H_F5C9E8CD_E8E2_4338_8F61_341CE9E0395A
#define MESSAGE_H_F5C9E8CD_E8E2_4338_8F61_341CE9E0395A

#include "UISDK\Kernel\Inc\Interface\imessage.h"
#include "UISDK\Kernel\Inc\Interface\iobject.h"

namespace UI
{

class Message;
interface IUIApplication;

//
// �����������������ҵ���Ϣ
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

	IMessage* pObj;                 // ��¼˭Ҫ�����ҵ���Ϣ
	int      nMsgMapIDToHook;      // pObjҪHOOK��map id����Ϣ
	int      nMsgMapIDToNotify;    // HOOK������Ϣ��pObj�Լ�ʹ�ø�map idȥ��Ӧ
};
//
//	����notify messsage������Ҫ֪ͨʲô����
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
	int       nMsgMapIDToNotify;    // ������Ϣ֪ͨpObjʱ��pObjʹ�ø�idȥ��Ӧ
};

// ��Ϣ����,object�Ӹ���̳дӶ�ӵ������Ϣ����
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

	// 2012.12.23 ���ڶ��notify�޷�������ֵ�����⣬�����Ȼ�޸�Ϊ��һnotify
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
	 
	// ����TRUE����ʾ����Ϣ�ѱ�����FALSE��ʾ����Ϣû������
    virtual BOOL innerVirtualProcessMessage(UIMSG* pMsg, int nMsgMapID=0, bool bDoHook=false){ return FALSE; }

    BOOL  DoHook(UIMSG* pMsg, int nMsgMapID);
    long  DoNotify(UIMSG* pMsg, bool bPost=false, IUIApplication* pUIApp=NULL);

    static  void ForwardMessageToChildObject(Object* pParent, UIMSG* pMsg);
protected:
    list< MsgHook* >     m_lHookMsgMap;  // ����ComboBoxҪhook Combobox�е�������ť���¼�
//	list< MsgNotify* >   m_lNotifyMsgMap;// ���簴ť�ĵ���¼���Ҫ֪ͨ����  
    MsgNotify   m_pObjNotify;   // �����¼�ʱ����Ҫ֪ͨ�Ķ���

    UIMSG *     m_pCurMsg;     // ��¼��ǰ���ڴ������Ϣ
    IMessage*   m_pIMessage;
    BOOL        m_bCreateIMessage;
};

// ��������һ������ֱ�Ӵ�Message��������Ixxx�ӿڵ������
// ��Ϊ����Ϣ����ͨ��IMessage�ӿڵģ����Message�ᴴ��һ��IMessageInnerProxy����м���
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