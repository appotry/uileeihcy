#pragma once
#include <shobjidl.h>  // IDropTargetHelper
namespace UI
{

class WindowBase;
class Object;

class WindowDragDropMgr : public IDropTarget
{
public:
    WindowDragDropMgr();
    ~WindowDragDropMgr();

public:
    void  SetWindowBase(WindowBase* p);
    
    HRESULT  SetCanDrop(bool b);
    void  OnWindowDestroy();

protected:
    // IUnknown Interface
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject);
    virtual ULONG STDMETHODCALLTYPE AddRef(void);
    virtual ULONG STDMETHODCALLTYPE Release(void);

protected:
    // IDropTarget Interface 
    virtual HRESULT STDMETHODCALLTYPE DragEnter(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect);
    virtual HRESULT STDMETHODCALLTYPE DragOver(DWORD grfKeyState, POINTL pt, DWORD *pdwEffect);
    virtual HRESULT STDMETHODCALLTYPE DragLeave(void);
    virtual HRESULT STDMETHODCALLTYPE Drop(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect);

    HRESULT  DragMove(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect);
    HRESULT  DoDropTargetNotify(Object* pHoverObj, DROPTARGETEVENT_TYPE eEventType, DROPTARGETEVENT_DATA* pData);

    Object*  GetHoverObject2Drop();

private:
    WindowBase*  m_pWindowBase;
    
    IDropTargetHelper*  m_pDropTargetHelper;  // ������ʾ��קʱ��ͼ��
    IDataObject*  m_pDragingDataObj;   // ���ڽ��DragOverʱû���õ�DataObjec������
    Object*  m_pObjHover;              // ��¼��ǰhover�������ڸ�hover������enter/leave/hover��Ϣ��[ע: drag��enter/leaveֻ����Դ��ڵ�]

    ULONG  m_lRef;
    long   m_lCanDrop;  // ����0��ʾ������ק
};

}