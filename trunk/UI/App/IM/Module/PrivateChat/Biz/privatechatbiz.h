#pragma once
#include "App\IM\include\privatechat_inc.h"

class CPrivateChatBiz : public IM::IPrivateChatBiz
{
public:
    CPrivateChatBiz();
    ~CPrivateChatBiz();

public:
    virtual long  ExecuteCommand(long nCommand, WPARAM wParam, LPARAM lParam);

public:
    long  OpenPrivateChatDlg(IM::OpenPrivateChatDlgReqTask* p);

private:

    IM::IFramework*  m_pFrameWork;
};