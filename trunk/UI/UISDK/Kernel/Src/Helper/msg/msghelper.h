#ifndef MSGHELPER_H_C82B8B2A_60AF_4321_A92F_E6A2D39DCB40
#define MSGHELPER_H_C82B8B2A_60AF_4321_A92F_E6A2D39DCB40

namespace UI
{
class UIApplication;
// ����һ��������Ϣ���ڣ�����ʵ��post ui message
class CForwardPostMessageWindow : public CWindowImpl<CForwardPostMessageWindow>
{
public:
	CForwardPostMessageWindow(UIApplication* pUIApp) { m_pUIApplication = pUIApp; }
	BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID = 0);

protected:
	UIApplication*  m_pUIApplication;
};

// ʵ����Ϣѭ���еȴ�HANDLE�Ĺ���
class WaitForHandle
{
public:
	WaitForHandle(HANDLE h, IWaitForHandleCallback* pCB, LPARAM l);
	HANDLE  GetHandle() { return m_hHandle; }
	IWaitForHandleCallback* GetCB() { return m_pCallback; }
	LPARAM  GetLParam() { return m_lParam; }

protected:
	HANDLE  m_hHandle;
	IWaitForHandleCallback*  m_pCallback;
	LPARAM  m_lParam;
};


class WaitForHandlesMgr : public IWaitForHandlesMgr
{
public:
	WaitForHandlesMgr();
	~WaitForHandlesMgr();

	typedef list<WaitForHandle*> _MyList;
	typedef list<WaitForHandle*>::iterator _MyIter;

	WaitForHandle* FindHandle(HANDLE h);
	_MyIter FindHandleIter(HANDLE h);
	bool  AddHandle(HANDLE h, IWaitForHandleCallback* pCB, LPARAM l);
	bool  RemoveHandle(HANDLE h);
	bool  UpdateHandleArray();
	void  Do(HANDLE h);

	HANDLE*  GetHandles() { return m_pHandles; }

protected:
	list<WaitForHandle*>  m_list;
public:
	HANDLE*  m_pHandles;
	DWORD    m_nHandleCount;  // ��ΪҪ��whileѭ���е��ã������Ϊpublic����ֱ�ӵ���
};

class MessageFilterMgr : public IMessageFilterMgr
{
public:
    ~MessageFilterMgr() {};

	virtual void  AddMessageFilter(IPreTranslateMessage* p);
	virtual void  RemoveMessageFilter(IPreTranslateMessage* p);

public:
	BOOL  PreTranslateMessage(MSG* pMsg);
	
protected:
    UIList<IPreTranslateMessage*> m_list;
};

}

#endif  // MSGHELPER_H_C82B8B2A_60AF_4321_A92F_E6A2D39DCB40