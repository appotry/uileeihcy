#pragma once
#include "App\IM\include\main_inc.h"
namespace IM
{
    class OpenPrivateChatDlgReqTask;
    struct IPrivateChatBiz;
}
class CMainBiz : public IM::IMainBiz
{
public:
    CMainBiz();
    ~CMainBiz();

public:
    virtual long  ExecuteCommand(long nCommand, WPARAM wParam, LPARAM lParam);

    virtual long  ShowDialog();
    virtual long  HideDialog();

public:
    long  OnQuit();
    long  OpenPrivateChatDlg(IM::OpenPrivateChatDlgReqTask* p);

private:

    IM::IFramework*  m_pFrameWork;

    IM::IPrivateChatBiz*  m_pPrivateChatBiz;
};